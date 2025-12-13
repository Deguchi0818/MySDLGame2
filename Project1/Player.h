#pragma once
#include "Collider.h"

#include <SDL3/SDL.h>
class Player
{
public:
	Player(SDL_Renderer* rendere,
		float x, float y, float w, float h,
		const char* texturtPath);

	~Player();

	void update(float dt, int m_width, int m_height);
	void render(SDL_Renderer* renderer);

	BoxCollider& collider() { return m_collider; }
	const BoxCollider& collider() const { return m_collider; }

private:
	BoxCollider m_collider;

	SDL_Texture* m_texture = nullptr;
};

