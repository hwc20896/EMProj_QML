#pragma once

#include <vector>
#include "questiondata.hpp"
#include "duckdb.h"


namespace EMProj_CMake_Backend {
    class Database final {
        public:
            static Database& instance() {
                static Database instance;
                return instance;
            }

            Database(const Database&) = delete;
            Database(Database&&) = delete;

            Database& operator=(const Database&) = delete;
            Database& operator=(Database&&) = delete;

            void testDatabase() const;

            [[nodiscard]]
            int getQuestionCount(int gamemode) const;

            [[nodiscard]]
            QList<int> getQuestionCount() const;

            [[nodiscard]]
            QList<QuestionData> getQuestions(int gamemode, int count) const;
        private:
            // DuckDB C API handles
            duckdb_database db_ = nullptr;
            duckdb_connection conn_ = nullptr;

            explicit Database();
            ~Database();

            static constexpr auto EXCEL_FILE = "question_data.xlsx";
    };

}
