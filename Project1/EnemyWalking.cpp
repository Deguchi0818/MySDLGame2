#include "EnemyWalking.h"

void EnemyWalking::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)
{
    m_velY += 1500.0f * dt; // Playerと同じくらいの重力

    if (m_stunTimer > 0)
    {
        m_stunTimer -= dt;
        if (m_onGround) {
            m_velX = 0.0f; // 地面にいる時だけ足を止める
        }
    }
    else 
    {
        if (abs(m_velX) > speed)
        {
            m_velX = (m_velX > 0) ? speed : -speed;
        }

        if (abs(m_velX) != speed)
        {
            if(m_velX == 0) m_velX = -speed;
        }
    }

    float beforeVelX = m_velX;  // 判定前の速度を保存（後で比較するため）

    SDL_FRect r = m_collider.rect();
    r.x += m_velX * dt;
    r.y += m_velY * dt;
    m_collider.setPosition(r.x, r.y);

    bool onGround = BoxCollider::resolveCollision(m_collider, m_velX, m_velY, grounds);
    setOnGround(onGround);

    //if (m_velX == 0) m_velX = -beforeVelX;
    if (m_stunTimer <= 0 && m_velX == 0 && beforeVelX != 0) {
        m_velX = (beforeVelX > 0) ? -speed : speed;
    }
}