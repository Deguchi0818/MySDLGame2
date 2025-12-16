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

	bool intersect(const BoxCollider& other) const 
	{
		const SDL_FRect& a = m_rect;
		const SDL_FRect& b = other.m_rect;

		// a‚Ì‰E‘¤‚ªb‚Ì¶‘¤‚æ‚è‰E‚É‚ ‚é‚©‚Âa‚Ì¶‘¤‚ªb‚Ì‰E‘¤‚æ‚è¶‚É‚ ‚é
		bool overlapX = (a.x < b.x + b.w) && (a.x + a.w > b.x);
		// a‚Ì‰º‘¤‚ªb‚Ìã‘¤‚æ‚è‚à‰º‚É‚ ‚é‚©‚Âa‚Ìã‘¤‚ª‚‚‚Ì‰º‘¤‚æ‚èã‚É‚ ‚é
		bool overlapY = (a.y < b.y + b.h) && (a.y + a.h > b.y);


		return overlapX && overlapY;
	}

private:
	SDL_FRect m_rect{ 0.0f, 0.0f, 0.0f, 0.0f };

};

