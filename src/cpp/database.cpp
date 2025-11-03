#include "database.hpp"
#include <QString>
#include <QJsonValue>
#include <QVariant>
#include <iostream>
#include <format>

using namespace EMProj_CMake_Backend;

Database::Database() {
    // 建立 in-memory DuckDB
    if (duckdb_open(nullptr, &db_) != DuckDBSuccess)
        throw std::runtime_error("Failed to open DuckDB in-memory database.");
    if (duckdb_connect(db_, &conn_) != DuckDBSuccess)
        throw std::runtime_error("Failed to connect to DuckDB in-memory database.");

    if (duckdb_query(conn_, "install excel; load excel;", nullptr) != DuckDBSuccess) {
        throw std::runtime_error("Unable to load Excel extension.");
    }

    // 把 Excel 讀進暫存表 QuestionData
    const std::string createTableSQL = std::format(
        "CREATE TABLE QuestionData AS "
        "SELECT * FROM read_xlsx('{}', header = true);",
        EXCEL_FILE
    );

    duckdb_result res;
    if (duckdb_query(conn_, createTableSQL.c_str(), &res) != DuckDBSuccess) {
        const char* msg = duckdb_result_error(&res);
        const std::string err = msg ? msg : "Unknown error loading Excel file.";
        throw std::runtime_error("Failed to load Excel: \n" + err);
    }
    duckdb_destroy_result(&res);

    std::cout << "[DuckDB] Loaded Excel file into in-memory table successfully.\n";
}

Database::~Database() {
    if (conn_) duckdb_disconnect(&conn_);
    if (db_) duckdb_close(&db_);
}

//  It seems this method is useless.
void Database::testDatabase() const {
    duckdb_result result;
    if (duckdb_query(conn_, "SELECT COUNT(*) FROM QuestionData;", &result) == DuckDBSuccess) {
        int count = duckdb_value_int32(&result, 0, 0);
        std::cout << std::format("Question count: {}\n", count);
    }
    duckdb_destroy_result(&result);
}

int Database::getQuestionCount(const int gamemode) const {
    std::string queryStr = "SELECT COUNT(*) FROM QuestionData";
    if (gamemode == 1) queryStr += " WHERE QuestionType = 0";
    else if (gamemode == 2) queryStr += " WHERE QuestionType = 1";

    duckdb_result result;
    int count = 0;
    if (duckdb_query(conn_, queryStr.c_str(), &result) == DuckDBSuccess)
        count = duckdb_value_int32(&result, 0, 0);
    duckdb_destroy_result(&result);
    return count;
}

QList<int> Database::getQuestionCount() const {
    QList<int> resultVec;
    duckdb_result result;
    if (duckdb_query(conn_,
                     "SELECT QuestionType, COUNT(ID) FROM QuestionData GROUP BY QuestionType;",
                     &result) == DuckDBSuccess) {
        const idx_t rows = duckdb_row_count(&result);
        for (idx_t i = 0; i < rows; i++) resultVec.push_back(duckdb_value_int32(&result, 1, i));
    }
    duckdb_destroy_result(&result);
    return resultVec;
}

QList<QuestionData> Database::getQuestions(const int gamemode, const int count) const {
    QList<QuestionData> questions;

    std::string queryStr = "SELECT * FROM QuestionData";
    if (gamemode == 1) queryStr += " WHERE QuestionType = 0";
    else if (gamemode == 2) queryStr += " WHERE QuestionType = 1";
    queryStr += std::format(" ORDER BY RANDOM() LIMIT {};", count);

    duckdb_result result;
    if (duckdb_query(conn_, queryStr.c_str(), &result) != DuckDBSuccess)
        return questions;

    const idx_t rows = duckdb_row_count(&result);
    for (idx_t i = 0; i < rows; i++) {
        const int id             = duckdb_value_int32(&result, 0, i);

        const auto title_c      = duckdb_value_varchar_internal(&result, 1, i);
        const QString title      = QString::fromUtf8(title_c ? title_c : "");

        const auto options_c    = duckdb_value_varchar_internal(&result, 2, i);
        const QString optionsStr = QString::fromUtf8(options_c ? options_c : "");

        const int correctOption  = duckdb_value_int32(&result, 3, i);
        const int questionType   = duckdb_value_int32(&result, 4, i);
        const int optionType     = duckdb_value_int32(&result, 5, i);

        questions.emplace_back(id, title, optionsStr,
                               correctOption, questionType, optionType);
    }
    duckdb_destroy_result(&result);
    return questions;
}
