#include "Game.h"

#include <SDL3_image/SDL_image.h>

Game::~Game()
{
	cleanup();
}

bool Game::init(const string& title, int width, int height)
{
	m_width = width;
	m_height = height;

	// SDL のビデオサブシステムを初期化
	if (!SDL_Init(SDL_INIT_VIDEO)) 
	{
		return false;
	}

	// ウィンドウの作成
	m_window = SDL_CreateWindow(title.c_str(), m_width, m_height, SDL_WINDOW_RESIZABLE);
	if (!m_window) 
	{
		SDL_Quit();
		return false;
	}

	// レンダラー作成
	m_renderer = SDL_CreateRenderer(m_window, nullptr);
	if (!m_renderer) 
	{
		SDL_DestroyWindow(m_window);
		SDL_Quit();
		return false;
	}

	// playerの作成
	m_player = make_unique <Player>(
		m_renderer,
		0.0f, 0.0f,		// 位置
		64.0f, 64.0f,	// 表示サイズ
		"assets/player.png"
	);

	m_isRunning = true;

	return true;
}

void Game::run() 
{
	static Uint64 prev = SDL_GetTicksNS();

	// メインループ
	while (m_isRunning)
	{
		Uint64 now = SDL_GetTicksNS();
		// デルタタイムの作成
		float dt = (now - prev) / 1'000'000'000.0f;  // ns -> seconds
		prev = now;

		if (dt > 0.1f) dt = 0.1f;  // dt が 0.1秒以上なら固定

		processEvents();
		update(dt);
		render();

		SDL_Delay(1); // 一気にdtが巨大化するのを防ぐ
	}
}

void Game::processEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT) {
			m_isRunning = false;
		}

		else if (event.type == SDL_EVENT_KEY_DOWN) {
			if (event.key.key == SDLK_ESCAPE) {
				m_isRunning = false;
			}
		}
	}
}

void Game::update(float dt) 
{
	m_player->update(dt, m_levelWidth, m_levelHeight);
}

void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 64, 255);
	SDL_RenderClear(m_renderer);

	m_player->render(m_renderer);

	// プレイヤーの中心座標を計算
	SDL_FRect pRect = m_player->collider().rect();
	float playerCenterX = pRect.x + pRect.w / 2.0f;
	float playerCenterY = pRect.y + pRect.h / 2.0f;

	// 画面に反映
	SDL_RenderPresent(m_renderer);
}

void Game::cleanup()
{
	if (m_renderer) {
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}
	if (m_window) {
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}


	if (SDL_WasInit(SDL_INIT_VIDEO)) {
		SDL_Quit();
	}

}
