#pragma once
#include "../ecs/ecs_registry.hpp"
#include "actions.hpp"
#include "../constants.hpp"
#include "input_utils.hpp"
#include "state_machine.hpp"
#include "../fighters/fighters.hpp"

/*
 * Command interface.
 * The execute function is used for checking if current state allows the command to be executed,
 * and if yes, do the following 2 things:
 * 1. transition to new state.
 * 2. set immediate effects, e.g. velocity
 */
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute(Entity entity, StateMachine &state_machine) = 0;
};

// TODO: implement state machine to handle player states
class MoveLeftCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        state_machine.transition(entity, PlayerState::WALKING);

        Motion &motion = registry.motions.get(entity);
        motion.velocity.x = -MOVE_SPEED;
    }
};

class MoveRightCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        state_machine.transition(entity, PlayerState::WALKING);

        Motion &motion = registry.motions.get(entity);
        motion.velocity.x = MOVE_SPEED;
    }
};

//class JumpCommand : public Command
//{
//public:
//    void execute(Entity entity, StateMachine &state_machine) override
//    {
//        if (!state_machine.transition(entity, PlayerState::JUMPING))
//            return;
//
//        Motion &motion = registry.motions.get(entity);
//        if (motion.inAir)
//            return;
//
//        Player &player = registry.players.get(entity);
//        std::cout << "Player " << player.id << " jumped!" << std::endl;
//        motion.inAir = true;
//        motion.velocity.y = JUMP_VELOCITY;
//    }
//};

class CrouchCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::CROUCHING))
            return;
        // if stateMachine.transition("CROUCHING") success:
        // TODO: implement
    }
};

inline bool canPunch(PlayerState state)
{
    return state == PlayerState::IDLE || state == PlayerState::WALKING;
};

class PunchCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::ATTACKING))
            return;
    }
};

// TODO: implement KickCommand to have a different hitbox
// currently, same as PunchCommand
class KickCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::KICKING))
            return;
    }
};

class ParryCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::PARRYING)) return;
    }
};