#include "Player.h"
#include <SDL3_image/SDL_image.h>

Player::Player(SDL_Renderer* renderer,
	float x, float y, float w, float h,
	const char* texturePath)
	:m_collider(x, y, w, h) 
{
	// 画像（テクスチャ）の読み込みと初期状態の設定
	m_texture = IMG_LoadTexture(renderer, texturePath);
	if (!m_texture) {
		SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
	}
	changeState(make_unique<IdleState>());
}
Player::~Player() 
{
	// プレイヤー破棄時にテクスチャメモリを解放
	if (m_texture)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
}

void Player::update(float dt, int screenW, int screenH) 
{
	// 前回の座標を保存
	m_prevRect = m_collider.rect();

	const bool* keys = SDL_GetKeyboardState(nullptr);

	updateInputState(keys);

	if (m_currentState)
	{
		m_currentState->handleInput(*this, keys);
		m_currentState->update(*this, dt);
	}

	// タイマーの更新
	updateTimers(dt);

	applyPhysics(dt);			// 動きの更新
	attack(keys, dt);

	checkScreenBounds(screenW, screenH);

	
}

// 地面についているかの判定
void Player::setOnGround(bool on) 
{
	if (on) 
	{
		// 着地時に垂直速度をリセットし、コヨーテタイムを付与
		velY = 0.0f;
		coyoteTimer = m_params.coyoteTimeMax;
	}
	m_onGround = on;
}

bool Player::isOnGround() const 
{
	return m_onGround;
}

// 動きの更新
void Player::applyPhysics(float dt) 
{
	SDL_FRect r = m_collider.rect();

	r.x += velX * dt;
	r.y += velY * dt;

	m_collider.setPosition(r.x, r.y);
}

// 各種猶予時間やクールダウンのカウントダウン
void Player::updateTimers(float dt)
{
	if (coyoteTimer > 0)
	{
		coyoteTimer -= dt;
	}
	if (jumpBufferTimer > 0)
	{
		jumpBufferTimer -= dt;
	}
	if (m_fireTimer > 0) 
	{
		m_fireTimer -= dt;
	}
	if (m_invincibleTimer > 0)
	{
		m_invincibleTimer -= dt;
	}
}

// 画面外に出ないようにする
void Player::checkScreenBounds(float screenW, float screenH) 
{
	SDL_FRect r = m_collider.rect();
	if (r.x < 0)
	{
		r.x = 0;
	}
	if (r.x + r.w > screenW)
	{
		r.x = screenW - r.w;
	}

	if (r.y + r.h > screenH)
	{
		r.y = screenH - r.h;
		velY = 0.0f;
	}

	m_collider.setPosition(r.x, r.y);
}

// プレイヤーの位置をリセット
void Player::resetPosition(float x, float y) 
{
	m_collider.setPosition(x, y);

	velY = 0;

	m_prevRect = m_collider.rect();
}

// 攻撃処理
void Player::attack(const bool* keys, float dt) 
{
	bool curAttack = keys[SDL_SCANCODE_LSHIFT];

	if (keys[SDL_SCANCODE_LSHIFT] && m_fireTimer <= 0)
	{
		m_wantsToShoot = true;
		m_fireTimer = m_fireCooldown;

		float vx = m_facingDir; // 基本は向いている方向
		float vy = 0.0f;

		if (keys[SDL_SCANCODE_W]) 
		{
			vy = -1.0f;
			if (!keys[SDL_SCANCODE_LEFT] && !keys[SDL_SCANCODE_RIGHT]) {
				vx = 0.0f;
			}
		}

		else if (keys[SDL_SCANCODE_S] && !isOnGround()) 
		{
			vy = 1.0f;
			if (!keys[SDL_SCANCODE_LEFT] && !keys[SDL_SCANCODE_RIGHT]) {
				vx = 0.0f;
			}
		}

		else if (keys[SDL_SCANCODE_Q]) 
		{
			vy = -1.0f;
			vx;
		}

		else if (keys[SDL_SCANCODE_E])
		{
			vy = 1.0f;
			vx;
		}

		m_currentAim = { vx, vy };
	}
}

// ノックバックの適用
void Player::applyKnockback(float forceX,  float forceY)
{
	velX = forceX;
	velY = forceY;
	setOnGround(false);
}

void Player::changeState(std::unique_ptr<PlayerState> newState) {
	m_currentState = std::move(newState);
}

// ダメージを受けた時の処理
void Player::takeDamage(int damage)
{
	// 無敵時間中はダメージ処理をスキップ
	if (m_invincibleTimer > 0) return;

	m_currentHp -= damage;
	if (m_currentHp < 0)
	{
		m_currentHp = 0;
	}

	m_invincibleTimer = INVINCIBLE_TIME;
}

void Player::render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset)
{
	SDL_FRect dst = m_collider.rect();

	// ワールド座標からスクリーン座標へ変換
	dst.x -= cameraOffset.x;
	dst.y -= cameraOffset.y;

	if (m_invincibleTimer > 0) 
	{
		if (static_cast<int>( m_invincibleTimer * 10) % 2 == 0)
			return;
	}

	if (!m_texture)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &dst);
		return;
	}

	SDL_FlipMode flip = (m_facingDir > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RenderTextureRotated(renderer, m_texture, nullptr, &dst, 0.0f, nullptr, flip);
}