#pragma once
#include "Enemy.h"
class Boss : public Enemy
{
public:
    Boss(float x, float y, float w, float h, SDL_Texture* texture, SDL_Texture* dashTexture)
        : Enemy(x, y, w, h, texture), m_dashTexture(dashTexture)
    { 
        m_hp = 50;
        speed = 400.0f;
    }

    void update(float dt, const SDL_FRect& playerRect, const Player& player, const std::vector<BoxCollider>& grounds) override;
    void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) override;

    void takeDamage() override;
    void applyKnockback(float forceX, float forceY) override;

private:
    SDL_Texture* m_dashTexture = nullptr;
    BossState m_currentState = BossState::Idle;
    float m_stateTimer = 0.0f;
    float m_shakeTimer = 0.0f; // êkÇ¶ââèoóp
    float m_dashDir = 0.0f;
};

