#include "JumpState.h"
#include "Player.h"

void JumpState::handleInput(Player& player, const bool* keys) {
    // 親クラス(AirborneState)の空中移動や着地判定をそのまま使う
    AirborneState::handleInput(player, keys);
}

void JumpState::update(Player& player, float dt) {
    // 親クラス(AirborneState)の重力計算をそのまま使う
    AirborneState::update(player, dt);

    // スペースを離したらホバリング状態を解除する
    const bool* keys = SDL_GetKeyboardState(nullptr);
    if (!keys[SDL_SCANCODE_SPACE] && player.getIsHovering()) {
        player.setIsHovering(false);
    }
}