#include "EnemyChase.h"


void EnemyChase::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)
{
    updateFlashTimer(dt);
    handleStunState(dt); // スタンタイマーを減らす処理

    if (m_stunTimer <= 0)
    {
        float dx = player.collider().rect().x - m_collider.rect().x;     // x方向の向きと差分を求めている 
        float distance = abs(dx);                                        // dxを絶対値に変換して距離を求めている 

        if (distance < m_detectRange)
        {
            m_currentState = EnemyState::CHASE;
        }
        else
        {
            m_currentState = EnemyState::PATOROL;
        }

        switch (m_currentState)
        {
        case EnemyState::PATOROL:
            patrol(dt);
            break;
        case EnemyState::CHASE:
            chase(dt, dx);
            break;
        }
    }

    updatePhysics(dt, grounds);
}

void EnemyChase::patrol(float dt)
{
    m_velX = 0; 
}

void EnemyChase::chase(float dt, float dx)
{
    // 方向を決める
    m_velX = (dx > 0) ? 1.0f : -1.0f;

    float chaseSpeed = speed * 0.5f;
    m_velX *= chaseSpeed;
}