#pragma once

#include <QString>
#include <QDateTime>
#include <utility>
#include <format>

class Podium {
    using string = QString;
    Q_GADGET
    Q_PROPERTY(QString uuid MEMBER uuid_)
    Q_PROPERTY(QString timeElapsed MEMBER timeElapsed_)
    Q_PROPERTY(QDateTime stamp MEMBER stamp_)
    public:
        string uuid_;
        string timeElapsed_;
        QDateTime stamp_;

        explicit Podium(
            string uuid,
            int64_t timeElapsed,
            int64_t stamp
        ) : uuid_(std::move(uuid)), timeElapsed_(QString::fromStdString(std::format("{:%M:%S}", timeElapsed))) {
            stamp_ = QDateTime::fromSecsSinceEpoch(stamp);
        }
};