#pragma once
#include "Collider.h"
#include "PlayerState.h"
#include "IdleState.h"

#include <SDL3/SDL.h>
using namespace std;

struct PlayerParams 
{
	float speed = 0.0f;				//　移動速度
	float gravity = 1500.0f;		// 重力加速度
	float jumpPower = -500.0f;		// ジャンプ力
	float fallMultiplier = 12.0f;	// 落下加速倍率

	// ホバリング
	float hoverFlapSpeed = -350.0f;		// ホバー時の上昇速度
	float hoverGravity = 250.0f;		// ホバー時の重力加速度
	float hoverFallMaxSpeed = 150.0f;	// ホバー時の最大落下速度

	// コヨーテタイム
	float coyoteTimeMax = 0.12f;		// コヨーテタイム

	float jumpBufferMax = 0.12f;		// ジャンプバッファタイム

	int m_hp = 100.0f;					// プレイヤーの初期HP
};

struct AimDir {
	float vx;
	float vy;
};

class Player
{
public:
	Player(SDL_Renderer* renderer,
		float x, float y, float w, float h,
		const char* texturtPath);

	~Player();

	void update(float dt, int m_width, int m_height);
	void render(SDL_Renderer* rendere, const SDL_FPoint& cameraOffset);

	// パラメータの適用
	void applyParams(const PlayerParams& p) 
	{ 
		m_params = p;
		m_currentHp = m_params.m_hp;
	}

	BoxCollider& collider() { return m_collider; }
	const BoxCollider& collider() const { return m_collider; }

	void setOnGround(bool on);								// 地面についているかの設定
	bool isOnGround() const;								// 地面についているかの取得
	void setIsHovering(bool hover) { isHovering = hover; }	// hovering状態の設定
	bool getIsHovering() const { return isHovering; }		// hovering状態の取得
	float getCoyoteTimer() const { return coyoteTimer; }	// コヨーテタイムの取得
	void setCoyoteTimer(float time) { coyoteTimer = time; }	// コヨーテタイムの設定
	float getJumpBufferTimer() const { return jumpBufferTimer; }	// ジャンプバッファタイムの取得	
	void setJumpBufferTimer(float time) { jumpBufferTimer = time; }	// ジャンプバッファタイムの設定
	void resetPosition(float x, float y);					// プレイヤーの位置をリセット
	int getCurrentHp() const { return m_currentHp; }		// 現在のHPを取得
	int getMaxHp() const { return m_params.m_hp; }			// 最大HPを取得
	float getInvincibleTimer() const { return m_invincibleTimer; } // 無敵タイムを取得 
	

	bool isJumpTriggered() const { return m_jumpTriggered; }	// ジャンプがトリガーされたか

	// 入力状態の更新
	void updateInputState(const bool* keys) {
		bool current = keys[SDL_SCANCODE_SPACE];
		m_jumpTriggered = (current && !m_prevJumpKey); 
		m_prevJumpKey = current;
	}

	bool wantsToShoot() const { return m_wantsToShoot; }	// 弾を撃ちたいか
	void consumeShootFlag() { m_wantsToShoot = false; } // フラグを消費（リセット）
	float getFacingDir() const { return m_facingDir; }  // 向きを教える
	AimDir getAimDir() const { return m_currentAim; }	// エイム方向を取得
	void applyKnockback(float forceX, float forceY);	// ノックバックの適用

	float velX = 0.0f;
	float velY = 0.0f;
	float m_facingDir = 1.0f; // 向き（1.0f: 右、-1.0f: 左）

	const SDL_FRect& getPrevRect() const { return m_prevRect; }


	void changeState(unique_ptr<PlayerState> newState);

	const PlayerParams& getParams() const { return m_params; }

	// ダメージを受けた時の処理
	bool takeDamage(int damage);

private:
	BoxCollider m_collider;

	SDL_FRect m_prevRect;

	void applyPhysics(float dt);							// 動きの更新
	void updateTimers(float dt);							// タイマーの更新
	void checkScreenBounds(float screenW, float screenH);	// 画面外判定のチェック
	void attack(const bool* kyes, float dt);				// 攻撃処理
	

	PlayerParams m_params;

	float jumpBufferTimer = 0.0f;	// ジャンプバッファタイム
	float coyoteTimer = 0.0f;		// コヨーテタイム

	bool isHovering;		// ホバリング状態かどうか
	bool m_onGround = false;// 地面についているかどうか

	float m_fireCooldown = 0.7f; // 発射間隔（秒）
	float m_fireTimer = 0.0f;    // 残り待ち時間
	bool m_wantsToShoot = false; // 弾を撃ちたいフラグ

	bool m_jumpTriggered = false; // ジャンプがトリガーされたか
	bool m_prevJumpKey = false;	  // 前回のジャンプキーの状態

	int m_currentHp;					// 現在のHP
	float m_invincibleTimer = 0.0f;		// 無敵タイマー
	const float INVINCIBLE_TIME = 1.0f;	// ダメージ後の無敵時間(秒)

	AimDir m_currentAim = { 1.0f, 0.0f }; // 現在のエイム方向

	SDL_Texture* m_texture = nullptr;

	unique_ptr<PlayerState> m_currentState;
};

