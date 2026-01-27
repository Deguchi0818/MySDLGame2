#pragma once
#include "Collider.h"
#include <SDL3/SDL.h>

class Goal
{
public:
	Goal(float x, float y, float w, float h, SDL_Texture* texture)
		: m_collider(x, y, w, h), m_texture(texture) {}
	~Goal() = default;

	const BoxCollider& collider() const { return m_collider;}

	void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) {
		if (!m_texture) return;

		SDL_FRect drawRect = m_collider.rect();
		drawRect.x -= cameraOffset.x;
		drawRect.y -= cameraOffset.y;

		// ƒS[ƒ‹’n“_‚ğ•`‰æ
		SDL_RenderTexture(renderer, m_texture, nullptr, &drawRect);
	}

private:
	BoxCollider m_collider;
	SDL_Texture* m_texture = nullptr;
};

