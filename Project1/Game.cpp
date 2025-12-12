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
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return false;
    }
    std::cout << "SDL_Init succeeded!\n";

    // ウィンドウ作成
	  m_window = SDL_CreateWindow(title.c_str(), m_width, m_height, SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return false;
    }

    // レンダラー作成（GPUアクセラレーション）
    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(m_window);
        SDL_Quit();
        return false;
    }

    m_isRunning = true;

	m_camera.w = static_cast<float>(width);
    m_camera.h = static_cast<float>(height);
    m_levelWidth = 2000;


    // プレイヤー生成
    m_player = make_unique <Player>(
        m_renderer,
        0.0f, 0.0f,   // 位置
        50.0f, 50.0f,     // 表示サイズ（画像に合わせて調整）
        "assets/player.png"
    );

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
    if (!m_player) {
        return;
    }

    m_player->setOnGround(false);

    m_player->update(dt, m_levelWidth, m_levelHeight);

    // プレイヤーの中心座標を計算
	SDL_FRect pRect = m_player->collider().rect();
	float playerCenterX = pRect.x + pRect.w / 2.0f;
	float playerCenterY = pRect.y + pRect.h / 2.0f;

	// カメラをプレイヤーの中心に合わせる
    m_camera.x = playerCenterX - m_width / 2;
    m_camera.y = playerCenterY - m_height / 2;

    // カメラがステージ外側を映さないように制限
    if (m_camera.x < 0) m_camera.x = 0;
    if (m_camera.y < 0) m_camera.y = 0;
    if (m_camera.x > m_levelWidth - m_camera.w) m_camera.x = m_levelWidth - m_camera.w;
    if (m_camera.y > m_levelHeight - m_camera.h) m_camera.y = m_levelHeight - m_camera.h;

    BoxCollider& pCol = m_player->collider();
    SDL_FRect p = pCol.rect();

	if (p.y > m_levelHeight + 100.0f)
	{
		m_player->resetPosition(100.0f, 300.0f);
	}

    // すべての床と当たり判定
    for (auto& ground : m_grounds)
    {
        if (!pCol.intersects(ground)) continue;

        const SDL_FRect& g = ground.rect();

        // X方向の重なり量（右と左どちらの侵入が小さいか）
        float overlapLeft = (p.x + p.w) - g.x;            // プレイヤー右側が地面の左側を越えた量
        float overlapRight = (g.x + g.w) - p.x;            // 地面の右側がプレイヤー左側を越えた量
        float penX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;

        // Y方向の重なり量（下と上どちらの侵入が小さいか）
        float overlapTop = (p.y + p.h) - g.y;           // プレイヤー下側が地面の上側を越えた量
        float overlapBottom = (g.y + g.h) - p.y;           // 地面下側がプレイヤー上側を越えた量
        float penY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

        const SDL_FRect& prev = m_player->getPrevRect();
        float prevBottom = prev.y + prev.h;
        float prevTop = prev.y;
        float prevRight = prev.x + prev.w;
        float prevLeft = prev.x;

        if (prevBottom <= g.y + 0.001f && m_player->velY >= 0) 
        {
            m_player->snapToGround(g.y);
        }
        else if (prevTop >= g.y + g.h - 0.001f && m_player->velY < 0) 
        {
            float newY = g.y + g.h;
            pCol.setPosition(p.x, newY);
            m_player->velY = 0;
        }
        else
        {
            if (overlapLeft < overlapRight) 
            {
                float newX = g.x - p.w;
                pCol.setPosition(newX, p.y);
            }
            else
            {
                float newX = g.x + g.w;
                pCol.setPosition(newX, p.y);
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

    // 床（茶色っぽい色）
    SDL_SetRenderDrawColor(m_renderer, 100, 50, 0, 255);
    for (auto& g : m_grounds) {
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

    if (m_player) {
        m_player = nullptr;

        m_isRunning = false;
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