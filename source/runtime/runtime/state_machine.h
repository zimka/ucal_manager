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
    struct DummyPlan {
        bool valid;
    };
    struct Context {
        Plan plan;
        DevicePtr device;
        storage::Storage storage;

        /*Context (Context&& other)
            : plan(std::move(other.plan))
            , device(std::move(other.device))
            , storage(std::move(other.storage))
        {}*/
    };



    class StateMachine : public IState {
    private:
        StatePtr state_;
        Context context_;
        //std::thread monitor_;

    public:
        StateMachine () = default;
        explicit StateMachine (Context context);
        //void update() override;

        common::MachineState getState() override;

        common::Config const& getConfig() override;

        Plan const& getPlan() override;

        storage::Storage const& getData() override;

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

        Context& getContext();

        void setState(StatePtr new_state);

        //std::thread& accessMonitor() { return monitor_; }
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

        //void throwError(const char* name);
    };

    /*template <>
    class GenericState<MachineState::Executing> : public IState {
    public:
        GenericState() = default;
        GenericState(StateMachine* machine);
        MachineState getState() override;

        common::Config const& getConfig() override;

        Plan const& getPlan() override;

        storage::Storage const& getData() override;

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

    private:
        StateMachine* machine_;

        //static void throwError(const char* name);
    };*/

    template <common::MachineStateType S>
    StatePtr createState(StateMachine* machine) {
        return std::make_unique<GenericState<S>>(machine);
    }
}
#endif //UCAL_MANAGER_STATE_MACHINE_H
