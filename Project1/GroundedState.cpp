#include "GroundedState.h"
#include "Player.h"
#include "AirborneState.h"
#include "JumpState.h"

void GroundedState::handleInput(Player& player, const bool* keys) 
{

    if (player.isJumpTriggered()) {
        player.velY = player.getParams().jumpPower; // ã•ûŒü‚Ö‰Á‘¬
        player.setOnGround(false);                  // Ú’n‚ğ‰ğœ
        player.changeState(std::make_unique<JumpState>());
        return; // ó‘Ô‚ª•Ï‚í‚Á‚½‚Ì‚Åˆ—‚ğ”²‚¯‚é
    }

    // ‘«ê‚ª‚È‚­‚È‚Á‚½‚ç—‰ºó‘Ô‚Ö
    if (!player.isOnGround()) {
        player.changeState(std::make_unique<AirborneState>());
    }
}

void GroundedState::update(Player& player, float dt) {
    player.setCoyoteTimer(player.getParams().coyoteTimeMax);
}