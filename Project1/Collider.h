#pragma once
#include <SDL3/SDL.h>
#include<iostream>
#include<vector>

using namespace std;

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

		// aの右側がbの左側より右にあるかつaの左側がbの右側より左にある
		bool overlapX = (a.x < b.x + b.w) && (a.x + a.w > b.x);
		// aの下側がbの上側よりも下にあるかつaの上側がｂの下側より上にある
		bool overlapY = (a.y < b.y + b.h) && (a.y + a.h > b.y);


		return overlapX && overlapY;
	}

	static bool resolveCollision(BoxCollider& actor, float& vx, float& vy, const vector<BoxCollider>& grounds)
	{
		SDL_FRect p = actor.rect();
		bool hitGround = false;

		for (auto& ground : grounds)
		{
			if (!actor.intersect(ground))
			{
				continue;
			}

			const SDL_FRect& g = ground.rect();
			float overlapLeft = (p.x + p.w) - g.x;		// プレイヤー右側が地面の左側を越えた量
			float overlapRight = (g.x + g.w) - p.x;		// 地面の右側がプレイヤー左側を越えた量
			float penX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;

			float overlapTop = (p.y + p.h) - g.y;		// プレイヤー下側が地面の上側を越えた量
			float overlapBottom = (g.y + g.h) - p.y;	// 地面の下側がプレイヤー上側を越えた量
			float penY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

			if (penX < penY)
			{
				// X軸方向の貫通が小さい -> X軸方向に修正
				if (overlapLeft < overlapRight)
				{
					// 左側から衝突
					actor.setPosition(g.x - p.w, p.y);
				}
				else
				{
					// 右側から衝突
					actor.setPosition(g.x + g.w, p.y);
				}
				vx = 0.0f;
			}
			else
			{
				// Y軸方向の貫通が小さい -> Y軸方向に修正
				if (overlapTop < overlapBottom)
				{
					// 上側から衝突
					actor.setPosition(p.x, g.y - p.h);
					vy = 0.0f;
					hitGround = true;
				}
				else
				{
					// 下側から衝突
					actor.setPosition(p.x, g.y + g.h);
					vy = 0.0f;
				}
			}
			p = actor.rect();
		}
		return hitGround;
	}




private:
	SDL_FRect m_rect{ 0.0f, 0.0f, 0.0f, 0.0f };

};

