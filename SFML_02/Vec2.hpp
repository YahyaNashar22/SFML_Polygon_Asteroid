#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>


template <typename T>
class Vec2
{
public:
	T x =	 0;
	T y =	 0;

	Vec2() = default;

	Vec2(T xin, T yin) : x(xin), y(yin) {}

	// constructor to convert from sf::Vector2
	Vec2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {}

	// allow automatic conversion to sf::Vector2
	// this lets us pass VEc2 into sfml functions
	operator sf::Vector2<T>()
	{
		return sf::Vector2<T>(x, y);
	}

	Vec2 operator + (const Vec2& rhs) const
	{
		// TODO
		return Vec2();
	}

	Vec2 operator - (const Vec2& rhs) const
	{
		// TODO
		return Vec2();
	}

	Vec2 operator * (const T val) const
	{
		// TODO
		return Vec2();
	}

	Vec2 operator / (const T val) const
	{
		// TODO
		return Vec2();
	}

	Vec2 operator == (const Vec2& rhs) const
	{
		// TODO
		return Vec2();
	}

	void operator += (const Vec2& rhs) const
	{
		// TODO
	}

	void operator -= (const Vec2& rhs) const
	{
		// TODO
	}

	void operator *= (const T val) const
	{
		// TODO
	}

	void operator /= (const T val) const
	{
		// TODO
	}

	float dist(const Vec2& rhs) const
	{
		// TODO
	}
};

using Vec2f = Vec2<float>;