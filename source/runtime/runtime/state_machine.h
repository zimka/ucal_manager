//
// Created by igor on 06.09.19.
//

#ifndef UCAL_MANAGER_STATE_MACHINE_H
#define UCAL_MANAGER_STATE_MACHINE_H

#include "i_state.h"
#include <common/config.h>
#include <device/device.h>
#include <storage/storage.h>
#include <better_enums/enum.h>
#include <json/include/nlohmann/json_fwd.hpp>
#include <memory>
#include <thread>

namespace runtime {
    using nlohmann::json;
    using DevicePtr = std::unique_ptr<device::IDevice>;

    class StateMachine : public IState {
    private:
        StatePtr state_;
        StatePtr core_;

    public:
        StateMachine ();
        
        StateMachine(StateMachine const& other) = delete;

        common::MachineState getState() override;

        common::Config const& getConfig() override;

        Plan const& getPlan() override;

        storage::Storage const& getData() override;

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

        void setState(StatePtr new_state);

        StatePtr& accessCore();
    };

    template <common::MachineStateType S>
    class GenericState : public IState {
    public:
        GenericState() = default;
        GenericState(StateMachine* machine) : machine_(machine) {}
        common::MachineState getState() override;

        common::Config const& getConfig() override;

        Plan const& getPlan() override;

        storage::Storage const& getData() override;

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

    private:
        StateMachine* machine_;
    };

    template <common::MachineStateType S>
    StatePtr createState(StateMachine* machine) {
        return std::make_unique<GenericState<S>>(machine);
    }
}
#endif //UCAL_MANAGER_STATE_MACHINE_H
