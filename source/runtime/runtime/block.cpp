//
// Created by igor on 27.07.19.
//
#include "block.h"

#include <json/single_include/nlohmann/json.hpp>
#include <sstream>

using namespace runtime;

std::string Block::repr() const {
    std::stringstream stream;
    stream << "struct Block {"
           << ", write_step_ = " << write_step_tu
           << ", block_len_ = " << block_len_tu
           << ", read_step_ = " << read_step_tu
           << ", voltage_0_ = { ";
    for (auto& item : voltage_0) {
        stream << item << ", ";
    }
    stream << "}, voltage_1_ = { ";
    for (auto& item : voltage_1) {
        stream << item << ", ";
    }
    stream << "}";
    return stream.str();
}

// Have to write namespace explicitly, got linking error otherwise
void runtime::to_json(nlohmann::json& j, runtime::Block const& b) {
    j = nlohmann::json{
        {"write_step_tu", b.write_step_tu},
        {"block_len_tu", b.block_len_tu},
        {"read_step_tu", b.read_step_tu},
        {"voltage_0", b.voltage_0},
        {"voltage_1", b.voltage_1},
    };
}

// Have to write namespace explicitly, got linking error otherwise
void runtime::from_json(nlohmann::json const& j, Block& b) {
    j.at("write_step_tu").get_to(b.write_step_tu);
    j.at("block_len_tu").get_to(b.block_len_tu);
    j.at("read_step_tu").get_to(b.read_step_tu);
    j.at("voltage_0").get_to(b.voltage_0);
    j.at("voltage_1").get_to(b.voltage_1);
}
