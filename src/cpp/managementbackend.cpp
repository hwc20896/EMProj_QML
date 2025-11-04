#include "managementbackend.hpp"
using EMProj_CMake_Backend::Database;

//  Qt Properties
int ManagementBackend::correctCount() const {
    return correctCount_;
}

int ManagementBackend::incorrectCount() const {
    return incorrectCount_;
}

QVariant ManagementBackend::currentQuestion() const {
    if (currentQuestionIndex_ >= 0 && currentQuestionIndex_ < questionList_.size()) {
        const auto item = QVariant::fromValue(questionList_.at(currentQuestionIndex_));
        return item;
    }
    return {};
}

int ManagementBackend::progress() const {
    return progress_;
}

int ManagementBackend::getCurrentQuestionIndex() const {
    return currentQuestionIndex_;
}

bool ManagementBackend::currentMuted() const {
    return currentMuted_;
}

void ManagementBackend::setCurrentQuestionIndex(const int index) {
    if (index >= 0 && index < questionList_.size() && index != currentQuestionIndex_) {
        currentQuestionIndex_ = index;
        emit currentQuestionIndexChanged();
        emit currentQuestionChanged();
    }
}

void ManagementBackend::setCurrentMuted(const bool muted) {
    if (muted != currentMuted_) {
        currentMuted_ = muted;
        audioOutput_->setMuted(muted);
        emit currentMutedChanged();
    }
}

//  Class methods
ManagementBackend::ManagementBackend(QObject* parent)
    : QObject(parent), correctCount_(0), incorrectCount_(0)
{
    correctSound_ = std::make_unique<QSoundEffect>();
    correctSound_->setSource({"qrc:/sounds/sounds/bingo.wav"});
    incorrectSound_ = std::make_unique<QSoundEffect>();
    incorrectSound_->setSource({"qrc:/sounds/sounds/ohno.wav"});

    audioOutput_ = std::make_unique<QAudioOutput>();
    audioOutput_->setVolume(0.15f);

    player_ = std::make_unique<QMediaPlayer>();
    player_->setAudioOutput(audioOutput_.get());
    player_->setSource({"qrc:/sounds/sounds/OMFG_Pizza.mp3"});
    player_->setLoops(QMediaPlayer::Infinite);
}

ManagementBackend::~ManagementBackend() {
    if (player_->isPlaying()) {
        player_->stop();
    }
    this->clearQuestions();
}

void ManagementBackend::playCorrect() const {
    correctSound_->play();
}

void ManagementBackend::playIncorrect() const {
    incorrectSound_->play();
}

void ManagementBackend::initialize() {
    correctCount_ = 0;
    incorrectCount_ = 0;
    this->startBackground();
}

void ManagementBackend::startBackground() const {
    if (player_->playbackState() != QMediaPlayer::PlayingState) {
        player_->play();
    }
}

void ManagementBackend::stopBackground() const {
    if (player_->playbackState() == QMediaPlayer::PlayingState) {
        player_->stop();
    }
}

//  Database
void ManagementBackend::loadQuestions(int gamemode, int quantity) {
    questionList_.clear();
    currentQuestionIndex_ = 0;

    questionList_ = Database::instance().getQuestions(gamemode, quantity);
    emit currentQuestionChanged();
}

void ManagementBackend::releaseQuestions() {
    this->clearQuestions();
}

void ManagementBackend::clearQuestions() {
    questionList_.clear();
    currentQuestionIndex_ = -1;
    emit currentQuestionChanged();
}

void ManagementBackend::handleAnswer(const QString& answer, const int id) {
    const auto current = questionList_.at(currentQuestionIndex_);
    const auto correctText = current.options_.at(current.correctOption_);
    const bool isCorrect = answer == correctText && id == current.id_;
    if (isCorrect) {
        correctCount_++;
        this->playCorrect();
        emit correctCountChanged();
    } else {
        incorrectCount_++;
        this->playIncorrect();
        emit incorrectCountChanged();
    }
    progress_ = correctCount_ + incorrectCount_;
    emit progressChanged();
    emit answerResult(isCorrect, correctText, current.id_);
}



