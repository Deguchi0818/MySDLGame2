#include "AirborneState.h"
#include "GroundedState.h"
#include "Player.h"
#include "JumpState.h"

void AirborneState::handleInput(Player& player, const bool* keys) 
{
	player.velX = 0;
	if (keys[SDL_SCANCODE_A]) {
		player.velX -= player.getParams().speed;
		player.m_facingDir = -1.0f;
	}
	if (keys[SDL_SCANCODE_D]) {
		player.velX += player.getParams().speed;
		player.m_facingDir = 1.0f;
	}

	if (keys[SDL_SCANCODE_SPACE]) {
		if (player.isJumpTriggered(keys)) {
			if (player.getCoyoteTimer() > 0) {
				// コヨーテタイム中の通常ジャンプ
				player.velY = player.getParams().jumpPower;
				player.setCoyoteTimer(0);
				player.changeState(make_unique<JumpState>());
			}
			else if (!player.getIsHovering()) {
				// 空中で初めて押した時だけホバリング開始！
				player.setIsHovering(true);
				player.velY = player.getParams().hoverFlapSpeed;
				player.changeState(make_unique<JumpState>());
			}
		}
	}
}

void AirborneState::update(Player& player, float dt) 
{
	const auto& params = player.getParams();

	float gravity = params.gravity;
	if (player.getIsHovering())
	{
		gravity = params.hoverGravity;
	}
	else if (player.velY > 0)
	{
		gravity = params.gravity * params.fallMultiplier;
	}

	player.velY += gravity * dt;

	if (player.isOnGround()) {
		player.changeState(std::make_unique<IdleState>());
	}
}