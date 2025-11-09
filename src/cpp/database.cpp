#include "database.hpp"
#include <QString>
#include <QJsonValue>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QFuture>
#include <iostream>
#include <format>

namespace EMProj_QML_Backend {
    Database::Database() {
        do {
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
        } while (false);

        do {
            //  Podium
            if (duckdb_open(PODIUM_DB_FILE, &podium_db_) != DuckDBSuccess)
                throw std::runtime_error("Failed to open Podium DuckDB database.");
            if (duckdb_connect(podium_db_, &podium_conn_) != DuckDBSuccess)
                throw std::runtime_error("Failed to connect to Podium DuckDB database.");

            if (const auto podium_query = "create table if not exists PodiumData(uuid VARCHAR PRIMARY KEY, timeElapsed INTEGER, stamp TIMESTAMP_S default (CURRENT_TIMESTAMP::TIMESTAMP));";
                duckdb_query(podium_conn_, podium_query, nullptr) != DuckDBSuccess)
                throw std::runtime_error("Failed to create podium_data table.");
        } while (false);
    }

    Database::~Database() {
        if (question_conn_) duckdb_disconnect(&question_conn_);
        if (question_db_) duckdb_close(&question_db_);

        if (podium_conn_) duckdb_disconnect(&podium_conn_);
        if (podium_db_) duckdb_close(&podium_db_);
    }

    QString Database::safeQString(const char* str) {
        return str ? QString::fromUtf8(str) : QString();
    }

    void Database::getQuestionData(const int count){
        const auto future = QtConcurrent::run([this, count] {
            return this->getQuestionData_IMPL(count);
        });

        auto watcher = new QFutureWatcher<QList<QuestionData>>(this);
        connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher] {
            try {
                const auto& result = watcher->result<>();
                emit questionDataReady(result);
            }
            catch (const std::exception& e) {
                std::cout << "Nah i'm not going to be done.\n";
                std::cout << e.what();
                emit questionDataError(QString::fromUtf8(e.what()));
            }

            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }

    QList<QuestionData> Database::getQuestionData_IMPL(const int count) const {
        std::cout << "Let me see see.\n";
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
        std::cout << "Yeah i'm done.\n";
        return questions;
    }

    void Database::savePodiumData(const QString& uuid, const int timeElapsed) const {
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