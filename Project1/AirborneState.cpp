#include "AirborneState.h"
#include "GroundedState.h"
#include "Player.h"
#include "JumpState.h"

void AirborneState::handleInput(Player& player, const bool* keys) {
    // 1. 空中移動 (左右)
    player.velX = 0;
    if (keys[SDL_SCANCODE_A]) {
        player.velX -= player.getParams().speed;
        player.m_facingDir = -1.0f;
    }
    if (keys[SDL_SCANCODE_D]) {
        player.velX += player.getParams().speed;
        player.m_facingDir = 1.0f;
    }

    if (player.isJumpTriggered()) {
        if (player.getCoyoteTimer() > 0) {
            player.velY = player.getParams().jumpPower;
            player.setCoyoteTimer(0);
            player.changeState(make_unique<JumpState>());
        }
        else {
            // 空中なら何度でもフラップ(再上昇)できるようにする
            player.setIsHovering(true);
            player.velY = player.getParams().hoverFlapSpeed;
            player.changeState(make_unique<JumpState>());
        }
    }
}

void AirborneState::update(Player& player, float dt) {
    const auto& params = player.getParams();

    // 3. 重力計算（ホバリング中は弱くなる）
    float gravity = params.gravity;
    if (player.getIsHovering()) {
        gravity = params.hoverGravity;
    }
    else if (player.velY > 0) {
        gravity *= params.fallMultiplier;
    }
    player.velY += gravity * dt;

    // 4. 着地判定
    if (player.isOnGround()) {
        player.setIsHovering(false); // 着地したらホバリング解除
        player.changeState(std::make_unique<IdleState>());
    }
}