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
           << ", pattern_len_ = " << pattern_len_tu
           << ", block_len_ = " << block_len_tu
           << ", sampling_rate_ = " << sampling_step_tu
           << ", guard_ = { ";
    for (auto& item : guard) {
        stream << item << ", ";
    }
    stream << "}, mod_ = { ";
    for (auto& item : mod) {
        stream << item << ", ";
    }
    stream << "}";
    return stream.str();
}

// Have to write namespace explicitly, got linking error otherwise
void runtime::to_json(nlohmann::json& j, runtime::Block const& b) {
    j = nlohmann::json{
        {"pattern_len_tu", b.pattern_len_tu},
        {"block_len_tu", b.block_len_tu},
        {"sampling_step_tu", b.sampling_step_tu},
        {"guard", b.guard},
        {"mod", b.mod},
    };
}

// Have to write namespace explicitly, got linking error otherwise
void runtime::from_json(nlohmann::json const& j, Block& b) {
    j.at("pattern_len_tu").get_to(b.pattern_len_tu);
    j.at("block_len_tu").get_to(b.block_len_tu);
    j.at("sampling_step_tu").get_to(b.sampling_step_tu);
    j.at("guard").get_to(b.guard);
    j.at("mod").get_to(b.mod);
}
