//
// Created by igor on 06.09.19.
//

#ifndef UCAL_MANAGER_STATE_MACHINE_H
#define UCAL_MANAGER_STATE_MACHINE_H

#include "block.h"
#include <common/config.h>
#include <device/device.h>
#include <storage/storage.h>
#include <better_enums/enum.h>
#include <json/include/nlohmann/json_fwd.hpp>
#include <memory>

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
    };

    using MachineStateType = unsigned;
    BETTER_ENUM(MachineState, MachineStateType, NotReady, Error, NoPlan, HasPlan, Executing)

    class IState {
    public:
        virtual void update() {}; // Does nothing, this is by design

        virtual MachineState getState() = 0;

        virtual common::Config const& getConfig() = 0; ///< returns read-only config reference

        virtual Plan const& getPlan() = 0;

        //virtual Data getData() = 0;

        virtual void setConfig(json const& json) = 0;

        virtual void setPlan(Plan) = 0; //!< We can move or copy here, so accept by value

        virtual void runNext() = 0;

        virtual void stop() = 0;

    };

    using StatePtr = std::unique_ptr<IState>;

    class StateMachine : public IState {
    private:
        StatePtr state_;
        Context context_;

    public:
        //void update() override;

        MachineState getState() override;

        common::Config const& getConfig() override;

        Plan const& getPlan() override;

        //Data getData() override;

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

        Context& getContext();

        void setState(StatePtr new_state);
    };

    template <MachineStateType S>
    class GenericState : public IState {
    public:
        GenericState(StateMachine* machine) : machine_(machine) {}
        MachineState getState() override;

        common::Config const& getConfig() override;

        Plan const& getPlan() override;

        /*Data getData() override;*/

        void setConfig(json const&) override;

        void setPlan(Plan) override;

        void runNext() override;

        void stop() override;

    private:
        StateMachine* machine_;

        void throwError(const char* name);
    };

    template <MachineStateType S>
    StatePtr createState(StateMachine* machine) {
        return std::make_unique<GenericState<S>>(machine);
    }
}
#endif //UCAL_MANAGER_STATE_MACHINE_H
