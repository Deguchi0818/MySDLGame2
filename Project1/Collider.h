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

	void setPosition(float x, float y) 
	{
		m_rect.x = x;
		m_rect.y = y;
	}

	void setSize(float w, float h)
	{
		m_rect.w = w;
		m_rect.h = h;
	}

private:
	SDL_FRect m_rect{ 0.0f, 0.0f, 0.0f, 0.0f };

	float speed;
};

