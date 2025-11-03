#include "templateelement.hpp"

QuestionListModel::QuestionListModel(QObject* parent)
    : QAbstractListModel(parent)
{}

void QuestionListModel::setQuestions(const QList<QuestionData>& questions) {
    this->beginResetModel();
    questions_ = questions;
    this->endResetModel();
}

int QuestionListModel::rowCount(const QModelIndex&) const {
    return static_cast<int>(questions_.size());
}

QVariant QuestionListModel::data(const QModelIndex& index, const int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= questions_.size()) {
        return {};
    }

    const auto& question = questions_.at(index.row());
    switch (role) {
        case QuestionTitleRole:
            return question.questionTitle_;
        case OptionsRole:
            return QVariant::fromValue(question.options_);
        default:
            return {};
    }
}

QHash<int, QByteArray> QuestionListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[QuestionTitleRole] = "questionTitle";
    roles[OptionsRole] = "options";
    return roles;
}
