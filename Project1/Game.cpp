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
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
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

	if (!MIX_Init()) {
		SDL_Log("MIX_Init Failed: %s", SDL_GetError());
		return false;
	}

	m_resourceManager = make_unique<ResourceManager>(m_renderer, m_mixer);

	loadTextAssets();
	m_bulletTexture = m_resourceManager->getTexture("assets/bullet.png");

	// playerの作成
	m_player = make_unique <Player>(
		m_renderer,
		0.0f, 0.0f,		// 位置
		64.0f, 64.0f,	// 表示サイズ
		m_resourceManager->getTexture("assets/player.png")
	);

	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer) return false;

	m_camera.w = static_cast<float>(m_width);
	m_camera.h = static_cast<float>(m_height);

	m_isRunning = true;

	
	loadConfig("PlayerParams.csv");
	loadMap("map.txt");	

	m_resourceManager->getAudio("assets/se_jump.mp3");
	m_resourceManager->getAudio("assets/se_shoot.mp3");
	m_resourceManager->getAudio("assets/se_damage.mp3");

	MIX_SetMixerGain(m_mixer, 0.4f);
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
			// タイトル画面の時
			if (m_status == GameStatus::Title) {
				if (event.key.key == SDLK_SPACE) {
					m_status = GameStatus::Playing; // ゲーム開始！
				}
			}
			else if (m_status == GameStatus::GameOver)
			{
				if (event.key.key == SDLK_R) 
				{
					m_status = GameStatus::Playing;

					resetGame();

				}
				else if (event.key.key == SDLK_T) 
				{
					m_status = GameStatus::Title;

					resetGame();
				}
			}
			// クリア画面の時
			else if (m_status == GameStatus::Clear) {
				if (event.key.key == SDLK_T) {
					m_status = GameStatus::Title;
				}
			}
			else if (event.key.key == SDLK_ESCAPE) {
				m_isRunning = false;
			}

			else if (event.key.key == SDLK_R)
				loadConfig("PlayerParams.csv");
		}
		else if (event.type == SDL_EVENT_KEY_DOWN) {
			
		}
	}
}

void Game::update(float dt)
{
	if (m_status == GameStatus::Title) 
	{
		m_titleTimer += dt;
	}

	if (m_status == GameStatus::Playing || m_status == GameStatus::BossBattle) 
	{
		updatePlaying(dt);
	}
	
	updateCamera();

}

void Game::updatePlaying(float dt)
{
	updateEntities(dt);  // 動かす
	checkCollisions();   // 当たっているか調べる
	spawnBullets();      // 新しく生成する
	cleanupEntities();   // 不要なものを消す
	updateCamera();      // カメラを合わせる

	// ゲームオーバー判定
	if (m_player->getCurrentHp() <= 0) {
		m_status = GameStatus::GameOver;
	}
}

void Game::updateEntities(float dt)
{

	m_player->update(dt, m_levelWidth, m_levelHeight);

	for (auto& enemy : m_enemies)
	{
	
		enemy->update(dt, m_player->collider().rect(), *m_player, m_grounds);
	}

	for (auto& bullet : m_bullets)
	{
		bullet->update(dt, m_grounds);
	}

	for (auto& door : m_doors)
	{
		door->update(dt);
	}
}

void Game::checkCollisions() 
{
	bool onGround = BoxCollider::resolveCollision(m_player->collider(), m_player->velX, m_player->velY, m_grounds);
	m_player->setOnGround(onGround);

	for (auto& door : m_doors)
	{
		// ドアの更新（アニメーションなど）

		// プレイヤーとの当たり判定
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

	for (auto& enemy : m_enemies) 
	{
		enemy->checkPlayerCollision(*m_player);
		// 敵との当たり判定ループ
		if (!enemy->isDead())
		{
			if (!enemy->isStunned() && m_player->collider().intersect(enemy->collider()))
			{
				if (m_player->getInvincibleTimer() <= 0)
				{
					//m_player->takeDamage(10);

					float pCenterX = m_player->collider().rect().x + m_player->collider().rect().w / 2;	// playerのｘ軸の中心を求める
					float eCenterX = enemy->collider().rect().x + enemy->collider().rect().w / 2; // enemyのｘ軸の中心を求める


					// 敵からみてplayerがどちらに向いているかの判定
					float direction = (pCenterX < eCenterX) ? -1.0f : 1.0f;

					enemy->applyKnockback(direction * -500.0f, -400.0f);
					m_player->applyKnockback(direction * 500.0f, -400.0f);


				}

				if (m_player->takeDamage(10))
				{
					playSE("assets/se_damage.mp3");
				}
			}

			for (auto& b : enemy->getBullets())
			{
				if (b->isActive() && b->collider().intersect(m_player->collider()))
				{
					if (m_player->takeDamage(5))
					{
						playSE("assets/se_damage.mp3");
					}
					b->deleteBullet();
				}
			}
		}
	}

	for (auto& enemy : m_enemies)
	{

		enemy->checkPlayerCollision(*m_player);
		// 敵との当たり判定ループ
		if (!enemy->isDead())
		{
			if (!enemy->isStunned() && m_player->collider().intersect(enemy->collider()))
			{
				if (m_player->getInvincibleTimer() <= 0)
				{
					float pCenterX = m_player->collider().rect().x + m_player->collider().rect().w / 2;	// playerのｘ軸の中心を求める
					float eCenterX = enemy->collider().rect().x + enemy->collider().rect().w / 2; // enemyのｘ軸の中心を求める


					// 敵からみてplayerがどちらに向いているかの判定
					float direction = (pCenterX < eCenterX) ? -1.0f : 1.0f;

					enemy->applyKnockback(direction * -500.0f, -400.0f);
					m_player->applyKnockback(direction * 500.0f, -400.0f);


				}

				if (m_player->takeDamage(10))
				{
					playSE("assets/se_damage.mp3");
				}
			}

			for (auto& b : enemy->getBullets())
			{
				if (b->isActive() && b->collider().intersect(m_player->collider()))
				{
					if (m_player->takeDamage(5))
					{
						playSE("assets/se_damage.mp3");
					}
					b->deleteBullet();
				}
			}
		}
	}


}

void Game::spawnBullets() 
{
	// プレイヤーの中心座標を計算
	SDL_FRect pRect = m_player->collider().rect();

	float playerCenterX = pRect.x + pRect.w / 2.0f;
	float playerCenterY = pRect.y + pRect.h / 2.0f;

	float bulletW = 16.0f;
	float bulletH = 16.0f;

	float spawnX = (m_player->m_facingDir > 0) ? (pRect.x + pRect.w) : (pRect.x - bulletW);
	float spawnY = playerCenterY - (bulletH / 2.0f);

	// 弾の発射
	if (m_player->wantsToShoot())
	{
		AimDir dir = m_player->getAimDir();

		playSE("assets/se_shoot.mp3");

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
}

void Game::cleanupEntities() 
{
	erase_if(m_bullets, [](const std::unique_ptr<Bullet>& b) {
		return !b->isActive();
		});

	erase_if(m_enemies, [](const std::unique_ptr<Enemy>& e) {
		return e->isDead();
		});

	if (m_player->isJumpTriggered())
	{
		if (m_player->isJumpTriggered()) {
			playSE("assets/se_jump.mp3");
		}
	}
}

void Game::updateCamera() 
{
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
}


void Game::render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 64, 255);
	SDL_RenderClear(m_renderer);


	if (m_status == GameStatus::Title) {
		// タイトル画面の描画
		SDL_SetRenderDrawColor(m_renderer, 0, 100, 0, 255);
		SDL_RenderClear(m_renderer);
		renderTitle();
	}
	else if (m_status == GameStatus::GameOver) 
	{
		SDL_SetRenderDrawColor(m_renderer, 0, 100, 0, 255);
		SDL_RenderClear(m_renderer);
		renderGameOver();
	}
	else if (m_status == GameStatus::Clear) {
		// クリア画面の描画
		SDL_SetRenderDrawColor(m_renderer, 100, 0, 0, 255);
		SDL_RenderClear(m_renderer);
	}
	else 
	{
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

		if (m_status == GameStatus::Playing || m_status == GameStatus::BossBattle) 
		{
			SDL_FRect bgRect = { 20.0f, 20.0f, 200.0f, 20.0f };
			SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, 255);
			SDL_RenderFillRect(m_renderer, &bgRect);

			float hpRatio = static_cast<float>(m_player->getCurrentHp()) / static_cast<float>(m_player->getMaxHp());
			float barWidth = 200.0f * hpRatio;

			SDL_FRect hpRect = { 20.0f, 20.0f, barWidth, 20.0f };

			if (hpRatio > 0.5f) {
				SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);  // 緑
			}
			else if (hpRatio > 0.2f) {
				SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255); // 黄色
			}
			else {
				SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);   // 赤
			}

			SDL_RenderFillRect(m_renderer, &hpRect);

			SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
			SDL_RenderRect(m_renderer, &bgRect);
		}
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
	m_enemies.clear();
	m_bullets.clear();
	m_doors.clear();

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
				m_enemies.push_back(make_unique<EnemyWalking>(
					x, y, 64.0f, 64.0f,
					m_resourceManager->getTexture("assets/enemy.png")
				));
			}
			else if (tile == 'C') {
				m_enemies.push_back(make_unique<EnemyChase>(
					x, y, 64.0f, 64.0f,
					m_resourceManager->getTexture("assets/enemy.png")
				));
			}
			else if (tile == 'S') {
				auto shooter = make_unique<EnemyShooter>(
					x, y, 64.0f, 64.0f,
					m_resourceManager->getTexture("assets/enemy_shooter.png")
				);
				shooter->setBulletTexture(m_bulletTexture);
				m_enemies.push_back(std::move(shooter));
			}
			else if (tile == 'T') {
				m_enemies.push_back(make_unique<EnemyTimedPatrol>(
					x, y, 64.0f, 64.0f,
					m_resourceManager->getTexture("assets/enemy.png")
				));
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
		if (line.empty()) continue;

		size_t commaPos = line.find(',');
		if (commaPos == string::npos) continue;

		string name = line.substr(0, commaPos);
		string valueStr = line.substr(commaPos + 1);

		auto trim = [](string& s) {
			s.erase(0, s.find_first_not_of(" \t\r\n\xEF\xBB\xBF"));
			s.erase(s.find_last_not_of(" \t\r\n") + 1);
			};
		trim(name);
		trim(valueStr);

		if (name == "パラメータ名" || name == "値" || valueStr.empty()) {
			continue;
		}
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
			else if (name == "m_hp") params.m_hp = static_cast<int>(value);
		}
		catch (const std::exception& e) {
			SDL_Log("Skipping invalid config line: %s (%s)", line.c_str(), e.what());
			continue;
		}
	}

	if (m_player)
	{
		m_player->applyParams(params);
	}
}

void Game::resetGame() 
{
	loadMap("map.txt");

	loadConfig("PlayerParams.csv");
}

void Game::loadTextAssets()
{
	m_titleLogo = m_resourceManager->getTexture("assets/title.png");
	m_gameOverLogo = m_resourceManager->getTexture("assets/gameover.png");
	m_retryText = m_resourceManager->getTexture("assets/retry_text.png");
	m_titleReturnText = m_resourceManager->getTexture("assets/return_title_text.png");
}

void Game::renderTitle() 
{
	SDL_SetRenderDrawColor(m_renderer, 10, 10, 30, 255);
	SDL_RenderClear(m_renderer);

	if (m_titleLogo)
	{
		float texW, texH;
		SDL_GetTextureSize(m_titleLogo, &texW, &texH);

		float scale = 400.0f / texW;
		float drawW = texW * scale;
		float drawH = texH * scale;

		SDL_FRect logoRect = {
			(m_width - drawW) / 2.0f,
			400.0f,
			drawW,
			drawH
		};

		float alpha = (sinf(m_titleTimer * 2.0f) + 1.0f) / 2.0f * 255.0f;

		SDL_SetTextureBlendMode(m_titleLogo, SDL_BLENDMODE_BLEND); // ブレンドモードを有効化
		SDL_SetTextureAlphaMod(m_titleLogo, (Uint8)alpha);

		SDL_RenderTexture(m_renderer, m_titleLogo, nullptr, &logoRect);
	}

}

void Game::renderGameOver()
{
	SDL_SetRenderDrawColor(m_renderer, 10, 10, 30, 255);
	SDL_RenderClear(m_renderer);
	if (m_gameOverLogo)
	{
		float texW, texH;
		SDL_GetTextureSize(m_gameOverLogo, &texW, &texH);

		float scale = 400.0f / texW;
		float drawW = texW * scale;
		float drawH = texH * scale;

		SDL_FRect logoRect = {
			(m_width - drawW) / 2.0f,
			100.0f,
			drawW,
			drawH
		};

		SDL_RenderTexture(m_renderer, m_gameOverLogo, nullptr, &logoRect);

		SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

	}

	if (m_retryText) {
		float texW, texH;
		SDL_GetTextureSize(m_retryText, &texW, &texH);
		SDL_FRect dest = { m_width / 2.0f - texW / 2.0f, m_height / 2.0f + 50, texW, texH };

		SDL_RenderTexture(m_renderer, m_retryText, nullptr, &dest);
	}

	if (m_titleReturnText)
	{
		float texW, texH;
		SDL_GetTextureSize(m_titleReturnText, &texW, &texH);
		SDL_FRect dest = { m_width / 2.0f - texW / 2.0f, m_height / 2.0f + 100, texW, texH };

		SDL_RenderTexture(m_renderer, m_titleReturnText, nullptr, &dest);

	}

}

void Game::playSE(const std::string& path) {
	if (!m_mixer || !m_resourceManager) return;
	MIX_Audio* audio = m_resourceManager->getAudio(path);
	if (audio) {
		MIX_PlayAudio(m_mixer, audio);
	}
}