#pragma once

#include <QObject>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <memory>
#include "database.hpp"

class ManagementBackend final : public QObject {
    Q_OBJECT

    //  Qt Properties
    Q_PROPERTY(int correctCount READ correctCount NOTIFY correctCountChanged)
    Q_PROPERTY(int incorrectCount READ incorrectCount NOTIFY incorrectCountChanged)
    Q_PROPERTY(QVariant currentQuestion READ currentQuestion NOTIFY currentQuestionChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(int currentQuestionIndex WRITE setCurrentQuestionIndex READ getCurrentQuestionIndex NOTIFY currentQuestionIndexChanged)
    public:
        [[nodiscard]] int correctCount() const;
        [[nodiscard]] int incorrectCount() const;
        [[nodiscard]] QVariant currentQuestion() const;
        [[nodiscard]] int progress() const;
        [[nodiscard]] int getCurrentQuestionIndex() const;
    public slots:
        void setCurrentQuestionIndex(int index);

    signals:
        void correctCountChanged();
        void incorrectCountChanged();
        void currentQuestionChanged();
        void progressChanged();
        void currentQuestionIndexChanged();
        void answerResult(bool isCorrect, const QString& correctAnswer, int id);

    //  Class methods
    public:
        explicit ManagementBackend(QObject* parent = nullptr);
        ~ManagementBackend() override;
        Q_INVOKABLE void initialize();
    private:
        int correctCount_, incorrectCount_;
        std::unique_ptr<QSoundEffect> correctSound_, incorrectSound_;
        std::unique_ptr<QMediaPlayer> player_;
        std::unique_ptr<QAudioOutput> audioOutput_;

        int currentQuestionIndex_ = -1;
        int progress_ = 0;

        QList<QuestionData> questionList_;
        void clearQuestions();

    //  Invokable
    public:
        Q_INVOKABLE void playCorrect() const;
        Q_INVOKABLE void playIncorrect() const;

        Q_INVOKABLE void setBackgroundMuted(bool muted) const;

        Q_INVOKABLE void startBackground() const;
        Q_INVOKABLE void stopBackground() const;

    //  Database
        Q_INVOKABLE void loadQuestions(int gamemode, int quantity);
        Q_INVOKABLE void handleAnswer(const QString& answer, int id);
        Q_INVOKABLE void releaseQuestions();
};
