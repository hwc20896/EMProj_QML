#include "managementbackend.hpp"
using EMProj_QML_Backend::Database;
#include <iostream>
#include <QUuid>

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
void ManagementBackend::loadQuestions(int, const int quantity) {
    questionList_.clear();
    currentQuestionIndex_ = 0;

    connect(&Database::instance(), &Database::questionDataReady, this, &ManagementBackend::onQuestionDataReady, Qt::UniqueConnection);

    Database::instance().getQuestionData(quantity);
}

void ManagementBackend::onQuestionDataReady(const QList<QuestionData>& q) {
    questionList_ = q;
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
    if (currentQuestionIndex_ < 0 || currentQuestionIndex_ >= questionList_.size()) return;

    auto& question = questionList_[currentQuestionIndex_];
    if (question.sessionAnswered_) return;

    question.sessionAnswered_ = true;
    emit currentAnsweredChanged();
    this->endTimer();

    question.sessionSelectedAnswer_ = answer;

    if (answer == question.correctText_) {
        correctCount_++;
        emit correctCountChanged();
        this->playCorrect();
    } else {
        incorrectCount_++;
        emit incorrectCountChanged();
        this->playIncorrect();
    }

    //  Update progress
    progress_++;
    emit progressChanged();
}

void ManagementBackend::finalize() const {
    this->stopBackground();

    const auto correct = std::ranges::count_if(questionList_, [](const QuestionData& i){return i.correctText_ == i.sessionSelectedAnswer_;});
    std::cout << "Correct answers: " << correct << " out of " << questionList_.size() << '\n';

    const bool ok = std::ranges::all_of(questionList_, [](const QuestionData& i){return i.correctText_ == i.sessionSelectedAnswer_;});
    std::cout << "Can be inserted to podium: " << std::boolalpha << ok << '\n';

    const int64_t totalTime = std::accumulate(questionList_.begin(), questionList_.end(), 0LL,
        [](const int64_t acc, const QuestionData& q){ return acc + q.sessionTimeSpentMs_; });
    std::cout << "Total time spent (ms): " << totalTime << '\n';

    if (ok) {
        const auto uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        constexpr auto timeElapsed = 8000; //  Placeholder time elapsed
        Database::instance().savePodiumData(uuid, timeElapsed);
    }
}

void ManagementBackend::startTimer() {
    questionStartTime_ = std::chrono::steady_clock::now();
}

void ManagementBackend::endTimer() {
    questionEndTime_ = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(questionEndTime_ - questionStartTime_).count();

    if (currentQuestionIndex_ >= 0 && currentQuestionIndex_ < questionList_.size()) {
        questionList_[currentQuestionIndex_].sessionTimeSpentMs_ = duration;
        std::cout << "Question ID " << questionList_[currentQuestionIndex_].id_ << " answered in " << duration << " ms.\n";
    }
}