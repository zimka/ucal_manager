#ifndef UCAL2_RUNTIME_BLOCK_H
#define UCAL2_RUNTIME_BLOCK_H

#include <common/measures.h>
#include <vector>
#include <string>
#include <json/include/nlohmann/json_fwd.hpp>

namespace runtime {

    struct Block {
        // TODO: add conversion from json
        Block() = default;
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
        bool isReadOnly() const {
            // fails if mod or guard size() > 0
            // TODO: validation in core
            return pattern_len_tu == 0;
        }

        std::string repr() const;

        uint32_t pattern_len_tu;
        uint32_t block_len_tu;
        double sampling_step_tu; ///< TimeUnit float
        std::vector<common::MilliVolt> guard;
        std::vector<common::MilliVolt> mod;
    };
    using Plan = std::vector<Block>;

    void to_json(nlohmann::json& j, Block const& b);
    void from_json(nlohmann::json const& j, Block& b);

}

#endif //UCAL2_RUNTIME_BLOCK_H

