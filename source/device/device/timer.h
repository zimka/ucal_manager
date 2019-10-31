#ifndef UCAL2_DEVICE_TIMER_H
#define UCAL2_DEVICE_TIMER_H

#include <chrono>
#include "common/timestamp.h"

#include <memory>

namespace common {
    class Config;

    using ConfigPtr = std::shared_ptr<Config>;
}

namespace device {
    class DeviceTimer {
    public:
        DeviceTimer(common::TimeUnit step);

        common::TimeStamp getStamp() const;

        bool setStep(common::TimeUnit step);

        bool run();

        bool stop();

        bool setOverdue(common::TimeUnit overdue);

        bool isOverdue() const;

        bool isRunning() const;

        void reconfigure(common::ConfigPtr const& config);

        double takeMultiplier() const;

        double unitsToMilliseconds(common::TimeUnit unit) const;

        common::TimeUnit millisecondsToUnits(double interval) const;

    private:
        bool is_running_ = false;
        common::TimeUnit step_;
        common::TimeUnit overdue_;
        std::chrono::steady_clock clock_;
        std::chrono::time_point<std::chrono::steady_clock> start_;
        double timeMultiplier = 1;
    };
}
#endif//UCAL2_DEVICE_TIMER_H
