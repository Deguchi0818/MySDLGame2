#pragma once
#include <SDL3/SDL.h>

class BoxCollider
{
public:
	BoxCollider() = default;

	BoxCollider(float x, float y, float w, float h)
		: m_rect{ x, y, w, h } 
	{
	}

	const SDL_FRect& rect() const { return m_rect; }

private:
	SDL_FRect m_rect{ 0.0f, 0.0f, 0.0f, 0.0f };
};

