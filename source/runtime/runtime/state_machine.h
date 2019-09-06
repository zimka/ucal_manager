//
// Created by igor on 06.09.19.
//

#ifndef UCAL_MANAGER_STATE_MACHINE_H
#define UCAL_MANAGER_STATE_MACHINE_H

#include <memory>

struct DummyPlan { bool fool; };
struct DummyContext { bool fool; };

class IState {
public:
    virtual void update() {}; // Does nothing, this is by design
    virtual void runNext() = 0;
    virtual void stop() = 0;
    virtual void setPlan(DummyPlan) = 0;
    virtual DummyPlan getPlan() = 0;
    //virtual setConfig(json) = 0;
    //virtual json getConfig() = 0;
    //virtual IState& getState() = 0;
    //virtual Data getData() = 0;

};

using StatePtr = std::unique_ptr<IState>;

class StateMachine : public IState {
private:
    StatePtr state_;
    DummyContext dummyContext_;

public:
    void update() override;
    void runNext() override;
    void stop() override;
    void setPlan(DummyPlan) override;
    DummyPlan getPlan() override;
    //setConfig(json) override;
    //json getConfig() override;
    //IState& getState() override;
    //Data getData() override;
    
    DummyContext& getContext();
    void setState(StatePtr new_state);
};
#endif //UCAL_MANAGER_STATE_MACHINE_H
