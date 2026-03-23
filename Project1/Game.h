#pragma once

#include<SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include <cstdlib>


#include "Player.h"
#include "Enemy.h"
#include "EnemyWalking.h"
#include "EnemyChase.h"
#include "EnemyShooter.h"
#include "EnemyTimedPatrol.h"
#include "Boss.h"
#include "Collider.h"
#include "Bullet.h"
#include "Door.h"
#include "ResourceManager.h"
#include "Goal.h"


enum class GameStatus {
	Title,
	Playing,
	BossBattle,
	Clear,
	GameOver
};

class Game
{
public:
	Game() = default;
	~Game();

	// SDLの初期化とウィンドウ/レンダラー作成
	bool init(const string& title, int width, int height);
	
	void run();
private:
	GameStatus m_status = GameStatus::Title;

	// イベント処理
	void processEvents();

	// ゲームの更新
	void update(float dt);

	// 描画処理
	void render();

	// 終了処理
	void cleanup();

	struct SDLWindowDeleter { void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); } };
	struct SDLRendererDeleter { void operator()(SDL_Renderer* r) const { SDL_DestroyRenderer(r); } };
	struct SDLTextureDeleter { void operator()(SDL_Texture* t) const { SDL_DestroyTexture(t); } };
	struct MixerDeleter {
		void operator()(MIX_Mixer* m) const {
			if (m) MIX_DestroyMixer(m);
		}
	};

	bool m_isRunning{ false };

	std::unique_ptr<SDL_Window, SDLWindowDeleter> m_window;
	std::unique_ptr<SDL_Renderer, SDLRendererDeleter> m_renderer;
	std::unique_ptr<MIX_Mixer, MixerDeleter> m_mixer;

	std::unique_ptr<Player> m_player;
	std::vector < unique_ptr<Enemy>> m_enemies;
	std::vector<unique_ptr<Bullet>> m_bullets;
	std::vector<unique_ptr<Door>> m_doors;
	std::unique_ptr<Goal> m_goal;

	std::vector<BoxCollider> m_grounds;

	int m_width{ 0 };
	int m_height{ 0 };

	SDL_FRect m_camera{ 0.0f, 0.0f, 800.0f, 600.0f };

	// ステージ全体の広さ
	int m_levelWidth{ 2000 };
	int m_levelHeight{ 600 };

	void loadMap(const string& filename);
	void loadConfig(const string& filename);


	SDL_Texture* m_bulletTexture = nullptr;

	const int TILE_SIZE = 50;

	SDL_Texture* m_titleLogo = nullptr;
	float m_titleTimer = 0.0f;

	SDL_Texture* m_gameOverLogo = nullptr;
	SDL_Texture* m_retryText = nullptr;
	SDL_Texture* m_titleReturnText = nullptr;
	SDL_Texture* m_clearLogo = nullptr;
	SDL_Texture* m_background = nullptr;
	SDL_Texture* m_bossBackground = nullptr;

	void loadTextAssets();
	void renderTitle();
	void renderGameOver();
	void renderClear();
	void renderStage();

	void updateEntities(float dt);     // プレイヤー、敵、弾などの挙動更新
	void checkCollisions();            // あらゆる当たり判定の解決
	void spawnBullets();               // 弾の発射処理
	void updateCamera();               // カメラ位置の計算
	void cleanupEntities();            // 死んだ敵や消えた弾の削除

	void resetGame();				   // リトライ処理

	unique_ptr<ResourceManager> m_resourceManager;
	void playSE(const std::string& path);

	float m_shakeTimer = 0.0f;
	float m_clearTimer = 0.0f;

	int m_currentLevel = 1;
	void transitionToBossRoom();

	float m_fadeAlpha = 0.0f;
	float m_fadeSpeed = 1.5f;
	bool m_isFadingOut = false;
	bool m_isFadingIn = false;

	void updateFade(float dt); 
	void renderFade();
	void onFadeOutComplete();

};

