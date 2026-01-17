#pragma once
#include "EnemyWalking.h"
#include "EnemyBullet.h"
#include <vector>
#include <memory>

class EnemyShooter : public EnemyWalking
{
public:
	using EnemyWalking::EnemyWalking;

    void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds) override;
    void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) override;
    void shoot(const SDL_FRect& playerRect);

    void setBulletTexture(SDL_Texture* tex) { m_bulletTexture = tex; }

private:
    float m_shootTimer = 0.0f;
    float m_shootInterval = 2.0f; // 2•b‚É1‰ñ
    std::vector<std::unique_ptr<Bullet>> m_bullets;

    SDL_Texture* m_bulletTexture = nullptr;
};

