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

    void  setPosition(float x, float y) 
    {
        m_rect.x = x;
        m_rect.y = y;
    }

    void setSize(float w, float h) 
    {
        m_rect.w = w;
        m_rect.h = h;
    }

    bool intersects(const BoxCollider& other) const
    {
        const SDL_FRect& a = m_rect;
        const SDL_FRect& b = other.m_rect;

        bool overlapX = (a.x < b.x + b.w) &&
            (a.x + a.w > b.x);
        bool overlapY = (a.y < b.y + b.h) &&
            (a.y + a.h > b.y);

        return overlapX && overlapY;
    }

private:
    SDL_FRect m_rect{ 0.0f, 0.0f, 0.0f, 0.0f };
};

