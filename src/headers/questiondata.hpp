#pragma once

#include <QJsonValue>
#include <utility>
#include <random>

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
        int64_t sessionTimeSpentMs_ = 0LL;

        explicit QuestionData(const int id,
                              string title,
                              const QString& optionsJson,
                              const int64_t correct,
                              string questionType,
                              string optionType
        ) : id_(id),
            questionTitle_(std::move(title)),
            options_(QJsonValue::fromJson(optionsJson.toUtf8()).toVariant().toStringList()),
            questionType_(std::move(questionType)),
            optionType_(std::move(optionType))
        {
            correctText_ = options_.at(correct);

            static std::random_device rd;
            static std::mt19937 g(rd());
            if (options_.size() == 2) return;
            std::ranges::shuffle(options_, g);
        }
};