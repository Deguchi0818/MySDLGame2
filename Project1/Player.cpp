#include "Player.h"
#include <SDL3_image/SDL_image.h>

Player::Player(SDL_Renderer* renderer,
	float x, float y, float w, float h,
	const char* texturePath)
	:m_collider(x, y, w, h) 
{
	m_texture = IMG_LoadTexture(renderer, texturePath);
	if (!m_texture) {
		SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
	}
	changeState(make_unique<IdleState>());
}
Player::~Player() 
{
	// Playerが破壊された時textureを破壊
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

void Player::setOnGround(bool on) 
{
	if (on) 
	{
		velY = 0.0f;
		coyoteTimer = m_params.coyoteTimeMax;
	}
	m_onGround = on;
}

bool Player::isOnGround() const 
{
	return m_onGround;
}


void Player::render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset)
{
	SDL_FRect dst = m_collider.rect();

	dst.x -= cameraOffset.x;
	dst.y -= cameraOffset.y;

	if (!m_texture)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &dst);
		return;
	}

	SDL_FlipMode flip = (m_facingDir > 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RenderTextureRotated(renderer, m_texture, nullptr, &dst, 0.0f, nullptr, flip);
}



void Player::applyPhysics(float dt) 
{
	SDL_FRect r = m_collider.rect();

	r.x += velX * dt;
	r.y += velY * dt;

	m_collider.setPosition(r.x, r.y);
}

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
}

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

void Player::resetPosition(float x, float y) 
{
	m_collider.setPosition(x, y);

	velY = 0;

	m_prevRect = m_collider.rect();
}

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

void Player::applyKnockback(float forceX,  float forceY)
{
	velX = forceX;
	velY = forceY;
	setOnGround(false);
}

void Player::changeState(std::unique_ptr<PlayerState> newState) {
	m_currentState = std::move(newState);
}