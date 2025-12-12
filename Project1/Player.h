#pragma once
#include "Collider.h"

#include <SDL3/SDL.h>

class Player
{
public:
	Player(SDL_Renderer* renderer,
		float x, float y, float w, float h,
		const char* texturePath);

	~Player();

	void update(float dt, int m_width, int m_height);
	void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset);

	//SDL_FRect getBounds() const;
	//void setPosition(float newX, float newY);

	BoxCollider& collider() { return m_collider; }
	const BoxCollider& collider() const { return m_collider; }

	void setOnGround(bool on);
	bool isOnGround() const;
	void snapToGround(float groundY);

	void resetPosition(float x, float y);

	const SDL_FRect& getPrevRect() const { return m_prevRect; }

	float velY = 0.0f;		// 縦速度
private:
	BoxCollider m_collider;

	SDL_FRect m_prevRect;	// 前フレームの矩形（衝突方向判定用）

	float speed = 150.0f;				// 横速度
	float gravity = 1500.0f;			// 重力
	float jumpSpeed = -600.0f;			// ジャンプ初速
	float fallMultiplier = 1.2f;		// 落下時の重力倍率
	float dashSpeedMultiplier = 1.1f;	// ダッシュ時の横速度倍率
	float doubleTapTimerMax = 0.25f;	// ダブルタップ受付時間
	float doubleTapTimer = 0.0f;
	bool prevAPressed = false;
	bool prevDPressed = false;
	bool isDashJumping = false;
	bool isDashing = false;
	int dashDirection = 0;			// -1: 左ダッシュ, 1: 右ダッシュ


	bool isHovering = false;
	float hoverFlapSpeed = -350.0f;		// 空中ジャンプ（羽ばたき）の強さ
	float hoverGravity = 400.0f;		// ホバリング中の弱い重力
	float hoverMaxFallSpeed = 150.0f;	// ホバリング中の最大落下速度

	// コヨーテタイム
	float coyoteTimeMax = 0.12f;
	float coyoteTimer = 0.0f;

	float jumpBufferMax = 0.12f;
	float jumpBufferTimer = 0.0f;

	float time = 0;

	bool m_onGround = false;
	bool isMoving = false;
	bool prevJumpPressed;

	SDL_Texture* m_texture = nullptr;
};

