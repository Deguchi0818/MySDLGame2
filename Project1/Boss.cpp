#include "Boss.h"

void Boss::update(float dt, const SDL_FRect& playerRect, const Player& player, const std::vector<BoxCollider>& grounds)
{
    m_stateTimer += dt;
    updateFlashTimer(dt);

    switch (m_currentState)
    {
    case BossState::Idle:
        m_velX = 0;
        if (m_stateTimer > 2.0f) {
            m_currentState = BossState::Prepare;
            m_stateTimer = 0.0f;
        }
        break;

    case BossState::Prepare:
        m_velX = 0;
        if (m_stateTimer > 1.5f)
        {
            // 突進する向きを決定し、速度をセット
            m_dashDir = (playerRect.x > m_collider.rect().x) ? 1.0f : -1.0f;
            m_velX = m_dashDir * speed;
            m_currentState = BossState::Dash;
            m_stateTimer = 0.0f;
        }
        break;

    case BossState::Dash:
        m_velX = m_dashDir * speed;
        break;

    case BossState::Stun:
        m_velX = 0;
        if (m_stateTimer > 3.0f)
        {
            m_currentState = BossState::Idle;
            m_stateTimer = 0.0f;
        }
        break;
    }

    float beforeVelX = m_velX;
    updatePhysics(dt, grounds);

    if (m_currentState == BossState::Dash)
    {
        if (m_velX == 0 && beforeVelX != 0)
        {
            m_currentState = BossState::Stun;
            m_stateTimer = 0.0f;
        }
    }
}

void Boss::takeDamage() 
{
	if (m_isDead || m_flashTimer > 0) return;

	m_hp -= (m_currentState == BossState::Stun) ? 2 : 1;

	m_flashTimer = 0.1f;
	if (m_hp <= 0) die();
}

void Boss::applyKnockback(float forceX, float forceY)
{

}

void Boss::render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) {
    if (m_isDead) return;

    SDL_FRect dst = m_collider.rect();

    SDL_Texture* currentTex = (m_currentState == BossState::Dash) ? m_dashTexture : m_texture;

    if (!currentTex) {
        currentTex = m_texture;
    }

    // ワールド座標からスクリーン座標へ変換
    dst.x -= cameraOffset.x;
    dst.y -= cameraOffset.y;;

    // 「溜め」状態なら、描画位置をランダムにずらして震わせる
    if (m_currentState == BossState::Prepare) {
        dst.x += (rand() % 10 - 5) * 1.5f;
        dst.y += (rand() % 10 - 5) * 1.5f;
    }

    if (m_flashTimer > 0) {
        SDL_SetTextureColorMod(currentTex, 255, 100, 100);
    }
    else {
        SDL_SetTextureColorMod(currentTex, 255, 255, 255);
    }

    SDL_FlipMode flip = (m_dashDir > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderTextureRotated(renderer, currentTex, nullptr, &dst, 0.0f, nullptr, flip);
}