#include "GroundedState.h"
#include "Player.h"
#include "AirborneState.h"
#include "JumpState.h"

void GroundedState::handleInput(Player& player, const bool* keys)
{
    if (player.isJumpTriggered(keys) || player.getJumpBufferTimer() > 0) {
        player.velY = player.getParams().jumpPower;
        player.setOnGround(false);
        player.setJumpBufferTimer(0);
        player.changeState(std::make_unique<JumpState>());
        return;
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