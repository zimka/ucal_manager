#include "config.h"
#include "json/single_include/nlohmann/json.hpp"
#include <iostream>
#include <sstream>
#include <atomic>
#include "exceptions.h"

// TODO: make some universal checks (arguments, flags) to avoid code duplicaton

using namespace common;
using nlohmann::json;

static const char DEFAULT[] =
        R"({"BoardId": "DaqBoard3001USB", "TimeUnitSize":1, "StorageFrameSize": 10})";

static std::atomic_bool IMMUTABLE(false);

namespace common {
    struct ConfigData {
        json def;
        json over;
    };

    ConfigPtr acquireConfig() {
        static ConfigPtr instance = std::make_shared<Config>();
        return instance;
    }

    ConfigPtr acquireConfig(std::string const& filename) {
        static ConfigPtr instance = std::make_shared<Config>(filename);
        return instance;
    }
}

static void write_file(std::string const& filename, const json& data) {
    std::ofstream file(filename);
    file << data; // dump overriden config
}

Config::Config()
    : data(std::make_unique<ConfigData>())
    , override_filename(FILENAME)
{
    std::ifstream file(FILENAME);
    if (file.is_open()) {
        data->over = json::parse(file);
    }
    data->def = json::parse(DEFAULT);
}

Config::Config(std::string const& filename)
    : data(std::make_unique<ConfigData>())
    , override_filename(filename)
{
    std::ifstream file(filename);
    if (file.is_open()) {
        data->over = json::parse(file);
    }
    data->def = json::parse(DEFAULT);
}

Config::~Config() {
    write_file(override_filename, data->over);
}

bool Config::write(common::ConfigStringKey key, std::string const& value) {
    if (!IMMUTABLE.exchange(true)) {
        data->over[key._to_string()] = value; // TODO: type check?
        write_file(override_filename, data->over);
    }
    else {
        throw AssertionError("Config simultaneous writing!");
    }
    IMMUTABLE.store(false);
    return true;
}

bool Config::write(common::ConfigDoubleKey key, double value) {
    if (!IMMUTABLE.exchange(true)) {
        data->over[key._to_string()] = value; // TODO: type check?
        write_file(override_filename, data->over);
    }
    else {
        throw AssertionError("Config simultaneous writing!");
    }
    IMMUTABLE.store(false);
    return true;
}

double Config::readDouble(common::ConfigDoubleKey key) const {
    if (IMMUTABLE.load()) {
        throw AssertionError("Config reading while writing!");
    }
    else if (key._value == common::ConfigDoubleKey::Undefined) {
        throw AssertionError("readDouble called with Undefined");
    }
    double result = 0.0;
    try {
        auto elem_iter = data->over.find(key._to_string());
        if (elem_iter != data->over.end()) {
            result = elem_iter->get<double>();
        }
        else {
            result = data->def.at(key._to_string()).get<double>();
        }
    } catch (json::type_error& e) {
        throw AssertionError(e.what());
    } catch (json::out_of_range& e) {
        throw AssertionError(e.what());
    }

    return result;
}

std::string Config::readStr(common::ConfigStringKey key) const {
    if (IMMUTABLE.load()) {
        throw AssertionError("Config reading while writing!");
    }
    else if (key._value == common::ConfigDoubleKey::Undefined) {
        throw AssertionError("readStr called with Undefined");
    }
    std::string result;
    try {
        auto elem_iter = data->over.find(key._to_string());
        if (elem_iter != data->over.end()) {
            result = elem_iter->get<std::string>();
        }
        else {
            result = data->def.at(key._to_string()).get<std::string>();
        }
    } catch (json::type_error& e) {
        throw AssertionError(e.what());
    } catch (json::out_of_range& e) {
        throw AssertionError(e.what());
    }
    return result;
}

bool Config::reset() {
    data->over.clear();
    return true;
}