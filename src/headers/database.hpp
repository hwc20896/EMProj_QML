#pragma once

#include <vector>
#include "questiondata.hpp"
#include "duckdb.h"
#include <future>

namespace EMProj_QML_Backend {
    class Database final : public QObject {
        Q_OBJECT
        public:
            static Database& instance() {
                static Database instance;
                return instance;
            }

            Database(const Database&) = delete;
            Database(Database&&) = delete;

            Database& operator=(const Database&) = delete;
            Database& operator=(Database&&) = delete;
        private:
            explicit Database();
            ~Database() override;

        //  QuestionData
        public:
            void getQuestionData(int count);

        signals:
            void questionDataReady(const QList<QuestionData>& data);
            void questionDataError(const QString& error);

        private:
            [[nodiscard]] QList<QuestionData> getQuestionData_IMPL(int count) const;
            [[nodiscard]] static QString safeQString(const char* str);
            duckdb_database question_db_ = nullptr;
            duckdb_connection question_conn_ = nullptr;
            static constexpr auto EXCEL_FILE = "question_data.xlsx";

        //  Podium Data
        public:
            /*  Saves the podium data with the given UUID and time elapsed in milliseconds. */
            void savePodiumData(const QString& uuid, int timeElapsed) const;
            duckdb_database podium_db_ = nullptr;
            duckdb_connection podium_conn_ = nullptr;
            static constexpr auto PODIUM_DB_FILE = "podium_data.db";
    };

}
