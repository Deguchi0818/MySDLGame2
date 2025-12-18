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
	// Player‚ª”j‰ó‚³‚ê‚½Žžtexture‚ð”j‰ó
	if (m_texture)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
}

void Player::update(float dt, int screenW, int screenH) 
{
	SDL_FRect r = m_collider.rect();

	m_prevRect = m_collider.rect();

	const bool* keys = SDL_GetKeyboardState(nullptr);

	bool curA = keys[SDL_SCANCODE_A];
	bool curD = keys[SDL_SCANCODE_D];

	bool curJump = keys[SDL_SCANCODE_SPACE];
	bool jumpPressed = (curJump && !prevJumpPressed);   // u‰Ÿ‚µ‚½uŠÔv
	prevJumpPressed = curJump;

	bool canJump = m_onGround;
	velX *= 0.0f;

	if (!curJump && isHovering)
	{
		isHovering = false;
	}

	if (curA) 
	{
		velX -= speed;
	}

	if (curD) 
	{
		velX += speed;
	}

	float currentGravity = gravity;
	if (isHovering) 
	{
		currentGravity = hoverGravity;
	}
	else
	{
		if (velY > 0) 
		{
			currentGravity *= fallMultiplier;
		}
	}
	if (curJump && canJump) 
	{
		velY = jumpPower;
		isHovering = false;
		setOnGround(false);
	}
	else if(jumpPressed)
	{
		isHovering = true;
		velY = hoverFlapSpeed;
	}

	r.x += velX * dt;
	velY += currentGravity * dt;
	r.y += velY *dt;

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

void Player::setOnGround(bool on) 
{
	if (on) 
	{
		velY = 0.0f;
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

void Player::resetPosition(float x, float y) 
{
	m_collider.setPosition(x, y);

	velY = 0;

	m_prevRect = m_collider.rect();
}