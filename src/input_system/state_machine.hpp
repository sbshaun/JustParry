#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include "../ecs/components.hpp"
#include "../ecs/ecs.hpp"

class State; // forward declaration 

/*
* A state machine class used to manage all the states. 
* The state machine stores a mapping: PlayerState -> State, this helps decouple the state transition logic from the game logic 
* e.g. without this mapping, if we want to update the state: if (currentState == PlayerState::IDLE) {// handle state game logic} else if (currentState == PlayerState::WALKING) {// handle state game logic for walking} 
* with this mapping, simply do: states[currentState]->update(elapsed_ms); 
*/
class StateMachine {
public:
    void addState(PlayerState state_enum, std::unique_ptr<State> state) {
        // bind a concrete state object to a palyer state (which is an enum) 
        states[state_enum] = std::move(state);
    }; 
    bool transition(Entity entity, PlayerState newState); // change to the current state to the passed new state. 
    void update(Entity entity, float elapsed_ms); // update the current state and check if it should transition to another state
    PlayerState getCurrentState() const { return currentState; } // getter function 

private:
    std::unordered_map<PlayerState, std::unique_ptr<State>> states; // mapping: PlayerState -> State object 
    PlayerState currentState = PlayerState::IDLE;
};

/*
* Base State class for the concrete states to derive from. 
*/
class State {
public:
    virtual ~State() = default;
    virtual void enter(Entity entity, StateMachine& stateMachine) = 0; // defines actions when entering the state 
    virtual void exit(Entity entity, StateMachine& stateMachine) = 0;  // defines actions when exiting the state 
    virtual void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) = 0; // defines actions to be taken during the state 
    virtual bool canTransitionTo(Entity entity, PlayerState newState) = 0; // check if the new state is valid to transition to for the current state 
};

class IdleState : public State {
public:
    void enter(Entity entity, StateMachine& stateMachine) override;
    void exit(Entity entity, StateMachine& stateMachine) override;
    void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) override;
    bool canTransitionTo(Entity entity, PlayerState newState) override;
};

class WalkingState : public State {
public:
    void enter(Entity entity, StateMachine& stateMachine) override;
    void exit(Entity entity, StateMachine& stateMachine) override;
    void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) override;
    bool canTransitionTo(Entity entity, PlayerState newState) override;
};

class JumpingState : public State {
public:
    void enter(Entity entity, StateMachine& stateMachine) override;
    void exit(Entity entity, StateMachine& stateMachine) override;
    void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) override;
    bool canTransitionTo(Entity entity, PlayerState newState) override;
};

class AttackingState : public State {
public:
    void enter(Entity entity, StateMachine& stateMachine) override;
    void exit(Entity entity, StateMachine& stateMachine) override;
    void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) override;
    bool canTransitionTo(Entity entity, PlayerState newState) override;
};

class CrouchingState : public State {
public:
    void enter(Entity entity, StateMachine& stateMachine) override;
    void exit(Entity entity, StateMachine& stateMachine) override;
    void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) override;
    bool canTransitionTo(Entity entity, PlayerState newState) override;
};

class ParryingState : public State {
public:
    void enter(Entity entity, StateMachine& stateMachine) override;
    void exit(Entity entity, StateMachine& stateMachine) override;
    void update(Entity entity, float elapsed_ms, StateMachine& stateMachine) override;
    bool canTransitionTo(Entity entity, PlayerState newState) override;
};
