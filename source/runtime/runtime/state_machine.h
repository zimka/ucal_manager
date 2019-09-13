//
// Created by igor on 06.09.19.
//

#ifndef UCAL_MANAGER_STATE_MACHINE_H
#define UCAL_MANAGER_STATE_MACHINE_H

#include <memory>
#include <better_enums/enum.h>
#include <common/config.h>

namespace runtime {
    struct DummyPlan {
        bool fool;
    };
    struct DummyContext {
        DummyPlan plan;
    };

    BETTER_ENUM(MachineState, unsigned, NotReady, Error, HasPlan, Executing)

    class IState {
    public:
        virtual void update() {}; // Does nothing, this is by design

        virtual MachineState getState() = 0;

        virtual common::Config const& getConfig() = 0; ///< returns read-only config reference

        virtual DummyPlan getPlan() = 0;

        //virtual Data getData() = 0;

        virtual void setConfig(common::Config) = 0;

        virtual void setPlan(DummyPlan) = 0;

        virtual void runNext() = 0;

        virtual void stop() = 0;

    };

    using StatePtr = std::unique_ptr<IState>;

    class StateMachine : public IState {
    private:
        StatePtr state_;
        DummyContext dummyContext_;

    public:
        void update() override;

        common::Config const& getConfig() override;

        void runNext() override;

        void stop() override;

        void setPlan(DummyPlan) override;

        DummyPlan getPlan() override;
        //setConfig(json) override;
        MachineState getState() override;
        //Data getData() override;

        DummyContext& getContext();

        void setState(StatePtr new_state);
    };
}
#endif //UCAL_MANAGER_STATE_MACHINE_H
