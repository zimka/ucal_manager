//
// Created by igor on 06.09.19.
//

#include "state_machine.h"
#include <common/exceptions.h>

#include <sstream>

#define STATE_DECL(STATE_NAME) \
public: \
    STATE_NAME(StateMachine* machine) : machine_(machine) {} \
    MachineState getState() override; \
    common::Config const& getConfig() override;\
    DummyPlan getPlan() override; \
    /*Data getData() override;*/ \
    void setConfig(common::Config) override; \
    void setPlan(DummyPlan) override; \
    void runNext() override; \
    void stop() override; \
    \
private: \
    StateMachine* machine_;

using namespace runtime;

class NoPlanState : public IState
{
STATE_DECL(NoPlanState)
};

class HasPlanState : public IState
{
STATE_DECL(HasPlanState)
};

class NotReadyState : public IState
{
STATE_DECL(NotReadyState)
};
#undef STATE_INTERFACE_DECL

void NoPlanState::runNext() {
    throw common::StateViolationError("Cannot run anything while in NoPlanState!");
}

void NoPlanState::stop() {
    throw common::StateViolationError("Cannot stop anything while in NoPlanState!");
}

void NoPlanState::setPlan(DummyPlan plan) {
    auto context = machine_->getContext();
    // TODO: write logic for context and plan
    machine_->setState(std::make_unique<HasPlanState>(machine_));
}

DummyPlan NoPlanState::getPlan() {
    return machine_->getContext().plan;
}

template <unsigned S>
class GenericState : public IState {
public:
    GenericState(StateMachine* machine) : machine_(machine) {}
    MachineState getState() override;

    common::Config const& getConfig() override;

    DummyPlan getPlan() override;

    /*Data getData() override;*/

    void setConfig(common::Config) override;

    void setPlan(DummyPlan) override;

    void runNext() override;

    void stop() override;

private:
    StateMachine* machine_;

    void throwError(const char* name);
};

template <unsigned S>
void GenericState<S>::throwError(const char* name) {
    std::stringstream message;
    message << "Cannot execute " << name << " from "
            << MachineState::_from_integral(S)._to_string();
    throw common::StateViolationError(message.str());
}

/*!
 * Defines member function that throws exception
 */
#define STATE_DEFAULT_THROW(retv, fname, ...) \
template <unsigned S> \
retv GenericState<S>::fname(__VA_ARGS__) { throwError(#fname); }

/*!
 * Returns state, same for all functions
 * @tparam S numeric representation of MachineState
 * @return MachineState value of current state
 */
template <unsigned S>
MachineState GenericState<S>::getState() {
    return MachineState::_from_integral(S);
}

template <unsigned S>
common::Config const& GenericState<S>::getConfig() {
    return *common::acquireConfig();
}

STATE_DEFAULT_THROW(DummyPlan, getPlan)

STATE_DEFAULT_THROW(void, setConfig, common::Config)

STATE_DEFAULT_THROW(void, setPlan, DummyPlan)

STATE_DEFAULT_THROW(void, runNext)

STATE_DEFAULT_THROW(void, stop)

template <>
common::Config const& GenericState<MachineState::NotReady>::getConfig() {
    throwError ("getConfig");
}
