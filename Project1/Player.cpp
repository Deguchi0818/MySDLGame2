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

	// タイマーの更新
	updateTimers(dt);

	horizontalMove(keys, dt);	// 横方向の動き
	jump(keys, dt);				// ジャンプ
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

	SDL_RenderTexture(renderer, m_texture, nullptr, &dst);
}

void Player::horizontalMove(const bool* keys, float dt)
{
	bool curA = keys[SDL_SCANCODE_A];
	bool curD = keys[SDL_SCANCODE_D];

	velX *= 0.0f;

	if (curA)
	{
		velX -= m_params.speed;
		m_facingDir = -1.0f;
	}

	if (curD)
	{
		velX += m_params.speed;
		m_facingDir = 1.0f;
	}

}

void Player::jump(const bool* keys, float dt) 
{
	bool curJump = keys[SDL_SCANCODE_SPACE];
	bool jumpPressed = (curJump && !prevJumpPressed);   // 「押した瞬間」
	prevJumpPressed = curJump;

	bool canJump = m_onGround || (coyoteTimer > 0);
	if (jumpPressed)
	{
		jumpBufferTimer = m_params.jumpBufferMax;
	}


	if (!curJump && isHovering)
	{
		isHovering = false;
	}



	float currentGravity = m_params.gravity;
	if (isHovering)
	{
		currentGravity = m_params.hoverGravity;
	}
	else
	{
		if (velY > 0)
		{
			currentGravity *= m_params.fallMultiplier;
		}
	}

	if (jumpBufferTimer > 0)
	{
		if (curJump && canJump)
		{
			velY = m_params.jumpPower;
			isHovering = false;
			coyoteTimer = 0.0f;
			jumpBufferTimer = 0.0f;
			setOnGround(false);
		}
		else if (jumpPressed)
		{
			isHovering = true;
			coyoteTimer = 0.0f;
			jumpBufferTimer = 0.0f;
			velY = m_params.hoverFlapSpeed;
		}
	}

	velY += currentGravity * dt;

	if (isHovering && velY > m_params.hoverFallMaxSpeed)
	{
		velY = m_params.hoverFallMaxSpeed;
	}

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