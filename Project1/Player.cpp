#include "Player.h"
#include <SDL3_image/SDL_image.h>

Player::Player(SDL_Renderer* renderer,
	float x, float y, float w, float h,
	const char* texturePath)
	:m_collider(x, y, w, h) 
{
	m_texture = IMG_LoadTexture(renderer, texturePath);
	if (!m_texture) {
		SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
	}

}
Player::~Player() 
{
	// Player‚ª”j‰ó‚³‚ê‚½Žžtexture‚ð”j‰ó
	if (m_texture)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = nullptr;
	}
}

void Player::update(float dt, int screenW, int screenH) 
{

}

void Player::render(SDL_Renderer* renderer) 
{
	SDL_FRect dst = m_collider.rect();

	if (!m_texture)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(renderer, &dst);
		return;
	}

	SDL_RenderTexture(renderer, m_texture, nullptr, &dst);
}