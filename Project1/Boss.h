#pragma once
#include "Enemy.h"

enum class BossState {
    Idle,      // 待機（プレイヤーを狙う）
    Prepare,   // 溜め（突進前の予備動作）
    Dash,      // 突進中
    Stun,      // 衝突後の気絶
    Return,     // 画面中央へ戻る
    JumpUp,    // 上昇＆プレイヤー追従
    Hover,     // 真上で一時停止
    Fall       // 急降下
};

class Boss : public Enemy
{
public:
    Boss(float x, float y, float w, float h, SDL_Texture* texture, SDL_Texture* dashTexture)
        : Enemy(x, y, w, h, texture), m_dashTexture(dashTexture)
    { 
        m_hp = m_maxHp;
        speed = 400.0f;
    }

    void update(float dt, const SDL_FRect& playerRect, const Player& player, const std::vector<BoxCollider>& grounds) override;
    void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) override;

    void takeDamage() override;
    void applyKnockback(float forceX, float forceY) override;
    bool isBoss() const override { return true; }
private:
    SDL_Texture* m_dashTexture = nullptr;
    BossState m_currentState = BossState::Idle;
    float m_stateTimer = 0.0f;
    float m_shakeTimer = 0.0f; // 震え演出用
    float m_dashDir = 0.0f;    // 向き
    int m_maxHp = 50;
};

