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
        if (!state_machine.transition(entity, PlayerState::WALKING))
            return;

        Motion &motion = registry.motions.get(entity);
        motion.velocity.x = -MOVE_SPEED;
        motion.direction = false; // facing left
    }
};

class MoveRightCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::WALKING))
            return;

        Motion &motion = registry.motions.get(entity);
        motion.velocity.x = MOVE_SPEED;
        motion.direction = true; // facing right
    }
};

class JumpCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::JUMPING))
            return;

        Motion &motion = registry.motions.get(entity);
        if (motion.inAir)
            return;

        Player &player = registry.players.get(entity);
        std::cout << "Player " << player.id << " jumped!" << std::endl;
        motion.inAir = true;
        motion.velocity.y = FighterManager::getFighterConfig(player.current_char).JUMP_VELOCITY;
    }
};

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

        PlayerCurrentState &playerState = registry.playerCurrentStates.get(entity);
        HitBox &hitBox = registry.hitBoxes.get(entity);
        StateTimer &stateTimer = registry.stateTimers.get(entity);
        Fighters fighter = registry.players.get(entity).current_char;
        FighterConfig fighterConfig = FighterManager::getFighterConfig(fighter);
        Motion &motion = registry.motions.get(entity);
        Player &player = registry.players.get(entity);

        if (canPunch(playerState.currentState))
        {
            hitBox.active = true;

            // Set base offset and adjust based on player direction
            float baseOffset = fighterConfig.PUNCH_X_OFFSET;
            hitBox.xOffset = motion.direction ? baseOffset : -baseOffset;

            hitBox.yOffset = fighterConfig.PUNCH_Y_OFFSET;
            hitBox.width = fighterConfig.PUNCH_WIDTH;
            hitBox.height = fighterConfig.PUNCH_HEIGHT;
            hitBox.hit = false;

            playerState.currentState = PlayerState::ATTACKING;
            stateTimer.reset(fighterConfig.HITBOX_DURATION);
        }
    }
};

// TODO: implement KickCommand to have a different hitbox
// currently, same as PunchCommand
class KickCommand : public Command
{
public:
    void execute(Entity entity, StateMachine &state_machine) override
    {
        if (!state_machine.transition(entity, PlayerState::ATTACKING))
            return;
        PlayerCurrentState &playerState = registry.playerCurrentStates.get(entity);
        HitBox &hitBox = registry.hitBoxes.get(entity);
        StateTimer &stateTimer = registry.stateTimers.get(entity);
        Fighters fighter = registry.players.get(entity).current_char;

        if (canPunch(playerState.currentState))
        {
            hitBox.active = true;
            playerState.currentState = PlayerState::ATTACKING;
            stateTimer.reset(FighterManager::getFighterConfig(fighter).HITBOX_DURATION);
        }
    }
};