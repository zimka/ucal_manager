//
// Created by igor on 27.07.19.
//
#include "block.h"

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
