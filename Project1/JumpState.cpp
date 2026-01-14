#include "JumpState.h"
#include "Player.h"
#include "RunState.h"
#include "IdleState.h"

void JumpState::handleInput(Player& player, const bool* keys)
{
	AirborneState::handleInput(player, keys);
    if (player.isOnGround()) {
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D]) {
            player.changeState(make_unique<RunState>());
        }
        else {
            player.changeState(make_unique<IdleState>());
        }
        return;
    }
}

void JumpState::update(Player& player, float dt)
{
    AirborneState::update(player, dt);

    const bool* keys = SDL_GetKeyboardState(nullptr);
    bool curJump = keys[SDL_SCANCODE_SPACE];
    if (!curJump && player.getIsHovering()) {
        player.setIsHovering(false);
    }


}