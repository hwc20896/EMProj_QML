#pragma once

#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <memory>
#include "database.hpp"
#include <chrono>

class ManagementBackend final : public QObject {
    Q_OBJECT

    //  Qt Properties
    Q_PROPERTY(int correctCount READ correctCount NOTIFY correctCountChanged)
    Q_PROPERTY(int incorrectCount READ incorrectCount NOTIFY incorrectCountChanged)
    Q_PROPERTY(QVariant currentQuestion READ currentQuestion NOTIFY currentQuestionChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(int currentQuestionIndex WRITE setCurrentQuestionIndex READ getCurrentQuestionIndex NOTIFY currentQuestionIndexChanged)
    Q_PROPERTY(bool isMuted WRITE setCurrentMuted READ currentMuted NOTIFY currentMutedChanged)
    Q_PROPERTY(bool isPopWrongEnabled READ isPopWrongEnabled NOTIFY isPopWrongEnabledChanged)
    Q_PROPERTY(bool isReviveEnabled READ isReviveEnabled NOTIFY isReviveEnabledChanged)
    public:
        [[nodiscard]] int correctCount() const;
        [[nodiscard]] int incorrectCount() const;
        [[nodiscard]] QVariant currentQuestion() const;
        [[nodiscard]] int progress() const;
        [[nodiscard]] int getCurrentQuestionIndex() const;
        [[nodiscard]] bool currentMuted() const;
        [[nodiscard]] bool isPopWrongEnabled() const;
        [[nodiscard]] bool isReviveEnabled() const;
    public slots:
        void setCurrentQuestionIndex(int index);
        void setCurrentMuted(bool muted);
        void setCurrentReviveEnabled(bool enabled);

        void onQuestionDataReady(const QList<QuestionData>& q);

    signals:
        void correctCountChanged();
        void incorrectCountChanged();
        void currentQuestionChanged();
        void progressChanged();
        void currentQuestionIndexChanged();
        void currentMutedChanged();
        void isPopWrongEnabledChanged();
        void currentAnsweredChanged();
        void isReviveEnabledChanged();

    //  Class methods
    public:
        explicit ManagementBackend(QObject* parent = nullptr);
        ~ManagementBackend() override;
        Q_INVOKABLE void initialize();
        Q_INVOKABLE void finalize();
        Q_INVOKABLE void startTimer();
        void endTimer();
    private:
        int correctCount_, incorrectCount_;
        std::unique_ptr<QSoundEffect> correctSound_, incorrectSound_;
        std::unique_ptr<QMediaPlayer> player_;
        std::unique_ptr<QAudioOutput> audioOutput_;
        std::unique_ptr<QSoundEffect> swoon_;

        std::chrono::steady_clock::time_point questionStartTime_, questionEndTime_;

        int currentQuestionIndex_ = -1;
        int progress_ = 0;
        bool currentMuted_ = true;
        int totalElapsedMS_ = 0;

        QList<QuestionData> questionList_;
        void clearQuestions();

    //  Task 3
        bool isPopWrongEnabled_ = false;
        bool isReviveEnabled_ = false;
        int consecutivePopWrong_ = 0;
        int consecutiveRevive_ = 0;

    //  Invokable
    public:
        Q_INVOKABLE void playCorrect() const;
        Q_INVOKABLE void playIncorrect() const;

        Q_INVOKABLE void startBackground() const;
        Q_INVOKABLE void stopBackground() const;

        Q_INVOKABLE void playSwoon() const;
        Q_INVOKABLE QString revokeMatch();

    //  Task 2
        [[nodiscard]] Q_INVOKABLE QVariant getRevivalQuestion() const;

    //  Database
        Q_INVOKABLE void loadQuestions(int quantity);
        Q_INVOKABLE void handleAnswer(const QString &answer);

    //  Outro
        [[nodiscard]] Q_INVOKABLE QString getElapsedTime() const;
        [[nodiscard]] Q_INVOKABLE QList<QVariant> getSessionQuestionData() const;
};
