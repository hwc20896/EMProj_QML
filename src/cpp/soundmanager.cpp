#include "soundmanager.hpp"
#include "constants.hpp"

#define SOUND_EFFECT_INITIALIZER std::make_unique<QSoundEffect>()

SoundManager& SoundManager::instance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager(QObject* parent)
  : QObject(parent),
    correctSound_(SOUND_EFFECT_INITIALIZER),
    incorrectSound_(SOUND_EFFECT_INITIALIZER),
    swoon_(SOUND_EFFECT_INITIALIZER),
    openRule_(SOUND_EFFECT_INITIALIZER),
    closeRule_(SOUND_EFFECT_INITIALIZER),
    abilitySound_(SOUND_EFFECT_INITIALIZER),
    clickSound_(SOUND_EFFECT_INITIALIZER),
    couldNotSelectSound_(SOUND_EFFECT_INITIALIZER),
    reviveActivatingSound_(SOUND_EFFECT_INITIALIZER),
    reviveDeactivatingSound_(SOUND_EFFECT_INITIALIZER),
    gameAudioOutput_(std::make_unique<QAudioOutput>()),
    reviewAudioOutput_(std::make_unique<QAudioOutput>()),
    bgm_during_game_(std::make_unique<QMediaPlayer>()),
    bgm_during_review_(std::make_unique<QMediaPlayer>())
{
    using namespace EMProj_QML_Backend;

    // Warning! The if else statement is not redundant here!
    // DO NOT DELETE!
    if (Literals::USE_OLD_SOUND_EFFECT) {
        correctSound_->setSource({Paths::SND_BINGO_PATH});
        incorrectSound_->setSource({Paths::SND_OHNO_PATH});
    }
    else {
        correctSound_->setSource({Paths::SND_WON_PATH});
        incorrectSound_->setSource({Paths::SND_ERROR_PATH});
        correctSound_->setVolume(0.5f);
        incorrectSound_->setVolume(0.5f);
    }

    swoon_->setSource({Paths::SND_SLASH_PATH});
    swoon_->setVolume(0.7f);

    openRule_->setSource({Paths::SND_RULE_OPEN_PATH});
    openRule_->setVolume(0.5f);

    closeRule_->setSource({Paths::SND_RULE_CLOSE_PATH});
    closeRule_->setVolume(0.5f);

    abilitySound_->setSource({Paths::SND_ABILITY_AVAILABLE_PATH});
    abilitySound_->setVolume(0.7f);

    clickSound_->setSource({Paths::SND_STATUS_ALTER});
    clickSound_->setVolume(0.7f);

    couldNotSelectSound_->setSource({Paths::SND_NOT_UNLOCKED});
    couldNotSelectSound_->setVolume(0.5f);

    // Initialize background music
    gameAudioOutput_->setVolume(0.25f);
    gameAudioOutput_->setMuted(currentMuted_);

    reviewAudioOutput_->setVolume(0.25f);
    reviewAudioOutput_->setMuted(currentMuted_);

    reviveActivatingSound_->setSource({Paths::SND_REVIVE_ACTIVATING});

    reviveDeactivatingSound_->setSource({Paths::SND_REVIVE_DEACTIVATING});

    bgm_during_game_->setAudioOutput(gameAudioOutput_.get());
    bgm_during_game_->setLoops(QMediaPlayer::Infinite);

    // Warning! The if else statement is not redundant here!
    // DO NOT DELETE!
    if (Literals::USE_OLD_BGM) {
        bgm_during_game_->setSource({Paths::BGM_PIZZA});
        bgm_during_game_->setPlaybackRate(0.793);
    }
    else {
        bgm_during_game_->setSource({Paths::BGM_MAXIMIKE});
    }

    bgm_during_review_->setAudioOutput(reviewAudioOutput_.get());
    bgm_during_review_->setLoops(QMediaPlayer::Infinite);
    bgm_during_review_->setSource({Paths::BGM_STATUS_CHECK});
}

SoundManager::~SoundManager() {
    this->stopGameBackground();
    this->stopReviewBackground();
}

bool SoundManager::isMuted() const {
    return currentMuted_;
}

void SoundManager::setMuted(const bool muted) {
    if (muted != currentMuted_) {
        currentMuted_ = muted;
        gameAudioOutput_->setMuted(muted);
        reviewAudioOutput_->setMuted(muted);
        this->clickSound_->play();
        emit isMutedChanged();
    }
}

void SoundManager::startGameBackground() const {
    if (!bgm_during_game_->isPlaying()) {
        bgm_during_game_->play();
    }
}

void SoundManager::stopGameBackground() const {
    if (bgm_during_game_->isPlaying()) {
        bgm_during_game_->stop();
    }
}

void SoundManager::playCorrect() const {
    correctSound_->play();
}

void SoundManager::playIncorrect() const {
    incorrectSound_->play();
}

void SoundManager::playSwoon() const {
    swoon_->play();
}

void SoundManager::openRuleSound() const {
    openRule_->play();
}

void SoundManager::closeRuleSound() const {
    closeRule_->play();
}

void SoundManager::playAbility() const {
    abilitySound_->play();
}

void SoundManager::playClickSound() const {
    clickSound_->play();
}

void SoundManager::playCantSelectSound() const {
    couldNotSelectSound_->play();
}

void SoundManager::startReviewBackground() const {
    bgm_during_review_->play();
}

void SoundManager::stopReviewBackground() const {
    bgm_during_review_->stop();
}

void SoundManager::playReviveActivatingSound() const {
    reviveActivatingSound_->play();
}

void SoundManager::playReviveDeactivatingSound() const {
    reviveDeactivatingSound_->play();
}
