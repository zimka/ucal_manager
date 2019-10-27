//
// Created by igor on 06.09.19.
//

#ifndef UCAL_MANAGER_STATE_MACHINE_H
#define UCAL_MANAGER_STATE_MACHINE_H

#include "i_state.h"
#include <device/device.h>
#include <thread>

namespace runtime {
    using nlohmann::json;
    using DevicePtr = std::unique_ptr<device::IDevice>;

    class CoreState;
    using CorePtr = std::unique_ptr<CoreState>;
    class StateMachine : public IState {
    private:
        StatePtr state_;
        CorePtr core_;

    public:
        StateMachine ();
        ~StateMachine() override = default;
        
        StateMachine(StateMachine const& other) = delete;
        StateMachine& operator=(StateMachine const& other) = delete;

        StateMachine(StateMachine&& other) = default;
        StateMachine& operator=(StateMachine&& other) = default;

        common::MachineState getState() override;

        json getConfig() override;

        Plan getPlan() override;

        storage::Storage const& getData() override;

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

        void setState(StatePtr new_state);

        CorePtr& accessCore();
    };

    template <common::MachineStateType S>
    class GenericState : public IState {
    public:
        GenericState() = default;
        explicit GenericState(StateMachine* machine) : machine_(machine) {}
        ~GenericState() override = default;
        common::MachineState getState() override;

        json getConfig() override;

        Plan getPlan() override;

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
