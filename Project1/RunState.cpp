#include "RunState.h"
#include "Player.h"
#include "IdleState.h"

void RunState::handleInput(Player& player, const bool* keys)
{
	GroundedState::handleInput(player, keys);

	if (!player.isOnGround()) return;

	if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) {
		player.changeState(make_unique<IdleState>());
	}
}
void RunState::update(Player& player, float dt) 
{
	const bool* keys = SDL_GetKeyboardState(nullptr);

	bool curA = keys[SDL_SCANCODE_A];
	bool curD = keys[SDL_SCANCODE_D];

	player.velX *= 0.0f;

	if (curA)
	{
		player.velX -= player.getParams().speed;
		player.m_facingDir = -1.0f;
	}

	if (curD)
	{
		player.velX += player.getParams().speed;
		player.m_facingDir = 1.0f;
	}
}