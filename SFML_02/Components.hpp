#pragma once

#include <SFML/Graphics.hpp>

#include "Vec2.hpp"

class Component
{
       public:
	bool exists = false;
};

class CTransform : public Component
{
       public:
	Vec2f pos      = {0.0f, 0.0f};
	Vec2f velocity = {0.0f, 0.0f};
	float angle    = 0.0f;

	CTransform() = default;
	CTransform(const Vec2f& p, const Vec2f& v, float a)
	    : pos(p), velocity(v), angle(a)
	{
	}
};

class CShape : public Component
{
       public:
	sf::CircleShape circle;

	CShape() = default;
	CShape(float radius, size_t points, const sf::Color& fill,
	       const sf::Color& outline, int thickness)
	    : circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(Vec2f(radius, radius));
	}
};

class CCollision : public Component
{
       public:
	float radius = 0;

	CCollision() = default;
	CCollision(float r) : radius(r) {}
};

class CScore : public Component
{
       public:
	int score = 0;

	CScore() = default;
	CScore(int s) : score(s) {}
};

class CLifeSpan : public Component
{
       public:
	int lifespan  = 0;
	int remaining = 0;
	CLifeSpan()   = default;
	CLifeSpan(int totalLifespan)
	    : lifespan(totalLifespan), remaining(totalLifespan)
	{
	}
};

class CInput : public Component
{
       public:
	bool up	     = false;
	bool left    = false;
	bool down    = false;
	bool right   = false;
	bool shoot   = false;
	bool special = false;

	CInput() = default;
};

class CSpecialPower : public Component
{
       public:
	int   cooldown	= 180;
	int   remaining = 0;
	float range	= 900.0f;
	float width	= 18.0f;
	int   visualFrames = 0;

	Vec2f target = {0.0f, 0.0f};

	CSpecialPower() = default;
	CSpecialPower(int cd, float r, float w)
	    : cooldown(cd), range(r), width(w)
	{
	}

	bool ready() const { return remaining <= 0; }
};
