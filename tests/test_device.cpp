#include <utility>
#include <sstream>
#include <catch2/catch.hpp>
#include <chrono>
#include <thread>
#include "common/timestamp.h"
#include "common/utils.h"
#include "device/timer.h"
#include "device/device.h"
#include "storage/frame.h"

using namespace device;
using namespace common;


void testSleep(common::TimeUnit t) {
    std::this_thread::sleep_for(std::chrono::milliseconds((int) t));
}

bool timeApproxSame(common::TimeUnit ts1, common::TimeUnit ts2, common::TimeUnit delta) {
    common::TimeUnit max = ts2 > ts1 ? ts2 : ts1;
    common::TimeUnit min = ts2 > ts1 ? ts1 : ts2;
    return ((max - min) < delta);
}

TEST_CASE("DeviceTimer") {
    WARN("DeviceTimer: time consuming tests...");
    auto td = TimeUnit(1);

    float sleep_ratio = 100;
    auto sleep_duration = TimeUnit(td * sleep_ratio);

    SECTION("Stopped") {
        DeviceTimer timer(td);
        REQUIRE_THROWS(timer.getStamp().value());
        REQUIRE(!timer.isOverdue());
        REQUIRE(!timer.stop());
    }
    SECTION("Running") {
        DeviceTimer timer(td);
        bool correct = true;
        timer.run();
        for (auto i = 0; i < 20; i++) {
            auto measured = timer.getStamp().value();
            auto target = i * sleep_duration;
            bool correct_current = timeApproxSame(measured, target, td * sleep_ratio / 10);
            testSleep(target + sleep_duration - measured); // runtime correction
            if (correct) {
                correct = correct && correct_current;
                if (!correct) {
                    std::string message = std::to_string(measured) + " instead of " + std::to_string(target);
                    WARN("incorrect at step " + std::to_string(i) + "; Got " + message);
                }
            }
        }
        REQUIRE(correct);
    }
    SECTION("Overdue") {
        float ratio = 100;
        auto sleep_duration = TimeUnit(td * ratio);

        float overdue_ratio = 20;
        auto overdue_ts = sleep_duration * (overdue_ratio + 0.5);//to overdue between steps
        DeviceTimer timer(td);
        timer.setOverdue(overdue_ts);
        timer.run();
        size_t overdue_step = 0;
        for (auto i = 0; i < 2 * overdue_ratio; i++) {
            auto measured = timer.getStamp().value();
            auto target = sleep_duration * i;
            testSleep(target + sleep_duration - measured); //runtime correction
            if (timer.isOverdue()) {
                overdue_step = i;
                break;
            }
        }
        REQUIRE(overdue_step == overdue_ratio);
    }
    SECTION("State machine") {
        bool status = false;
        DeviceTimer timer(td);

        REQUIRE_FALSE(timer.isRunning());
        REQUIRE_FALSE(timer.isOverdue());
        REQUIRE_FALSE(timer.stop());
        REQUIRE_FALSE(timer.isRunning());

        REQUIRE(timer.run());
        REQUIRE(timer.isRunning());

        REQUIRE_FALSE(timer.run());

        REQUIRE_FALSE(timer.setOverdue(td));
        REQUIRE(timer.stop());
        REQUIRE_FALSE(timer.isRunning());
    }
}

TEST_CASE("MockDevice") {
    SECTION("State machine") {
        MockDevice device;
        REQUIRE(device.getState() == +DeviceState::CanSet);
        REQUIRE_NOTHROW(device.stop()); // stop is always safe
        REQUIRE(device.getState() == +DeviceState::CanSet);
        REQUIRE_THROWS(device.run());
        device.prepare();
        REQUIRE(device.getState() == +DeviceState::Prepared);
        device.run();
        REQUIRE(device.getState() == +DeviceState::Running);
        device.stop();
        REQUIRE(device.getState() == +DeviceState::CanSet);
        device.prepare();
        REQUIRE(device.getState() == +DeviceState::Prepared);
        device.stop();
        REQUIRE(device.getState() == +DeviceState::CanSet);
    }
    SECTION("Overdue") {
        MockDevice device;
        TimeUnit sleep_time(100);
        TimeUnit sleep_time_eps(5);
        device.setDuration(sleep_time * 2);
        device.prepare();
        device.run();
        REQUIRE(device.getState() == +DeviceState::Running);
        testSleep(sleep_time);
        REQUIRE(device.getState() == +DeviceState::Running);
        testSleep(sleep_time + sleep_time_eps);
        REQUIRE_FALSE(device.getState() == +DeviceState::Running);
    }
    SECTION("Get data") {
        MockDevice device;
        size_t steps_num = 10;
        size_t reading_sampling = 20;
        TimeUnit sleep_time(reading_sampling * steps_num);
        device.setReadingSampling(reading_sampling);
        device.prepare();
        device.run();
        testSleep(sleep_time);
        storage::Frame f1 = device.getData();
        REQUIRE(common::approxEqual(f1.size(), steps_num));

        testSleep(sleep_time);
        storage::Frame f2 = device.getData();
        REQUIRE(common::approxEqual(f2.size(), steps_num));
    }
}
