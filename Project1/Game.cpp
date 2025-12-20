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

	m_camera.w = static_cast<float>(m_width);
	m_camera.h = static_cast<float>(m_height);

	// playerの作成
	m_player = make_unique <Player>(
		m_renderer,
		0.0f, 0.0f,		// 位置
		64.0f, 64.0f,	// 表示サイズ
		"assets/player.png"
	);

	m_grounds.emplace_back(0, 550, m_levelWidth, 50);
	m_grounds.emplace_back(0, 350, 50, 300);
	m_grounds.emplace_back(300, 250, 50, 200);

	m_isRunning = true;

	loadConfig("PlayerParams.csv");
	loadMap("map.txt");

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

			else if(event.key.key == SDLK_R)
				loadConfig("PlayerParams.csv");
		}
	}
}

void Game::update(float dt) 
{
	m_player->update(dt, m_levelWidth, m_levelHeight);

	// プレイヤーの中心座標を計算
	SDL_FRect pRect = m_player->collider().rect();
	float playerCenterX = pRect.x + pRect.w / 2.0f;
	float playerCenterY = pRect.y + pRect.h / 2.0f;
	// カメラをプレイヤーの中心に合わせる
	m_camera.x = playerCenterX - m_camera.w / 2.0f;
	m_camera.y = playerCenterY - m_camera.h / 2.0f;
	// カメラの範囲制限
	if (m_camera.x < 0)
	{
		m_camera.x = 0;
	}
	if (m_camera.y < 0)
	{
		m_camera.y = 0;
	}
	if (m_camera.x > m_levelWidth - m_camera.w) m_camera.x = m_levelWidth - m_camera.w;
	if (m_camera.y > m_levelHeight - m_camera.h) m_camera.y = m_levelHeight - m_camera.h;

	BoxCollider& pCol = m_player->collider();
	SDL_FRect p = pCol.rect();

	for (auto& ground : m_grounds) 
	{
		if (!pCol.intersect(ground)) 
		{
			continue;
		}

		const SDL_FRect& g = ground.rect();
		float overlapLeft = (p.x + p.w) - g.x;		// プレイヤー右側が地面の左側を越えた量
		float overlapRight = (g.x + g.w) - p.x;		// 地面の右側がプレイヤー左側を越えた量
		float penX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;

		float overlapTop = (p.y + p.h) - g.y;		// プレイヤー下側が地面の上側を越えた量
		float overlapBottom = (g.y + g.h) - p.y;	// 地面の下側がプレイヤー上側を越えた量
		float penY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

		if (penX < penY)
		{
			// X軸方向の貫通が小さい -> X軸方向に修正
			if (overlapLeft < overlapRight)
			{
				// 左側から衝突
				pCol.setPosition(g.x - p.w, p.y);
			}
			else
			{
				// 右側から衝突
				pCol.setPosition(g.x + g.w, p.y);
			}
			m_player->velX = 0.0f;
		}
		else
		{
			// Y軸方向の貫通が小さい -> Y軸方向に修正
			if (overlapTop < overlapBottom)
			{
				// 上側から衝突
				pCol.setPosition(p.x, g.y - p.h);
				m_player->velY = 0.0f;
				m_player->setOnGround(true);
			}
			else
			{
				// 下側から衝突
				pCol.setPosition(p.x, g.y + g.h);
				m_player->velY = 0.0f;
			}
		}
		p = pCol.rect();
	}
}

void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 64, 255);
	SDL_RenderClear(m_renderer);


	m_player->render(m_renderer, { m_camera.x, m_camera.y });

	SDL_SetRenderDrawColor(m_renderer, 100, 50, 0, 255);
	for (auto& g : m_grounds) 
	{
		SDL_FRect r = g.rect();
		r.x -= m_camera.x;
		r.y -= m_camera.y;


		SDL_RenderFillRect(m_renderer, &r);
	}


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

void Game::loadMap(const string& filename) 
{
	m_grounds.clear();

	ifstream file(filename);
	if (!file.is_open()) 
	{
		return;
	}

	string line;
	int row = 0;


	while (getline(file, line))
	{
		for (int col = 0; col < line.length(); ++col)
		{
			char tile = line[col];

			float x = static_cast<float>(col * TILE_SIZE);
			float y = static_cast<float>(row * TILE_SIZE);

			if (tile == '#')
			{
				m_grounds.emplace_back(x, y, (float)TILE_SIZE, (float)TILE_SIZE);
			}
			else if (tile == 'P')
			{
				if (m_player)
				{
					m_player->resetPosition(x, y);
				}
			}
		}

		int lineWidth = line.length() * TILE_SIZE;
		if (lineWidth > m_levelWidth)
		{
			m_levelWidth = lineWidth;
		}

		row++;
	}

	m_levelHeight = row * TILE_SIZE;

	file.close();
}

void Game::loadConfig(const string& filename) 
{
	PlayerParams params;

	ifstream file(filename);
	string line;

	while (getline(file, line))
	{
		size_t commaPos = line.find(',');
		if (commaPos == string::npos) continue;

		string name = line.substr(0, commaPos);
		string valueStr = line.substr(commaPos + 1);

		try
		{
			float value = stof(valueStr);

			if (name == "speed") params.speed = value;
			else if (name == "gravity") params.gravity = value;
			else if (name == "jumpPower") params.jumpPower = value;
			else if (name == "fallMultiplier") params.fallMultiplier = value;
			else if (name == "hoverFlapSpeed") params.hoverFlapSpeed = value;
			else if (name == "hoverGravity") params.hoverGravity = value;
			else if (name == "hoverFallMaxSpeed") params.hoverFallMaxSpeed = value;
			else if (name == "coyoteTimeMax") params.coyoteTimeMax = value;
			else if (name == "jumpBufferMax") params.jumpBufferMax = value;
		}
		catch (...) {
			// 数字に変換できない行（ヘッダーや空行）は無視する
			continue;
		}
	}

	if (m_player)
	{
		m_player->applyParams(params);
	}
}