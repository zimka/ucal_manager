//
// Created by igor on 06.09.19.
//

#include "state_machine.h"
#include <common/exceptions.h>

#include <sstream>
//#include <thread>

using namespace runtime;
using std::move;


StateMachine::StateMachine(Context context)
    : context_(move(context))
{
    state_ = createState<MachineState::NoPlan>(this);
}

MachineState StateMachine::getState() {
    return state_->getState();
}

common::Config const& StateMachine::getConfig() {
    return state_->getConfig();
}

Plan const& StateMachine::getPlan() {
    return state_->getPlan();
}

storage::Storage const& StateMachine::getData() {
    return state_->getData();
}

void StateMachine::setConfig(json const& json_cfg) {
    state_->setConfig(json_cfg);
}

void StateMachine::setPlan(Plan new_plan) {
    state_->setPlan(move(new_plan));
    //dummyContext_.plan = new_plan;
}

void StateMachine::runNext() {
    state_->runNext();
}

void StateMachine::stop() {
    state_->stop();
}

Context& StateMachine::getContext() {
    return context_;
}

void StateMachine::setState(StatePtr new_state) {
    state_ = move(new_state);
}

/*template <MachineStateType S>
void GenericState<S>::throwError(const char* name) {
    std::stringstream message;
    message << "Cannot execute " << name << " from "
            << MachineState::_from_integral(S)._to_string();
    throw common::StateViolationError(message.str());
}*/

// THIS CODE IS FAKE AND MUST NOT BE EXECUTED
// IT'S SOLE PURPOSE IS TO INSTATIATE ALL NEEDED TEMPLATE VARIANTS IN COMPILE TIME
template <size_t _idx>
class GenRecursive {
public:
    static int exec() {
        constexpr auto iter = MachineState::_values();
        constexpr auto start = iter.begin() + _idx;
        constexpr auto end = iter.end();
        GenericState<start->_value>();
        GenRecursive<_idx + 1>::exec();
        return 0;
    }
};

template <>
class GenRecursive<MachineState::_size()> {
public:
    static int exec() {
        return 0;
    }
};

static int foo = GenRecursive<0>::exec();
// END OF FAKE CODE

#define THROW_ERROR(fname, S) \
    std::stringstream message; \
    message << "Cannot execute " << #fname << " from " \
    << MachineState::_from_integral(S)._to_string(); \
    throw common::StateViolationError(message.str());

/*!
 * Defines member function that throws exception
 */
#define STATE_DEFAULT_THROW(retv, fname, ...) \
template <MachineStateType S> \
retv GenericState<S>::fname(__VA_ARGS__) { \
    THROW_ERROR(fname, S); \
}

/*!
 * Returns state, same for all functions
 * @tparam S numeric representation of MachineState
 * @return MachineState value of current state
 */
template <MachineStateType S>
MachineState GenericState<S>::getState() {
    return MachineState::_from_integral(S);
}

template <MachineStateType S>
common::Config const& GenericState<S>::getConfig() {
    return *common::acquireConfig();
}

STATE_DEFAULT_THROW(Plan const&, getPlan)

STATE_DEFAULT_THROW(storage::Storage const&, getData)

STATE_DEFAULT_THROW(void, setConfig, json const&)

STATE_DEFAULT_THROW(void, setPlan, Plan)

STATE_DEFAULT_THROW(void, runNext)

STATE_DEFAULT_THROW(void, stop)

template <>
common::Config const& GenericState<MachineState::NotReady>::getConfig() {
    THROW_ERROR(getConfig, MachineState::NotReady);
}

template <>
Plan const& GenericState<MachineState::NoPlan>::getPlan() {
    return machine_->getContext().plan;
}

template <>
void GenericState<MachineState::NoPlan>::setPlan(Plan new_plan) {
    if (!new_plan.empty()) {
        machine_->getContext().plan = move(new_plan);
        machine_->setState(createState<MachineState::HasPlan>(machine_));
    }
}

template <>
void GenericState<MachineState::HasPlan>::setPlan(Plan new_plan) {
    auto& stored_plan = machine_->getContext().plan;
    stored_plan = move(new_plan);
    if (stored_plan.empty()) {
        machine_->setState(createState<MachineState::NoPlan>(machine_));
    }
}

template <>
Plan const& GenericState<MachineState::HasPlan>::getPlan() {
    return machine_->getContext().plan;
}

template <>
void GenericState<MachineState::HasPlan>::runNext() {
    machine_->setState(createState<MachineState::Executing>(machine_));
}

/*GenericState<MachineState::Executing>::GenericState(runtime::StateMachine* machine)
    :machine_(machine)
{
    // Start monitoring
    auto callable = [] (Context* context) {
        using common::ControlKey;
        auto& device = context->device;
        auto& plan = context->plan;
        auto p_start = plan.begin();
        auto p_end = plan.end();
        //std::rotate(plan.begin(), plan.begin() + 1, plan.end());
        auto p_block = p_start;
        while (p_block != p_end) {
            device->setProfiles(
                    {
                            {ControlKey::Vg, p_block->guard},
                            {ControlKey::Vm, p_block->mod},
                    },
                    p_block->pattern_len_tu
            );
            device->setDuration(p_block->block_len_tu);
            device->setReadingSampling(p_block->sampling_step_tu, 1);
            device->prepare();
            device->run();
            std::this_thread::sleep_for(std::chrono::milliseconds(p_block->block_len_tu));
            context->storage.append(device->getData());
            if(device->getState() == +device::DeviceState::CanSet) {
                throw common::StateViolationError ("Device must be in CanSet after all data was extracted");
            }
            p_block++;
        }
        context->storage.finalize();
    };
    callable(&machine_->getContext());
    //machine_->accessMonitor() = std::thread(callable, &machine_->getContext());
    //machine_->setState(createState<MachineState::HasPlan>(machine_));
}*/

/*void GenericState<MachineState::Executing>::throwError(const char* name) {
    std::stringstream message;
    message << "Cannot execute " << name << " from "
            << MachineState::Executing;
    throw common::StateViolationError(message.str());
}

MachineState GenericState<MachineState::Executing>::getState() {
    return MachineState::Executing;
}

common::Config const& GenericState<MachineState::Executing>::getConfig() {
    return *common::acquireConfig();
}*/

template <>
Plan const& GenericState<MachineState::Executing>::getPlan() {
    return machine_->getContext().plan; // TODO: cut alredy done blocks from plan
}

template <>
storage::Storage const& GenericState<MachineState::Executing>::getData() {
    return machine_->getContext().storage;
}

/*void GenericState<MachineState::Executing>::setConfig(json const&) {
    THROW_ERROR(setConfig, MachineState::Executing)
}

void GenericState<MachineState::Executing>::setPlan(runtime::Plan) {
    THROW_ERROR(setPlan, MachineState::Executing)
}

void GenericState<MachineState::Executing>::runNext() {
    THROW_ERROR(runNext, MachineState::Executing)
}*/

template <>
void GenericState<MachineState::Executing>::stop() {
    auto& context = machine_->getContext();
    machine_->setState(createState<MachineState::HasPlan>(machine_));
}
