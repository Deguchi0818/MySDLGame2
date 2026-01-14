#include "IdleState.h"
#include "Player.h"
#include "RunState.h"
#include "JumpState.h"

void IdleState::handleInput(Player& player, const bool* keys) 
{
    if (keys[SDL_SCANCODE_SPACE]) {
        player.velY = player.getParams().jumpPower;
        player.changeState(std::make_unique<JumpState>());
    }

    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D]) {
        player.changeState(std::make_unique<RunState>());
    }
}

void IdleState::update(Player& player, float dt) {
    player.velX = 0.0f;
}