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

	// SDLの初期化とウィンドウ/レンダラー作成
	bool init(const string& title, int width, int height);
	
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

	unique_ptr<Player> m_player;

	vector<BoxCollider> m_grounds;

	int m_width{ 800 };
	int m_height{ 600 };

	// ステージ全体の広さ
	int m_levelWidth{ 2000 };
	int m_levelHeight{ 600 };
};

