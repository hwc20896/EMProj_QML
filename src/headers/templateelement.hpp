#pragma once

#include <QAbstractListModel>
#include "questiondata.hpp"

class QuestionListModel final : public QAbstractListModel {
    Q_OBJECT
    public:
        enum QuestionRoles {
            QuestionTitleRole = Qt::UserRole + 1,
            OptionsRole
        };

        explicit QuestionListModel(QObject* parent = nullptr);
        void setQuestions(const QList<QuestionData>& questions);

        [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
        [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
        [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    private:
        QList<QuestionData> questions_;
};