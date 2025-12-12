#pragma once

#include<SDL3/SDL.h>
#include<iostream>
#include<string>
#include<vector>
#include<fstream>

#include "Player.h"
#include "Collider.h"


using namespace std;

class Game
{
public:
	Game() = default;
	~Game();

	// SDL の初期化とウィンドウ/レンダラー作成
	bool init(const string& title, int width, int height);

	// メインループ開始
	void run();

private:
	// イベント処理
	void processEvents();

	// ゲームの更新
	void update(float dt);

	// 描画処理
	void render();

	// 終了処理
	void cleanup();

	bool m_isRunning{ false };

	SDL_Window* m_window{ nullptr };
	SDL_Renderer* m_renderer{ nullptr };

	int m_width{ 0 };
	int m_height{ 0 };

	unique_ptr<Player> m_player;

	// 床
	vector<BoxCollider> m_grounds;

	// カメラ（画面の表示範囲を表す矩形）
	SDL_FRect m_camera{ 0, 0, 800, 600 };

	// ステージ全体の広さ
	int m_levelWidth{ 2000 };
	int m_levelHeight{ 600 };

	void loadMap(const string& filename);

	const int TILE_SIZE = 50;
};

