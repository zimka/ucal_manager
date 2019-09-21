#ifndef UCAL2_RUNTIME_BLOCK_H
#define UCAL2_RUNTIME_BLOCK_H

#include <common/measures.h>
#include <vector>
#include <string>

namespace runtime {

    struct Block {
        // TODO: add conversion from json
        Block(Block const& other) = default;
        Block& operator=(Block const& other) = default;
        Block(Block&& other) = default;
        Block& operator=(Block && other) = default;

        bool operator == (Block const& rhs) const {
            return  this->pattern_len_tu == rhs.pattern_len_tu
                 && this->block_len_tu == rhs.block_len_tu
                 && this->sampling_step_tu == rhs.sampling_step_tu
                 && this->guard == rhs.guard
                 && this->mod == rhs.mod;
        }

        bool isFinite() const { return block_len_tu > 0; }
        bool isReadOnly() const { return pattern_len_tu == 0; }

        std::string repr() const;

        uint32_t pattern_len_tu;
        uint32_t block_len_tu;
        double sampling_step_tu; ///< TimeUnit float
        std::vector<common::VoltUnit> guard;
        std::vector<common::VoltUnit> mod;
    };
    using Plan = std::vector<Block>;
}

#endif //UCAL2_RUNTIME_BLOCK_H

