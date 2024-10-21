#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include "../ecs/components.hpp"

/*
* Base State class for the concrete states to derive from. 
*/
class State {
public:
    virtual ~State() = default;
    virtual void enter() = 0; // defines actions when entering the state 
    virtual void exit() = 0;  // defines actions when exiting the state 
    virtual void update(float dt) = 0; // defines actions to be taken during the state 
    virtual bool canTransitionTo(PlayerState newState) = 0; // check if the new state is valid to transition to for the current state 
};

class IdleState : public State {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    bool canTransitionTo(PlayerState newState) override;
};

class WalkingState : public State {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    bool canTransitionTo(PlayerState newState) override;
};

class JumpingState : public State {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    bool canTransitionTo(PlayerState newState) override;
};

class AttackingState : public State {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    bool canTransitionTo(PlayerState newState) override;
};

class ParryingState : public State {
public:
    void enter() override;
    void exit() override;
    void update(float dt) override;
    bool canTransitionTo(PlayerState newState) override;
};

/*
* A state machine class used to manage all the states. 
* The state machine stores a mapping: PlayerState -> State, this helps decouple the state transition logic from the game logic 
* e.g. without this mapping, if we want to update the state: if (currentState == PlayerState::IDLE) {// handle state game logic} else if (currentState == PlayerState::WALKING) {// handle state game logic for walking} 
* with this mapping, simply do: states[currentState]->update(dt); 
*/
class StateMachine {
public:
    void addState(PlayerState state_enum, State* state) {
        // bind a concrete state object to a palyer state (which is an enum) 
        states[state_enum] = std::move(state);
    }; 
    bool transition(PlayerState newState); // change to the current state to the passed new state. 
    void update(float dt); // update the current state and check if it should transition to another state
    PlayerState getCurrentState() const { return currentState; } // getter function 

private:
    std::unordered_map<PlayerState, State*> states; 
    PlayerState currentState = PlayerState::IDLE;
};

