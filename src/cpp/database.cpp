#include "database.hpp"
#include <QString>
#include <QJsonValue>
#include <QVariant>
#include <iostream>
#include <format>

namespace EMProj_QML_Backend {
    Database::Database() {
        if (duckdb_open(nullptr, &question_db_) != DuckDBSuccess)
            throw std::runtime_error("Failed to open DuckDB in-memory database.");
        if (duckdb_connect(question_db_, &question_conn_) != DuckDBSuccess)
            throw std::runtime_error("Failed to connect to DuckDB in-memory database.");
        if (duckdb_query(question_conn_, "install excel; load excel;", nullptr) != DuckDBSuccess)
            throw std::runtime_error("Unable to load Excel extension.");

        const auto query = std::format(
            "CREATE TABLE questions AS SELECT * FROM read_xlsx({}, header=true);",
            EXCEL_FILE
        );

        duckdb_result result;
        if (duckdb_query(question_conn_, query.c_str(), &result) != DuckDBSuccess)
            throw std::runtime_error("Failed to create questions table from Excel file.");
        duckdb_destroy_result(&result);
        std::cout << "[DuckDB] Loaded Excel file into in-memory table successfully.\n";

        //  Podium
        if (duckdb_open(PODIUM_DB_FILE, &podium_db_) != DuckDBSuccess)
            throw std::runtime_error("Failed to open Podium DuckDB database.");
        if (duckdb_connect(podium_db_, &podium_conn_) != DuckDBSuccess)
            throw std::runtime_error("Failed to connect to Podium DuckDB database.");

        if (const auto podium_query = "create table if not exists PodiumData(UUID VARCHAR PRIMARY KEY, TimeElapsed INT);";
            duckdb_query(podium_conn_, podium_query, nullptr) != DuckDBSuccess)
            throw std::runtime_error("Failed to create podium_data table.");
    }

    Database::~Database() {
        if (question_conn_) duckdb_disconnect(&question_conn_);
        if (question_db_) duckdb_close(&question_db_);

        if (podium_conn_) duckdb_disconnect(&podium_conn_);
        if (podium_db_) duckdb_close(&podium_db_);
    }

    QList<QuestionData> Database::getQuestions(int quantity) const {
        QList<QuestionData> questions;
        duckdb_result result;

        const auto query = std::format(
            "SELECT ID, QuestionTitle, Options, CorrectOption, QuestionType, OptionType FROM questions ORDER BY RANDOM() LIMIT {};",
            quantity
        );

        if (duckdb_query(question_conn_, query.c_str(), &result) != DuckDBSuccess)
            throw std::runtime_error("Failed to retrieve questions from database.");

        const idx_t row_count = duckdb_row_count(&result);
        for (idx_t i = 0; i < row_count; ++i) {
            const auto id = duckdb_value_int32(&result, 0, i);
            const auto questionTitle = QString::fromStdString(
                duckdb_value_varchar(&result, 1, i)
            );
            const auto optionsJson = QString::fromStdString(
                duckdb_value_varchar(&result, 2, i)
            );
            const auto correct = duckdb_value_int64(&result, 3, i);

            const auto questionType = QString::fromStdString(
                duckdb_value_varchar(&result, 4, i)
            );
            const auto optionType = QString::fromStdString(
                duckdb_value_varchar(&result, 5, i)
            );

            questions.emplace_back(
                id,
                questionTitle,
                optionsJson,
                correct,
                questionType,
                optionType
            );
        }

        duckdb_destroy_result(&result);
        return questions;
    }

    void Database::savePodiumData(const QString& uuid, const int timeElapsed) const {
        if (uuid.isEmpty() || timeElapsed < 0) {
            throw std::invalid_argument("Invalid UUID or TimeElapsed value.");
        }

        const auto query = std::format(
            "INSERT INTO PodiumData (UUID, TimeElapsed) VALUES ('{}', {});",
            uuid.toStdString(),
            timeElapsed
        );

        if (duckdb_query(podium_conn_, query.c_str(), nullptr) != DuckDBSuccess) {
            throw std::runtime_error("Failed to insert podium data into database.");
        }
    }


}