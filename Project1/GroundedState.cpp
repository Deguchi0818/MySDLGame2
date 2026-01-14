#include "GroundedState.h"
#include "Player.h"
#include "AirborneState.h"
#include "JumpState.h"

void GroundedState::handleInput(Player& player, const bool* keys) 
{

    // ‘«ê‚ª‚È‚­‚È‚Á‚½‚ç—‰ºó‘Ô‚Ö
    if (!player.isOnGround()) {
        player.changeState(std::make_unique<AirborneState>());
    }
}

void GroundedState::update(Player& player, float dt) {
    player.setCoyoteTimer(player.getParams().coyoteTimeMax);
}