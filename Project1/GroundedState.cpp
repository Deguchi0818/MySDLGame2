#include "GroundedState.h"
#include "Player.h"
#include "AirborneState.h"

void GroundedState::handleInput(Player& player, const bool* keys)
{
    if (keys[SDL_SCANCODE_SPACE] || player.getJumpBufferTimer() > 0) {
        player.velY = player.getParams().jumpPower;
        player.setOnGround(false);
        player.setJumpBufferTimer(0);
        player.changeState(std::make_unique<AirborneState>());
    }
    player.setCoyoteTimer(player.getParams().coyoteTimeMax);

    if (!player.isOnGround()) {
        player.changeState(std::make_unique<AirborneState>());
    }
}

void GroundedState::update(Player& player, float dt)
{
    player.setCoyoteTimer(player.getParams().coyoteTimeMax);
}