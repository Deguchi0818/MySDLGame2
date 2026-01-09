#pragma once
#include "Collider.h"
#include <SDL3/SDL.h>

enum class DoorColor { Blue, Red, Green };
enum DoorState{ Closed, Opening, Open, Closing };

class Door
{
public:
    Door(float x, float y, float w, float h, DoorColor color);
	~Door();

	void update(float dt);
    void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) {
        if (m_state == DoorState::Open) return;

        SDL_FRect drawRect = m_collider.rect();
        drawRect.x -= cameraOffset.x;
        drawRect.y -= cameraOffset.y;
        drawRect.h = m_currentHeight;

        // 扉の描画（縦長の矩形になる）
        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255); // 青い扉
        SDL_RenderFillRect(renderer, &drawRect);

        // 枠線を描くとタイルっぽさが消えて「一枚の扉」に見える
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &drawRect);
    }

	void onHit();

	bool isOpen() const { return m_state == DoorState::Open; }
	const BoxCollider& collider() const { return m_collider; }

private:
	BoxCollider m_collider;
	DoorColor m_color;
	DoorState m_state;
    float m_timer = 0.0f;           // 状態遷移用のタイマー
    float m_originalHeight = 0.0f;  // 全閉時の高さ
    float m_currentHeight = 0.0f;   // 現在の描画上の高さ
};

