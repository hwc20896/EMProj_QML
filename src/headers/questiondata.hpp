#pragma once

#include <QJsonValue>
#include <utility>

class QuestionData {
    using string = QString;
    Q_GADGET
    Q_PROPERTY(int id MEMBER id_)
    Q_PROPERTY(QString questionTitle MEMBER questionTitle_)
    Q_PROPERTY(QStringList options MEMBER options_)
    Q_PROPERTY(QString correctText MEMBER correctText_)
    Q_PROPERTY(bool sessionAnswered MEMBER sessionAnswered_)
    Q_PROPERTY(QString sessionSelectedAnswer MEMBER sessionSelectedAnswer_)
    public:
        //  Basic
        int id_ = 0;
        string questionTitle_;
        QStringList options_;
        string correctText_;

        //  Info, useful only on debugging
        string questionType_;
        string optionType_;

        //  Status
        bool sessionAnswered_ = false;
        string sessionSelectedAnswer_ = "";

        explicit QuestionData(const int id,
                              string title,
                              const QString& optionsJson,
                              const int64_t correct,
                              string questionType,
                              string optionType
        ) : id_(id),
            questionTitle_(std::move(title)),
            options_(QJsonValue::fromJson(optionsJson.toLocal8Bit()).toVariant().toStringList()),
            questionType_(std::move(questionType)),
            optionType_(std::move(optionType))
        {
            correctText_ = options_.at(correct);
        }
};