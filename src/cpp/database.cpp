#include "database.hpp"

#include <QFuture>
#include <QtConcurrentRun>
#include <iostream>

namespace EMProj_QML_Backend {
    Database::Database() {
        watcher_ = std::make_unique<QFutureWatcher<void>>(this);
        connect(watcher_.get(), &QFutureWatcher<void>::finished, this, &Database::initializationComplete);

        const QFuture<void> future = QtConcurrent::run([this] {
            this->initializeDatabase();
        });
        watcher_->setFuture(future);
    }

    Database::~Database() {
        if (watcher_ && watcher_->isRunning()) watcher_->waitForFinished();

        if (question_conn_) duckdb_disconnect(&question_conn_);
        if (question_db_) duckdb_close(&question_db_);

        if (podium_conn_) duckdb_disconnect(&podium_conn_);
        if (podium_db_) duckdb_close(&podium_db_);
    }

    void Database::initializeDatabase() {
        try {
            //  Question Database
            if (duckdb_open(nullptr, &question_db_) != DuckDBSuccess)
                throw std::runtime_error("Failed to open DuckDB in-memory database.");
            if (duckdb_connect(question_db_, &question_conn_) != DuckDBSuccess)
                throw std::runtime_error("Failed to connect to DuckDB in-memory database.");
            if (duckdb_query(question_conn_, "install excel; load excel;", nullptr) != DuckDBSuccess)
                throw std::runtime_error("Unable to load Excel extension.");

            const auto query = std::format(
                "CREATE TABLE questions AS SELECT * FROM read_xlsx('{}', header=true, all_varchar = true);",
                EXCEL_FILE
            );

            duckdb_result result;
            if (duckdb_query(question_conn_, query.c_str(), &result) != DuckDBSuccess)
                throw std::runtime_error("Failed to create questions table from Excel file:");
            duckdb_destroy_result(&result);
            std::cout << "[DuckDB] Loaded Excel file into in-memory table successfully.\n";

            // Podium database
            if (duckdb_open(PODIUM_DB_FILE, &podium_db_) != DuckDBSuccess)
                throw std::runtime_error("Failed to open Podium DuckDB database.");
            if (duckdb_connect(podium_db_, &podium_conn_) != DuckDBSuccess)
                throw std::runtime_error("Failed to connect to Podium DuckDB database.");

            static constexpr auto podium_query = "create table if not exists PodiumData(uuid VARCHAR PRIMARY KEY, timeElapsed INTEGER, stamp INTEGER default (CAST(strftime('%s', 'now') AS INTEGER));";

            if (duckdb_query(podium_conn_, podium_query, nullptr) != DuckDBSuccess)
                throw std::runtime_error("Failed to create podium_data table.");

            isInitialized_.store(true, std::memory_order_release);
            std::cout << "[DuckDB] Database initialization completed successfully.\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Database initialization error: " << e.what() << std::endl;
            QMetaObject::invokeMethod(this, [this, e] {
                emit initializationError(QString::fromUtf8(e.what()));
            }, Qt::QueuedConnection);
        }
    }

    QString Database::safeQString(const char* str) {
        return str ? QString::fromUtf8(str) : QString();
    }

    void Database::getQuestionData(const int count){
        if (!isInitialized_.load(std::memory_order_acquire)) {
            emit questionDataError("Database is not initialized yet.");
            return;
        }

        const auto future = QtConcurrent::run([this, count] {
            return this->getQuestionData_IMPL(count);
        });

        auto watcher = std::make_unique<QFutureWatcher<QList<QuestionData>>>(this);
        const auto rawWatcher = watcher.get();

        connect(rawWatcher, &QFutureWatcherBase::finished, this, [this, watcher = std::move(watcher)] mutable {
            try {
                const auto& result = watcher->result<>();
                emit questionDataReady(result);
            }
            catch (const std::exception& e) {
                std::cout << "Error fetching question data.\n";
                std::cout << e.what();
                emit questionDataError(QString::fromUtf8(e.what()));
            }
        });

        rawWatcher->setFuture(future);
    }

    QList<QuestionData> Database::getQuestionData_IMPL(const int count) const {
        if (!isInitialized_.load(std::memory_order_acquire)) {
            throw std::runtime_error("Database is not initialized yet.");
        }

        std::cout << "Fetching question data...\n";
        QList<QuestionData> questions;
        duckdb_result result;
        static constexpr auto query_template = R"(
            SELECT
              COALESCE(ID, '0') :: INTEGER AS id,
              COALESCE(QuestionTitle, '') AS title,
              COALESCE(Options, '[]') AS options_json,
              COALESCE(CorrectOption, '0') :: INTEGER AS correct,
              COALESCE(QuestionType, '') AS q_type,
              COALESCE(OptionType, '') AS o_type,
              COALESCE(Description, '') AS description
            FROM
              questions
            ORDER BY RANDOM()
            LIMIT ?;
        )";

        duckdb_prepared_statement stmt;
        duckdb_prepare(question_conn_, query_template, &stmt);
        duckdb_bind_int32(stmt, 1, count);

        if (duckdb_execute_prepared(stmt, &result) != DuckDBSuccess)
            throw std::runtime_error("Failed to execute prepared statement for fetching question data.");

        const idx_t rows = duckdb_row_count(&result);
        questions.reserve(rows);

        for (idx_t i = 0; i < rows; i++) {
            const auto id = duckdb_value_int32(&result, 0, i);
            const auto title = safeQString(duckdb_value_varchar(&result, 1, i));
            const auto options_json_str = safeQString(duckdb_value_varchar(&result, 2, i));
            const auto correct = duckdb_value_int32(&result, 3, i);
            const auto q_type = safeQString(duckdb_value_varchar(&result, 4, i));
            const auto o_type = safeQString(duckdb_value_varchar(&result, 5, i));
            const auto description = safeQString(duckdb_value_varchar(&result, 6, i));

            questions.emplace_back(id, title, options_json_str, correct, q_type, o_type, description);
        }
        duckdb_destroy_result(&result);
        duckdb_destroy_prepare(&stmt);
        std::cout << "Question data fetched successfully.\n";
        return questions;
    }

    void Database::savePodiumData(const QString& uuid, const int timeElapsed) const {
        if (!isInitialized_.load(std::memory_order_acquire)) {
            std::cerr << "Database is not initialized yet. Cannot save podium data." << std::endl;
            return;
        }

        static constexpr auto insert_query = R"(
            INSERT INTO PodiumData (uuid, timeElapsed)
            VALUES (?, ?)
            ON CONFLICT (uuid) DO UPDATE SET timeElapsed = excluded.timeElapsed;
        )";
        duckdb_prepared_statement stmt;
        duckdb_prepare(podium_conn_, insert_query, &stmt);
        duckdb_bind_varchar(stmt, 1, uuid.toUtf8().constData());
        duckdb_bind_int32(stmt, 2, timeElapsed);

        if (duckdb_execute_prepared(stmt, nullptr) != DuckDBSuccess) {
            throw std::runtime_error("Failed to execute prepared statement for saving podium data.");
        }
    }

}