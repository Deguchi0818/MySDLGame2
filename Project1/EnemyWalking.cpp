#include "EnemyWalking.h"
using namespace std;
void EnemyWalking::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)
{
    updateFlashTimer(dt);
    handleStunState(dt);

    if (m_stunTimer <= 0)
    {

        if (std::abs(m_velX) < 0.1f)
        {
            m_velX = -speed;
        }

        // ノックバックから復帰した瞬間や、静止している時に歩き出させる
        else if (std::abs(m_velX) != speed) 
        {
            m_velX = (m_velX > 0) ? speed : -speed;
        }

    }

    float beforeVelX = m_velX;  // 判定前の速度を保存（後で比較するため）

    updatePhysics(dt, grounds);

    handleWallBounce(beforeVelX);
}

// 壁に当たった時
void EnemyWalking::handleWallBounce(float beforeVelX) 
{
    if (m_stunTimer <= 0 && m_velX == 0 && beforeVelX != 0)
    {
        m_stuckFrames++; // 引っかかりカウンターを増やす

        // 3フレーム（約0.05秒）連続で進めなかったら「本物の壁」とみなす
        if (m_stuckFrames > 3)
        {
            m_velX = (beforeVelX > 0) ? -speed : speed; // Uターン
            m_stuckFrames = 0; // カウンターをリセット
        }
        else
        {
            m_velX = beforeVelX;
        }
    }
    else if (m_velX != 0)
    {
        // 正常に動けている間はカウンターをリセットしておく
        m_stuckFrames = 0;
    }
}
