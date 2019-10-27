#include <catch2/catch.hpp>
#include <sstream>
#include <thread>
#include <functional>
#include "common/timestamp.h"
#include "common/keys.h"
#include "common/config.h"
#include "common/exceptions.h"

using namespace common;


TEST_CASE("TimeStamp") {
    SECTION("Creation") {
        TimeStamp ts(0, 0);
        REQUIRE(ts.step == 0);
        REQUIRE(ts.count == 0);
    }
    SECTION("Comparation") {
        TimeStamp ts1(10, 10);
        TimeStamp ts2(10, 10);

        TimeStamp ts3(10, 20);
        TimeStamp ts4(20, 10);

        TimeStamp ts5(0, 10);
        TimeStamp ts6(0, 10);

        TimeStamp ts7(10, 0);
        TimeStamp ts8(10, 0);
        TimeStamp ts9(20, 0);


        REQUIRE(ts1 == ts2);
        REQUIRE(ts2 == ts1);

        REQUIRE(ts3 > ts1);
        REQUIRE(ts4 > ts1);

        REQUIRE(ts2 < ts3);
        REQUIRE(ts2 < ts4);

        REQUIRE(ts3 == ts4);

        REQUIRE(ts5 < ts1);
        REQUIRE(ts6 < ts3);
        REQUIRE(ts9 < ts4);


        REQUIRE(ts5 == ts6);
        REQUIRE(ts5 == ts7);
        REQUIRE(ts7 == ts8);
        REQUIRE(ts7 == ts9);
    }
    SECTION("Representation") {
        TimeStamp ts(5, 20);
        std::string desired_repr = "TimeStamp<step:5,count:20>";
        std::stringstream ss;
        ss << ts;
        REQUIRE(ts.value() == TimeUnit(100));
        REQUIRE(ts.repr() == desired_repr);
        REQUIRE(ss.str() == desired_repr);
    }
    SECTION("Copy") {
        TimeStamp ts1(10, 10);
        TimeStamp ts2(ts1);
        REQUIRE(ts1.step == ts2.step);
        REQUIRE(ts1.count == ts2.count);
    }
    SECTION("Assignment") {
        TimeStamp ts1(10, 10);
        TimeStamp ts2(30, 30);
        ts1 = ts2;
        REQUIRE(ts1.step == ts2.step);
        REQUIRE(ts1.count == ts2.count);
    }
}

TEST_CASE("Keys") {
    SECTION("SignalKey") {
        std::string source = "Uhtr";
        // NOTE: can't be cast from std::string directly
        SignalKey k1 = SignalKey::_from_string(source.c_str());
        SignalKey k2 = SignalKey::Uhtr;
        SignalKey k3 = SignalKey::Umod;
        REQUIRE(k1 == k2);
        //NOTE: must be compared with +Key::Value, not Key::Value
        REQUIRE(k1 == +SignalKey::Uhtr);
        REQUIRE(k1 != k3);
        REQUIRE(k1._to_string() == source);
        REQUIRE(k1._to_string() == k2._to_string());
    }
}

TEST_CASE("Config") {
    SECTION("Doubles") {
        ConfigPtr mock = acquireConfig();
        double value = 10.11;
        REQUIRE_NOTHROW(mock->readDouble(ConfigDoubleKey::StorageFrameSize));
        CHECK(mock->write(ConfigDoubleKey::StorageFrameSize, value));
        REQUIRE(mock->readDouble(ConfigDoubleKey::StorageFrameSize) == value);
        REQUIRE_THROWS(mock->readDouble(ConfigDoubleKey::Undefined));
    }

    SECTION("Strings") {
        ConfigPtr mock = acquireConfig();
        std::string value = "WeirdId";
        REQUIRE(mock->write(ConfigStringKey::BoardId, value));
        REQUIRE(mock->readStr(ConfigStringKey::BoardId) == value);
    }

    SECTION("Defaults") {
        ConfigPtr mock = acquireConfig();
        double value = mock->readDouble(ConfigDoubleKey::StorageFrameSize);
        double new_value = value + 1.1;
        mock->write(ConfigDoubleKey::StorageFrameSize, new_value);
        REQUIRE(mock->readDouble(ConfigDoubleKey::StorageFrameSize) == new_value);
        REQUIRE(mock->reset());
        REQUIRE(mock->readDouble(ConfigDoubleKey::StorageFrameSize) == value);
    }

    SECTION("Recreation") {
        double value = 10.1;
        {
            ConfigPtr first = acquireConfig();
            first->write(ConfigDoubleKey::StorageFrameSize, value);
        }
        {
            ConfigPtr second = acquireConfig();
            REQUIRE(second->readDouble(ConfigDoubleKey::StorageFrameSize) == value);
        }
    }
}

