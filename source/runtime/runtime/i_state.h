//
// Created by igor on 24.09.2019.
//

#ifndef UCAL_MANAGER_I_STATE_H
#define UCAL_MANAGER_I_STATE_H

#include "block.h"
#include <common/config.h>
#include <storage/storage.h>
#include <json/include/nlohmann/json_fwd.hpp>
#include <memory>

namespace runtime {
    class IState {
    public:
        virtual void update() {}; // Does nothing, this is by design

        virtual common::MachineState getState() = 0;

        virtual common::Config const& getConfig() = 0; ///< returns read-only config reference

        virtual Plan const& getPlan() = 0;

        virtual storage::Storage const& getData() = 0;

        virtual void setConfig(json const& json) = 0;

        virtual void setPlan(Plan) = 0; //!< We can move or copy here, so accept by value

        virtual void runNext() = 0;

        virtual void stop() = 0;

    };

    using StatePtr = std::unique_ptr<IState>;
}

#endif //UCAL_MANAGER_I_STATE_H
