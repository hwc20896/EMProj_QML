#pragma once

#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <memory>
#include <QObject>

class SoundManager final : public QObject {
    Q_OBJECT

    // Qt Properties
    Q_PROPERTY(bool isMuted WRITE setMuted READ isMuted NOTIFY isMutedChanged)

public:
    // Property getters
    [[nodiscard]] bool isMuted() const;

public slots:
    // Property setters
    void setMuted(bool muted);

signals:
    void isMutedChanged();

public:
    // Singleton pattern for global access
    static SoundManager& instance();

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    
    // Destructor
    ~SoundManager() override;

    // Background music control
    Q_INVOKABLE void startGameBackground() const;
    Q_INVOKABLE void stopGameBackground() const;
    Q_INVOKABLE void startReviewBackground() const;
    Q_INVOKABLE void stopReviewBackground() const;

    // Sound effects
    Q_INVOKABLE void playCorrect() const;
    Q_INVOKABLE void playIncorrect() const;
    Q_INVOKABLE void playSwoon() const;
    Q_INVOKABLE void openRuleSound() const;
    Q_INVOKABLE void closeRuleSound() const;
    Q_INVOKABLE void playAbility() const;
    Q_INVOKABLE void playClickSound() const;
    Q_INVOKABLE void playCantSelectSound() const;
    Q_INVOKABLE void playReviveActivatingSound() const;
    Q_INVOKABLE void playReviveDeactivatingSound() const;

private:
    // Private constructor for singleton
    explicit SoundManager(QObject* parent = nullptr);

    // Sound effect players
    std::unique_ptr<QSoundEffect> correctSound_,
                                  incorrectSound_,
                                  swoon_,
                                  openRule_,
                                  closeRule_,
                                  abilitySound_,
                                  clickSound_,
                                  couldNotSelectSound_,
                                  reviveActivatingSound_,
                                  reviveDeactivatingSound_;

    // Background music player
    std::unique_ptr<QAudioOutput> gameAudioOutput_,
                                  reviewAudioOutput_;
    std::unique_ptr<QMediaPlayer> bgm_during_game_,
                                  bgm_during_review_;

    // Current muted state
    bool currentMuted_ = true;
};