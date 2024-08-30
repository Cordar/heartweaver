// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "Vector2.h"

#define COMMENT_HEADER Vector2(0., 30.)

class Box
{
// ======== CONSTRUCTOR ========

public:
	static Box pos_size(Vector2 pos, Vector2 size)
	{
		return Box(pos, pos + size);
	}

private:
	Box() : Box(Vector2(), Vector2()) {}
	Box(Vector2 _Min, Vector2 _Max) : Min(_Min), Max(_Max) {}

// ======== GETTERS ========

public:
	const Vector2& getMin() const { return Min; }
	const Vector2& getMax() const { return Max; }

	Vector2 getSize() const { return Max - Min; }

// ======== METHODS ========

	bool IntersectWith(const Box& rhs) const
	{
		if (Min.x > rhs.Max.x || rhs.Min.x > Max.x)
			return false;
		if (Min.y > rhs.Max.y || rhs.Min.y > Max.y)
			return false;
		return true;
	}

	Box extendBox(Vector2 extend) const
	{
		return Box(Min - extend, Max + extend);
	}

	Box withCommentHeader() const
	{
		return Box(Min - COMMENT_HEADER, Max);
	}

	Box offsetBox(Vector2 offset) const
	{
		return Box(Min + offset, Max + offset);
	}

	// used by connectedGraph
	Box ComputeBoxToNotOverlapInY(int commentHeaderCount, int commentDiffCount,
		const Vector2& spacing, const Vector2& commentSpacing) const
	{
		Box result = *this;
		result.Max += COMMENT_HEADER * commentHeaderCount;
		return result.extendBox(spacing + commentSpacing * commentDiffCount);
	}

	// used by connectedGraph
	Box ComputeBoxToNotOverlapInX(int commentHeaderCount, int commentDiffCount,
		const Vector2& spacing, const Vector2& commentSpacing) const
	{
		Box result = *this;
		result.Min += COMMENT_HEADER * commentHeaderCount;
		return result.extendBox(spacing + commentSpacing * commentDiffCount);
	}

	std::string boxToString() const
	{
		return "(" + Min.vector2ToString() + " : " + Max.vector2ToString() + ")";
	}

// ======== OPERATORS ========

	Box& operator+=(const Box& rhs)
	{
		Min = Min.minWith(rhs.Min);
		Max = Max.maxWith(rhs.Max);
		return *this;
	}

// ======== ATTRIBUTES ========

private:
	Vector2 Min;
	Vector2 Max;
};
