#pragma once
#include "Collider.h"
#include "Player.h"
#include "Bullet.h"
#include <SDL3/SDL.h>
#include<iostream>
#include<vector>

// 敵キャラクターの基底クラス。

class Enemy
{
public:
	Enemy(float x, float y, float w, float h, SDL_Texture* texture)
		: m_collider(x, y, w, h), m_isDead(false), m_texture(texture)
	{
		speed = 100;
	}

	virtual ~Enemy() = default;
	virtual void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds) = 0;

	virtual void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) 
	{
		if (m_isDead) {
			return;
		}
		SDL_FRect drawRect = m_collider.rect();
		drawRect.x -= cameraOffset.x;
		drawRect.y -= cameraOffset.y;

		if (m_texture) 
		{

			if (m_flashTimer > 0) {
				SDL_SetTextureColorMod(m_texture, 255, 100, 100);
			}
			else {
				SDL_SetTextureColorMod(m_texture, 255, 255, 255);
			}
			SDL_RenderTexture(renderer, m_texture, nullptr, &drawRect);
		}
		else 
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &drawRect);
		}
	}

	bool isDead() const { return m_isDead; }// 死亡フラグの確認
	bool isDying() const { return m_isDying; }
	void die() { m_isDead = true; }
	bool isActive() const { return m_isActive; }
	void activate() { m_isActive = true; }
	const BoxCollider& collider() const { return m_collider; }
	void handleStunState(float dt);
	virtual void updatePhysics(float dt, const std::vector<BoxCollider>& grounds);
	
	// ダメージを受けた際、指定された力でキャラクターを弾き飛ばす。同時にスタン状態へ移行させる。
	virtual void applyKnockback(float forceX, float forceY) {
		m_velX = forceX;
		m_velY = forceY;
		setOnGround(false);
		m_stunTimer = 1.0f;
	}

	// 地面についているか
	void setOnGround(bool on) {
		if (on)
		{
			m_velY = 0.0f;
		}
		m_onGround = on;
	}
	// 接地状態の確認
	bool isOnGround()  const
	{
		return m_onGround;
	}
	// スタン中（行動不能）かどうかの判定
	bool isStunned() const { return m_stunTimer > 0; }

	// ダメージを受けた時
	virtual void takeDamage() 
	{
		if (m_isDead || m_flashTimer > 0) return;

		--m_hp;
		m_flashTimer = 0.1f;
		applyKnockback(1.0f, 0.0f);

		if (m_hp <= 0) 
		{
			die();
		}
	}

	void updateFlashTimer(float dt) {
		if (m_flashTimer > 0) {
			m_flashTimer -= dt;
		}
	}

	virtual void checkPlayerCollision(Player& player) {}

	virtual std::vector<unique_ptr<Bullet>>& getBullets() {
		static std::vector<unique_ptr<Bullet>> empty;
		return empty;
	}
	virtual bool isBoss() const { return false; }
protected:
	BoxCollider m_collider;
	SDL_Texture* m_texture;
	bool m_isDead = false;		// 生きているかどうか
	bool m_isDying = false;
	bool m_isActive = false;	// 画面に入って動き始めたかどうか
	float m_velX = 0;
	float m_velY = 0;
	float gravity = 1500.0f;	// 下方向への重力加速度
	float speed = 100;			// 移動スピード
	float m_stunTimer = 0.0f;	// スタンの残り時間(秒)。
	int m_hp = 3;				// 残りhp
	bool m_onGround = false;	// 地面にいるかどうか
	float m_flashTimer = 0.0f;

	

};

