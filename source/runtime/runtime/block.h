#ifndef UCAL2_RUNTIME_BLOCK_H
#define UCAL2_RUNTIME_BLOCK_H

#include <common/measures.h>
#include <vector>
#include <string>
#include <json/include/nlohmann/json_fwd.hpp>

namespace runtime {

    struct Block {
        Block() = default;
        Block(Block const& other) = default;
        Block& operator=(Block const& other) = default;
        Block(Block&& other) = default;
        Block& operator=(Block && other) = default;

        bool operator == (Block const& rhs) const {
            return  this->write_step_tu == rhs.write_step_tu
                 && this->block_len_tu == rhs.block_len_tu
                 && this->read_step_tu == rhs.read_step_tu
                 && this->voltage_0 == rhs.voltage_0
                 && this->voltage_1 == rhs.voltage_1;
        }

        bool isFinite() const { return block_len_tu > 0; }
        bool isReadOnly() const {
            // fails if mod or guard size() > 0
            // TODO: validation in core
            return write_step_tu == 0;
        }

        std::string repr() const;

        uint32_t write_step_tu;
        uint32_t block_len_tu;
        double read_step_tu; ///< TimeUnit float
        std::vector<common::MilliVolt> voltage_0;
        std::vector<common::MilliVolt> voltage_1;
    };
    using Plan = std::vector<Block>;

    void to_json(nlohmann::json& j, Block const& b);
    void from_json(nlohmann::json const& j, Block& b);

}

#endif //UCAL2_RUNTIME_BLOCK_H

