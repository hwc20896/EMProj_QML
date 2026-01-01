#include "managementbackend.hpp"
#include <iostream>
#include <QUuid>
#include <format>
#include <algorithm>
#include <ranges>
#include <QDebug>
#include <print>

#include "constants.hpp"
using EMProj_QML_Backend::Database;

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

bool ManagementBackend::currentMuted() {
    return SoundManager::instance().isMuted();
}

bool ManagementBackend::isPopWrongEnabled() const {
    return isPopWrongEnabled_;
}

bool ManagementBackend::isReviveEnabled() const {
    return isReviveEnabled_;
}

QString ManagementBackend::revokeMatch() {
    const auto currentQuestion = questionList_.at(currentQuestionIndex_);
    if (!isPopWrongEnabled_ || currentQuestion.sessionAnswered_) return "";
    if (currentQuestion.options_.size() == 2) {
        std::println("Pop Wrong is not supported in True or False questions");
        return "";
    }

    isPopWrongEnabled_ = false;
    consecutivePopWrong_ = 0;
    emit isPopWrongEnabledChanged();

    thread_local std::mt19937 mt{std::random_device{}()};

    QStringList wrongOptions;
    wrongOptions.reserve(3);
    std::ranges::remove_copy(currentQuestion.options_, std::back_inserter(wrongOptions), currentQuestion.correctText_);
    std::uniform_int_distribution dist(0LL, wrongOptions.size()-1);
    const auto targetWrongOption = wrongOptions.at(dist(mt));
    qDebug() << "Popping option: " << targetWrongOption;
    return targetWrongOption;
}

void ManagementBackend::setCurrentQuestionIndex(const int index) {
    if (index >= 0 && index < questionList_.size() && index != currentQuestionIndex_) {
        currentQuestionIndex_ = index;
        emit currentQuestionIndexChanged();
        emit currentQuestionChanged();
    }
}

void ManagementBackend::setCurrentMuted(const bool muted) {
    if (auto& soundManager = SoundManager::instance();
        muted != soundManager.isMuted()
    ) {
        soundManager.setMuted(muted);
        emit currentMutedChanged();
    }
}

void ManagementBackend::setCurrentReviveEnabled(const bool enabled) {
    if (enabled == isPopWrongEnabled_) return;
    isPopWrongEnabled_ = enabled;
    emit isPopWrongEnabledChanged();
}

//  Class methods
ManagementBackend::ManagementBackend(QObject* parent)
    : QObject(parent), correctCount_(0), incorrectCount_(0)
{
    static_cast<void>(Database::instance());
    static_cast<void>(SoundManager::instance());
}

ManagementBackend::~ManagementBackend() {
    this->clearQuestions();
}

//  Database
void ManagementBackend::loadQuestions(const int quantity) {
    questionList_.clear();
    currentQuestionIndex_ = 0;
    correctCount_ = 0;
    incorrectCount_ = 0;
    progress_ = 0;
    isPopWrongEnabled_ = false;
    isReviveEnabled_ = false;
    consecutivePopWrong_ = 0;
    consecutiveRevive_ = 0;

    emit currentQuestionIndexChanged();
    emit correctCountChanged();
    emit incorrectCountChanged();
    emit progressChanged();
    emit isPopWrongEnabledChanged();
    emit isReviveEnabledChanged();

    connect(&Database::instance(), &Database::questionDataReady, this, &ManagementBackend::onQuestionDataReady, Qt::UniqueConnection);

    Database::instance().getQuestionData(quantity);
}

void ManagementBackend::onQuestionDataReady(const QList<QuestionData>& q) {
    questionList_ = q;
    emit currentQuestionChanged();
}

void ManagementBackend::clearQuestions() {
    questionList_.clear();
    currentQuestionIndex_ = -1;
}

void ManagementBackend::handleAnswer(const QString& answer) {
    if (currentQuestionIndex_ < 0 || currentQuestionIndex_ >= questionList_.size()) return;

    auto& question = questionList_[currentQuestionIndex_];
    if (question.sessionAnswered_) return;

    question.sessionAnswered_ = true;
    emit currentAnsweredChanged();
    this->endTimer();

    question.sessionSelectedAnswer_ = answer;

    //  Update progress
    progress_++;
    emit progressChanged();

    if (answer == question.correctText_) {
        correctCount_++;
        emit correctCountChanged();
        SoundManager::instance().playCorrect();

        consecutiveRevive_++;
        if (question.sessionTimeSpentMs_ < 7000) {
            consecutivePopWrong_++;
        }
    } else {
        incorrectCount_++;
        emit incorrectCountChanged();
        SoundManager::instance().playIncorrect();
        consecutivePopWrong_ = 0;
        consecutiveRevive_ = 0;
    }

}

void ManagementBackend::finalize(){
    SoundManager::instance().stopGameBackground();

    const auto correct = std::ranges::count_if(questionList_, funcCurrentQuestionCorrect);
    std::println("Correct answers: {} out of {}", correct, questionList_.size());

    const bool ok = correct == questionList_.size();
    std::println("Can be inserted to podium: {}", ok);

    const auto totalTime = std::accumulate(questionList_.begin(), questionList_.end(), 0,
        [](const int64_t acc, const QuestionData& q){ return acc + q.sessionTimeSpentMs_; });
    std::println("Total time spent (ms): {}", totalTime);

    totalElapsedMS_ = totalTime;

    if (!ok) return;
    const auto uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    Database::instance().savePodiumData(uuid, totalTime);
}

void ManagementBackend::startTimer() {
    questionStartTime_ = std::chrono::steady_clock::now();
}

void ManagementBackend::endTimer() {
    questionEndTime_ = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(questionEndTime_ - questionStartTime_).count();

    if (currentQuestionIndex_ >= 0 && currentQuestionIndex_ < questionList_.size()) {
        auto& current = questionList_[currentQuestionIndex_];
        current.sessionTimeSpentMs_ = duration;
        std::println("Question ID {} answered in {} ms.", current.id_, duration);
    }
}

QString ManagementBackend::getElapsedTime() const {
    return QString::fromStdString(
        std::format("{:%M:%S}", std::chrono::milliseconds(totalElapsedMS_))
    );
}

QList<QVariant> ManagementBackend::getSessionQuestionData() const {
    auto result = questionList_ | std::views::transform([](const QuestionData& data){return QVariant::fromValue(data);}) | std::ranges::to<QList<QVariant>>();
    return result;
}

QVariant ManagementBackend::getRevivalQuestion() const {
    if (incorrectCount_ == 0) return {};
    thread_local std::mt19937 mt{std::random_device{}()};

    QList<QuestionData> candidates;
    std::ranges::remove_copy_if(questionList_, std::back_inserter(candidates), funcCurrentQuestionCorrect);

    std::uniform_int_distribution<int64_t> dist(0, candidates.size()-1);
    const auto& target = candidates.at(dist(mt));
    return QVariant::fromValue(target);
}

void ManagementBackend::nextPageCheck() {
    const bool a = consecutiveRevive_ >= 5 && !isReviveEnabled_;
    const bool b = consecutivePopWrong_ >= 3 && !isPopWrongEnabled_;

    if (a || b) SoundManager::instance().playAbility();

    if (a) {
        isReviveEnabled_ = true;
        emit isReviveEnabledChanged();
        std::println("isReviveEnabled up");
    }

    if (b) {
        isPopWrongEnabled_ = true;
        emit isPopWrongEnabledChanged();
        std::println("isPopWrongEnabled up");
    }
}
