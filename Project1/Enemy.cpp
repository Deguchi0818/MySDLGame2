#include "Enemy.h"
void Enemy::handleStunState(float dt) {
    if (m_stunTimer > 0) {
        m_stunTimer -= dt;
        if (m_onGround) {
            m_velX = 0.0f; // ínñ Ç…Ç¢ÇÍÇŒé~Ç‹ÇÈ
        }
    }
}

void Enemy::updatePhysics(float dt, const vector<BoxCollider>& grounds) {
    m_velY += 1500.0f * dt; // èdóÕ

    SDL_FRect r = m_collider.rect();
    r.x += m_velX * dt;
    r.y += m_velY * dt;
    m_collider.setPosition(r.x, r.y);

    bool onGround = BoxCollider::resolveCollision(m_collider, m_velX, m_velY, grounds);
    setOnGround(onGround);
}