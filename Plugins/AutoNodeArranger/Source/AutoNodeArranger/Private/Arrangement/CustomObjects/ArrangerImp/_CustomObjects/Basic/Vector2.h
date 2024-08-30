// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <algorithm>
#include <iostream>
#include <string>

struct Vector2
{

// ======== ATTRIBUTES ========

	double x = 0.;
	double y = 0.;

// ======== CONSTRUCTOR ========

	Vector2() : Vector2(0., 0.) {}
	Vector2(double _x, double _y) : x(_x), y(_y) {}

	template<typename T>
	Vector2(T t) : Vector2(t.X, t.Y) {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, Vector2& vector2)
	{
		return is >> vector2.x >> vector2.y;
	}

	friend std::ostream& operator<<(std::ostream& os, const Vector2& vector2)
	{
		return os << vector2.x << " " << vector2.y;
	}

// ======== METHODS ========

	Vector2 minWith(const Vector2& rhs) const
	{
		return Vector2(std::min(x, rhs.x), std::min(y, rhs.y));
	}

	Vector2 maxWith(const Vector2& rhs) const
	{
		return Vector2(std::max(x, rhs.x), std::max(y, rhs.y));
	}

	std::string vector2ToString() const
	{
		return std::to_string(x) + ", " + std::to_string(y);
	}

// ======== OPERATORS ========

	Vector2 operator+(const Vector2& rhs) const
	{
		return Vector2(x + rhs.x, y + rhs.y);
	}

	Vector2 operator-(const Vector2& rhs) const
	{
		return Vector2(x - rhs.x, y - rhs.y);
	}

	Vector2 operator*(double f) const
	{
		return Vector2(x * f, y * f);
	}

	Vector2 operator/(double f) const
	{
		return *this * (1. / f);
	}

	bool operator==(const Vector2& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	bool operator!=(const Vector2& rhs) const
	{
		return ! (*this == rhs);
	}

	bool operator<(const Vector2& rhs) const
	{
		return x != rhs.x ? x < rhs.x : y < rhs.y;
	}

	Vector2 operator-() const
	{
		return Vector2() - *this;
	}

	Vector2 operator+=(const Vector2& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
};

