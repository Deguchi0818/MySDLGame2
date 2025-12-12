#include "Player.h"
#include <SDL3_image/SDL_image.h>

Player::Player(SDL_Renderer* renderer,
    float x, float y, float w, float h,
    const char* texturePath)
    : m_collider(x, y, w, h)
{
    m_texture = IMG_LoadTexture(renderer, texturePath);
    if (!m_texture) {
        SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
    }
}

Player::~Player() 
{
    if (m_texture) 
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

void Player::setOnGround(bool on) 
{
    if (on) 
    {
        coyoteTimer = coyoteTimeMax;    // ジャンプの猶予時間を与える
        velY = 0.0f;
    }

    m_onGround = on;
}

bool Player::isOnGround() const {
    return m_onGround;
}

void Player::snapToGround(float groundY) {
    SDL_FRect r = m_collider.rect();
    r.y = groundY - r.h;
    m_collider.setPosition(r.x, r.y);

    setOnGround(true);
}

void Player::update(float dt, int screenW, int screenH)
{
    SDL_FRect r = m_collider.rect();
    // 前フレームの矩形を記録
    m_prevRect = m_collider.rect();

    if (coyoteTimer > 0.0f) 
    {
        coyoteTimer -= dt;      // コヨーテタイマーを徐々に減らす
    }
    if (jumpBufferTimer > 0.0f) 
    {
        jumpBufferTimer -= dt;
    }
	if (doubleTapTimer > 0.0f)
	{
		doubleTapTimer -= dt;
	}

    const bool* keys = SDL_GetKeyboardState(nullptr);

	bool curA = keys[SDL_SCANCODE_A];
	bool curD = keys[SDL_SCANCODE_D];

	bool downA = curA && !prevAPressed;
	bool downD = curD && !prevDPressed;

	prevAPressed = curA;
	prevDPressed = curD;

    bool curJump = keys[SDL_SCANCODE_SPACE];
    bool jumpPressed = (curJump && !prevJumpPressed);   // 「押した瞬間」
    prevJumpPressed = curJump;
    bool isMoving = false;

    if (!curJump && isHovering) 
    {
        isHovering = false;
    }

    if (downA || downD) 
    {
		if (doubleTapTimer > 0.0f)
		{
			isDashing = true;
		}
        else
        {
			doubleTapTimer = doubleTapTimerMax;
            if (!isDashing)
            {
                isDashing = false;
            }
        }
    }

    if (!curA && !curD) {
        isDashing = false;
    }

    // 左右移動
    if (curA) {
        r.x -= speed * dt;
		isMoving = true;
		if (isDashing && dashDirection == -1)
		{
            r.x -= speed * dashSpeedMultiplier * dt;    // speedにダッシュスピード倍率をかける
		}
        else
        {
            dashDirection = -1;
        }
		
    }

    if (curD) {
        r.x += speed * dt;
        isMoving = true;
        if (isDashing && dashDirection == 1) 
        {
            r.x += speed * dashSpeedMultiplier * dt;
        }
        else
        {
            dashDirection = 1;
        }
		
    }

    // jump
    if (jumpPressed) {
        jumpBufferTimer = jumpBufferMax; // バッファに入れる
    }

    bool canGroundJump = m_onGround || (coyoteTimer > 0.0f);

    if (jumpBufferTimer > 0.0f) 
    {
        // ジャンプ
        if (canGroundJump) 
        {
            velY = jumpSpeed;
            m_onGround = false;
            coyoteTimer = 0.0f;
            jumpBufferTimer = 0.0f;
            isHovering = false;
        }
        // ホバリング
        else if (jumpPressed) 
        {
            velY = hoverFlapSpeed;
            isHovering = true;
            jumpBufferTimer = 0.0f;
        }
    }
    
    float currentGravity = gravity;
    if (isHovering) 
    {
        currentGravity = hoverGravity;
    }
    else
    {
        if (velY > 0) {
            currentGravity *= fallMultiplier;   // 落下時に落下速度を速くする
        }
    }

    velY += currentGravity * dt;    // 重力をかける

    if (isHovering) 
    {
        if (velY > hoverMaxFallSpeed) {
            velY = hoverMaxFallSpeed;
        }
    }

    r.y += velY * dt;

    if (r.x < 0) 
    {
        r.x = 0;
    }

    //if (r.y < -50) 
    //{
    //    r.y = -50;
    //}

    if (r.x + r.w > screenW) 
    {
        r.x = screenW - r.w;
    }


    //if (r.y + r.h > screenH)
    //{
    //    r.y = screenH - r.h;
    //    velY = 0.0f;
    //    setOnGround(true);
    //}

    if (isMoving == false) 
    {
        time += dt;
    }
	else
	{
		time = 0;
	}


    m_collider.setPosition(r.x, r.y);
}

void Player::resetPosition(float x, float y)
{
    m_collider.setPosition(x, y);

	velY = 0.0f;
	isHovering = false;
	m_onGround = false;
	coyoteTimer = 0.0f;

    m_prevRect = m_collider.rect();
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
    if (time > 0 && m_onGround) 
    {
        dst.y += sin(time * 5) * 4.0f;
    }

    SDL_RenderTexture(renderer, m_texture, nullptr, &dst);
}

