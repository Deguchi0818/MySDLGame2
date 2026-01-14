#include "GroundedState.h"
#include "Player.h"
#include "AirborneState.h"
#include "JumpState.h"

void GroundedState::handleInput(Player& player, const bool* keys) {
    // 1. 地面でジャンプボタンが押されたらジャンプ
    if (player.m_jumpTriggered || player.getJumpBufferTimer() > 0) {
        player.velY = player.getParams().jumpPower;
        player.setOnGround(false);
        player.setJumpBufferTimer(0);
        player.changeState(std::make_unique<JumpState>());
        return; // 状態が変わったので処理終了
    }

    // 2. 足場がなくなったら落下状態へ
    if (!player.isOnGround()) {
        player.changeState(std::make_unique<AirborneState>());
    }
}

void GroundedState::update(Player& player, float dt) {
    player.setCoyoteTimer(player.getParams().coyoteTimeMax);
}