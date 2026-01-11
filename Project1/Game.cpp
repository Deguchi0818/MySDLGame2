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

	m_isRunning = true;

	m_bulletTexture = IMG_LoadTexture(m_renderer, "assets/bullet.png");

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

	for (auto& door : m_doors) {
		// 1. ドアの更新（アニメーションなど）
		door->update(dt);

		// 2. プレイヤーとの当たり判定
		if (m_player->collider().intersect(door->collider())) {

			// 3. もしドアが完全に開いていなければ、壁として押し戻す
			if (!door->isOpen()) {
				float vx = m_player->velX;
				float vy = m_player->velY;

				// 既存の衝突解決メソッドを利用して、ドアを壁として扱う
				vector<BoxCollider> tempDoorVec = { door->collider() };
				BoxCollider::resolveCollision(m_player->collider(), vx, vy, tempDoorVec);

				m_player->velX = vx;
				m_player->velY = vy;
			}
		}
	}

	// プレイヤーの中心座標を計算
	SDL_FRect pRect = m_player->collider().rect();

	bool onGround = BoxCollider::resolveCollision(m_player->collider(), m_player->velX, m_player->velY, m_grounds);
	m_player->setOnGround(onGround);

	for (auto& enemy : m_enemies) {
		enemy->update(dt, m_player->collider().rect(), *m_player, m_grounds);

		

		if (!enemy->isDead()) 
		{
			if (!enemy->isStunned() && m_player->collider().intersect(enemy->collider()))
			{
				float pCenterX = m_player->collider().rect().x + m_player->collider().rect().w / 2;	// playerのｘ軸の中心を求める
				float eCenterX = enemy->collider().rect().x + enemy->collider().rect().w / 2; // enemyのｘ軸の中心を求める

				// 敵からみてplayerがどちらに向いているかの判定
				float direction = (pCenterX < eCenterX) ? -1.0f : 1.0f;

				m_player->applyKnockback(direction * 500.0f, -400.0f);
				enemy->applyKnockback(direction * -500.0f, -400.0f);
			}
		}
	}

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

	float bulletW = 16.0f;
	float bulletH = 16.0f;

	float spawnX = (m_player->m_facingDir > 0) ? (pRect.x + pRect.w) : (pRect.x - bulletW);
	float spawnY = playerCenterY - (bulletH / 2.0f);

	// 弾の発射
	if (m_player->wantsToShoot())
	{
		AimDir dir = m_player->getAimDir();

		m_bullets.push_back(make_unique<Bullet>(
			m_renderer,
			spawnX, spawnY, bulletW, bulletH,
			// 弾を撃つたびに IMG_LoadTextureを読んでいたが起動時にポインタで場所を指定することで一回の読み込みでよくなりかくつきが解消された
			m_bulletTexture,
			dir.vx, dir.vy
		));

		m_player->consumeShootFlag();
	}

	for (auto& bullet : m_bullets) {
		bullet->update(dt, m_grounds);
		// 消えている弾は無視
		if (!bullet->isActive()) continue;

		for (auto& enemy : m_enemies)
		{
			// 既に消えていいる敵を無視
			if (enemy->isDead()) continue;

			if (bullet->collider().intersect(enemy->collider()))
			{
				enemy->takeDamage();
				bullet->deleteBullet();
				break;
			}
		}

		for (auto& door : m_doors) 
		{
			if (!door->isOpen() && bullet->collider().intersect(door->collider())) {
				door->onHit();
				bullet->deleteBullet();
				break;
			}
		}
	}

	erase_if(m_bullets, [](const std::unique_ptr<Bullet>& b) {
		return !b->isActive();
		});

	erase_if(m_enemies, [](const std::unique_ptr<Enemy>& e) {
		return e->isDead();
		});
}

void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 64, 255);
	SDL_RenderClear(m_renderer);


	m_player->render(m_renderer, { m_camera.x, m_camera.y });

	for (auto& bullet : m_bullets) {
		bullet->render(m_renderer, { m_camera.x, m_camera.y });
	}
	for (auto& enemy : m_enemies) {
		enemy->render(m_renderer, { m_camera.x, m_camera.y });
	}


	SDL_SetRenderDrawColor(m_renderer, 100, 50, 0, 255);
	for (auto& g : m_grounds) 
	{
		SDL_FRect r = g.rect();
		r.x -= m_camera.x;
		r.y -= m_camera.y;


		SDL_RenderFillRect(m_renderer, &r);
	}

	SDL_FPoint cameraOffset = { m_camera.x, m_camera.y };
	for (auto& door : m_doors) {
		door->render(m_renderer, cameraOffset);
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
			else if (tile == 'E') {
				m_enemies.push_back(make_unique<EnemyWalking>(x, y, 64.0f, 64.0f));
			}
			else if (tile == 'C') {
				m_enemies.push_back(make_unique<EnemyChase>(x, y, 64.0f, 64.0f));
			}
			else if (tile == 'D') {
				m_doors.push_back(make_unique<Door>(x, y, (float)TILE_SIZE, (float)TILE_SIZE * 2, DoorColor::Blue));
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