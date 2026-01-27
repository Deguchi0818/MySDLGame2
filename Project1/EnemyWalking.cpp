#include "EnemyWalking.h"
using namespace std;
void EnemyWalking::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)
{
    updateFlashTimer(dt);
    handleStunState(dt);

    if (m_stunTimer <= 0)
    {
        // ノックバックから復帰した瞬間や、静止している時に歩き出させる
        if (abs(m_velX) != speed) {
            m_velX = (m_velX >= 0) ? speed : -speed;
        }
        if (m_velX == 0) m_velX = -speed;
    }

    float beforeVelX = m_velX;  // 判定前の速度を保存（後で比較するため）

    updatePhysics(dt, grounds);

    handleWallBounce(beforeVelX);
}

void EnemyWalking::handleWallBounce(float beforeVelX) 
{
    //if (m_velX == 0) m_velX = -beforeVelX;
    if (m_stunTimer <= 0 && m_velX == 0 && beforeVelX != 0) {
        m_velX = (beforeVelX > 0) ? -speed : speed;
    }
}
