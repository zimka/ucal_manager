//
// Created by igor on 25.06.19.
//

#include "config.h"
#include <single_header/json.hpp>
#include <iostream>
#include <sstream>
#include "exceptions.h"

using namespace common;
using nlohmann::json;

static const char DEFAULT[] =
        R"({"BoardId": "DaqBoard2000", "SamplingFreq": 10.11})";

namespace common {
    struct ConfigData {
        json def;
        json over;
    };
}

static void write_file(const char* filename, const json& data) {
    std::ofstream file(filename);
    file << data; // dump overriden config
}

Config::Config()
    : data(std::make_unique<ConfigData>())
{
    std::ifstream file(FILENAME);
    std::stringstream content_s;
    //file.seekg(0);
    content_s << file.rdbuf();
    if (!content_s.str().empty()) {
        data->over = json::parse(content_s.str());
    }
    data->def = json::parse(DEFAULT);
    //std::cout << "File contents: " << content_s.str();
}

Config::~Config() {
    write_file(FILENAME, data->over);
}

bool Config::write(common::ConfigStringKey key, const std::string &value) {
    data->over[key._to_string()] = value; // TODO: type check?
    write_file(FILENAME, data->over);
    return true;
}

bool Config::write(common::ConfigDoubleKey key, double value) {
    data->over[key._to_string()] = value; // TODO: type check?
    write_file(FILENAME, data->over);
    return true;
}

double Config::readDouble(common::ConfigDoubleKey key) const {
    if (key._value == common::ConfigDoubleKey::Undefined) {
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
        throw AssertionError (e.what()); // TODO: посмотри, что там за инфа
    } catch (json::out_of_range& e) {
        throw AssertionError (e.what());
    }

    return result;
}

std::string Config::readStr(common::ConfigStringKey key) const {
    if (key._value == common::ConfigDoubleKey::Undefined) {
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
        throw AssertionError (e.what()); // TODO: посмотри, что там за инфа
    } catch (json::out_of_range& e) {
        throw AssertionError (e.what());
    }
    return result;
}

bool Config::reset() {
    data->over.clear();
    return true;
}

/*std::string Config::getDefault() const {
    return std::string(DEFAULT);
}

std::string Config::getOverride() const {
    std::stringstream content_s;
    //override.seekg(0);
    //content_s << override.rdbuf();
    std::cout << "File contents: " << content_s.str();
    return content_s.str();
}*/

