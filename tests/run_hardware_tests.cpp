#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <iostream>

#include <thread>
#include "device/device.h"
#include "common/keys.h"
#include "common/config.h"
using namespace device;

/*
These tests suppose that
1) DaqboardDevice is compiled (we are on win32)
2) hardware is available (plugged in and drivers are installed on PC)
3) hardware controls and signals are shortcut to each other as:
- Uref to Vm;
- Umod to Vg;
- Utpl to ground
*/

int main(int argc, char* argv[]) {
    int result = Catch::Session().run(argc, argv);
    system("pause");
    return result;
}
void testSleep(common::TimeUnit t) {
    std::this_thread::sleep_for(std::chrono::milliseconds((int)t));
}

bool isEqualConstValue(storage::SignalData signal, storage::SignalValue value, storage::SignalValue eps = 1e-2, size_t outliers_allowed = 4) {
    size_t error_count = 0;
    for (auto pos = 0; pos < signal.size(); pos++) {
        auto max = value > signal[pos] ? value : signal[pos];
        auto min = value < signal[pos] ? value : signal[pos];
        auto rel_error = double(max - min) / (max + min);
        if (rel_error > eps) {
            error_count += 1;
        }
    }
    return (error_count < outliers_allowed);
}

bool isApproxEqual(int32_t target, int32_t measured, int32_t tolerance) {
    auto diff = target - measured;
    return ((diff < tolerance) && (diff > -tolerance));
}

TEST_CASE("Hardware") {
    auto second = common::TimeUnit(1000);

    SECTION("State machine") {
        DaqboardDevice device;
        REQUIRE(device.getState() == +DeviceState::CanSet);
        REQUIRE_THROWS(device.getData());
        REQUIRE_THROWS(device.run());
        REQUIRE_NOTHROW(device.prepare());
        REQUIRE(device.getState() == +DeviceState::Prepared);
        REQUIRE_NOTHROW(device.stop());
        REQUIRE(device.getState() == +DeviceState::CanSet);

        REQUIRE_NOTHROW(device.prepare());
        REQUIRE(device.getState() == +DeviceState::Prepared);
        REQUIRE_THROWS(device.getData());
        REQUIRE_NOTHROW(device.run());
        REQUIRE(device.getState() == +DeviceState::Running);
        REQUIRE_NOTHROW(device.getData());
        REQUIRE_NOTHROW(device.stop());
        REQUIRE(device.getState() == +DeviceState::CanSet);
    }
    SECTION("Test isEqualConstValue") {
        const int len = 10;
        storage::SignalValue values[len] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
        storage::SignalData sig(values, len);
        REQUIRE(isEqualConstValue(sig, 1.));
        REQUIRE(!isEqualConstValue(sig, 1.2));
        REQUIRE(!isEqualConstValue(sig, 0.8));
    }
    SECTION("Basic io") {
        DaqboardDevice device;
        device.setProfiles({
            {common::ControlKey::Vg, { 1000, 1000}},
            {common::ControlKey::Vm, { 1000, 1000}},
            }, second);
        REQUIRE_NOTHROW(device.prepare());
        device.run();
        testSleep(second);
        storage::Frame frame = device.getData();
        device.stop();
        REQUIRE(frame.size() > 0);
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 1000.));
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Umod], 1000.));
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Utpl], 0.));
    }
    SECTION("Zero when not specified") {
        DaqboardDevice device;
        device.prepare();
        device.run();
        testSleep(second);
        storage::Frame frame = device.getData();
        device.stop();
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 0.));
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Umod], 0.));
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Utpl], 0.));
    }
    SECTION("Zero after destruction") {
        {
            DaqboardDevice device;
            device.setProfiles({
                {common::ControlKey::Vg, {1000, 1000}}
                }, second);
            device.prepare();
            device.run();
            testSleep(second);
            //device was not stopped, but desctructor called
        }
        {
            DaqboardDevice device;
            device.prepare();
            device.run();
            testSleep(second);
            auto frame = device.getData();
            // Check that all signals are zero
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 0.));
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Umod], 0.));
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Utpl], 0.));
        }
    }
    SECTION("Single channel write") {
        {
            DaqboardDevice device;
            device.setProfiles({
                {common::ControlKey::Vg, { 1000, 1000 }},
                }, second);
            device.prepare();
            device.run();
            testSleep(second);
            storage::Frame frame = device.getData();
            INFO("Vg should be shortcuted to Umod, Utpl to the ground");
            INFO(frame.repr());
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Umod], 1000.));
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 0.));
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Utpl], 0.));
        }
        {
            DaqboardDevice device;
            device.setProfiles({
                {common::ControlKey::Vm, { 1000, 1000 }},
                }, second); device.prepare();
            device.run();
            testSleep(second);
            storage::Frame frame = device.getData();
            INFO("Vm should be shortcuted to Uref, Utpl to the ground");
            INFO(frame.repr());
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Umod], 0.));
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 1000.));
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Utpl], 0.));
        }
    }
    SECTION("Sampling rate setup") {
        int16_t f1 = 0;
        int16_t f2 = 0;
        int16_t not_less = 1000 / 50;
        int16_t tolerance = not_less / 10;
        not_less -= tolerance;
        {
            DaqboardDevice device;
            device.setReadingSampling(common::TimeUnit(50));
            device.prepare();
            device.run();
            testSleep(second);
            auto frame = device.getData();
            f1 = frame.size();
            INFO(frame.repr());
            REQUIRE(f1 > not_less);
            device.stop();
        }
        {
            DaqboardDevice device;
            device.setReadingSampling(common::TimeUnit(25));
            device.prepare();
            device.run();
            testSleep(second);
            auto frame = device.getData();
            f2 = frame.size();
            INFO(frame.repr());
            REQUIRE(f2 > not_less);
            device.stop();
        }
        REQUIRE(isApproxEqual(2 * f1, f2, tolerance));
    }
    SECTION("Fixed duration one step") {
        DaqboardDevice device;
        auto sampling_step = common::TimeUnit(10);
        size_t ratio = 50; // less than a second
        auto duration = ratio * sampling_step;
        device.setReadingSampling(sampling_step);
        device.setProfiles({
            {common::ControlKey::Vm, {1000, 1000, 1000, 1000}},
            }, sampling_step * 4);
        device.setDuration(duration);
        device.prepare();
        device.run();
        testSleep(second);
        // State is Running till any data is available - ongoing or cached
        REQUIRE(device.getState() == +DeviceState::Running);
        auto frame = device.getData();
        // Now there is no data and device should stop itself
        REQUIRE(device.getState() == +DeviceState::CanSet);
        INFO(frame.size());
        REQUIRE(isApproxEqual(frame.size(), ratio, 2));
        INFO(frame.repr());
        REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 1000.));

    }
    SECTION("Fixed duration two steps") {
        DaqboardDevice device;
        auto sampling_step = common::TimeUnit(10);
        size_t ratio = 200; // less than a second
        auto duration = ratio * sampling_step;
        device.setReadingSampling(sampling_step);
        device.setProfiles({
            {common::ControlKey::Vm, {1000, 1000, 1000, 1000}},
            }, sampling_step * 4);
        device.setDuration(duration);
        device.prepare();
        device.run();
        size_t total_size = 0;
        size_t i = 0;
        for (i = 0; i < 5; i++) {
            if (device.getState() != +DeviceState::Running) {
                break;
            }
            testSleep(second);
            auto frame = device.getData();
            REQUIRE(isEqualConstValue(frame[common::SignalKey::Uref], 1000.));
            total_size += frame.size();
        }
        REQUIRE(i < 4);
        REQUIRE(total_size == ratio);
    }
    SECTION("Timer replacement") {
        size_t step_ms = 10;
        DaqboardDevice device;
        auto timer = DeviceTimer(step_ms);
        device.setTimer(
            timer
        );
        device.prepare();
        device.run();
        testSleep(second);
        auto one = device.getData();
        device.stop();

        common::ConfigPtr cfg = common::acquireConfig();
        double mult = 0.1;
        cfg->write(common::ConfigDoubleKey::TimeUnitSize, mult);
        timer.reconfigure(cfg);

        device.setTimer(
            timer
        );
        device.prepare();
        device.run();
        testSleep(second);
        auto two = device.getData();
        INFO(one.getTs().value());
        INFO(two.getTs().value());
        REQUIRE(isApproxEqual(one.getTs().value(), two.getTs().value() * mult, 10));
    }
}
