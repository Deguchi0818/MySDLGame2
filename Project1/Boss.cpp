#include "Boss.h"

void Boss::update(float dt, const SDL_FRect& playerRect, const Player& player, const std::vector<BoxCollider>& grounds)
{
    m_stateTimer += dt;
    updateFlashTimer(dt);

    float isAngry = (m_hp <= 25);
    float currentSpeed = isAngry ? speed * 1.5f : speed;

    switch (m_currentState)
    {
    case BossState::Idle:
        m_velX = 0;
        m_dashDir = (playerRect.x > m_collider.rect().x) ? 1.0f : -1.0f;

        if (m_stateTimer > 2.0f) 
        {
            if (isAngry && (rand() % 100 < 50) )
            {
                m_currentState = BossState::JumpUp;
            }
            else
            {
                m_currentState = BossState::Prepare;
            }

            m_stateTimer = 0.0f;
        }
        break;

    case BossState::JumpUp: 
    {
        m_velY = -1100.0f;

        // 上昇中も少しだけプレイヤーに近づく
        float targetX = playerRect.x + (playerRect.w / 2.0f) - (m_collider.rect().w / 2.0f);
        if (m_collider.rect().x < targetX - 20.0f) m_velX = currentSpeed * 0.5f;
        else if (m_collider.rect().x > targetX + 20.0f) m_velX = -currentSpeed * 0.5f;

        // 高度が十分に達したか、天井にぶつかったらHoverへ
        if (m_collider.rect().y < playerRect.y - 300.0f || m_stateTimer > 0.8f) {
            m_currentState = BossState::Hover;
            m_stateTimer = 0.0f;
        }
        break;
    }
       

    case BossState::Hover: 
    {  
        m_velY = 0;
        float targetX = playerRect.x + (playerRect.w / 2.0f) - (m_collider.rect().w / 2.0f);

       
        if (m_stateTimer < 0.8f) {
            if (m_collider.rect().x < targetX - 5.0f) m_velX = currentSpeed * 1.2f;
            else if (m_collider.rect().x > targetX + 5.0f) m_velX = -currentSpeed * 1.2f;
            else m_velX = 0;
        }
      
        else {
            m_velX = 0;
        }
        if (m_stateTimer > 1.2f) {
            m_currentState = BossState::Fall;
            m_stateTimer = 0.0f;
        }
        break;
    }

    case BossState::Fall:
        m_velX = 0;
        m_velY = 1500.0f;

        if (isOnGround()) { 
            m_currentState = BossState::Idle;
            m_stateTimer = 0.0f;
        }
        break;

    case BossState::Prepare:
        m_velX = 0;
        if (m_stateTimer > 1.5f)
        {
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

    float gravityBackup = gravity;
    if (m_currentState == BossState::Hover) gravity = 0;

    float beforeVelX = m_velX;
    updatePhysics(dt, grounds);

    gravity = gravityBackup;

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

    //SDL_FRect dst = m_collider.rect();

    SDL_Texture* currentTex = (m_currentState == BossState::Dash) ? m_dashTexture : m_texture;

    if (!currentTex) {
        currentTex = m_texture;
    }

    float tw, th;
    SDL_GetTextureSize(currentTex, &tw, &th);   // テクスチャが本来どんな形（幅 tw、高さ th）なのか

    float drawH = m_collider.rect().h; // 基準となる高さ
    float aspect = tw / th;            // アスペクト比 高さ 1 に対して幅がどれくらいあるか
    float drawW = drawH * aspect;      // 高さに合わせて幅を出す

    SDL_FRect dst;
    dst.w = drawW;
    dst.h = drawH;
    dst.x = (m_collider.rect().x + m_collider.rect().w / 2.0f) - (drawW / 2.0f) - cameraOffset.x;
    dst.y = (m_collider.rect().y + m_collider.rect().h) - drawH - cameraOffset.y;

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