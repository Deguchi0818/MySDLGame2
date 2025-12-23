#include "WalkingEnemy.h"

void WalkingEnemy::update(float dt, const SDL_FRect& playerRect, const vector<BoxCollider>& grounds) 
{
    m_velY += 1500.0f * dt; // Playerと同じくらいの重力

    // 2. とりあえず左に歩く
    if (m_velX == 0) m_velX = -100.0f;

    float beforeVelX = m_velX;  // 判定前の速度を保存（後で比較するため）
    // 3. 移動を適用
    SDL_FRect r = m_collider.rect();
    r.x += m_velX * dt;
    r.y += m_velY * dt;
    m_collider.setPosition(r.x, r.y);

    // 4. 【重要】共通化した衝突判定を呼び出す
    BoxCollider::resolveCollision(m_collider, m_velX, m_velY, grounds); // m_velXが０になる

    if (m_velX == 0) m_velX = -beforeVelX;
}