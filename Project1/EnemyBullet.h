#pragma once
#include "Bullet.h"
class EnemyBullet : public Bullet
{
public:
    EnemyBullet(SDL_Renderer* renderer, float x, float y,
        SDL_Texture* texture, float vx, float vy)
        : Bullet(renderer, x, y, 12.0f, 12.0f, texture, vx, vy)
    {
        bulletSpeed = 300.0f; // “G‚Ì’e‚Ì‘¬“x’²®
    }
};