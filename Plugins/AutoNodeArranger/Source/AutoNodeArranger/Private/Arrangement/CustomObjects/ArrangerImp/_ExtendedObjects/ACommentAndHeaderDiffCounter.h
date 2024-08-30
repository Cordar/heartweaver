// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <map>
#include <set>
#include "ExtendedCommon.h"

struct ACommentAndHeaderDiffCounter
{
public:
	virtual const std::set<const ACommentAndHeaderDiffCounter*> computeContainedByCommentSet() const = 0;
	virtual ~ACommentAndHeaderDiffCounter() {}

	int getOrComputeCommentDiffCount(ACommentAndHeaderDiffCounter* other)
	{
		auto it = commentDiffCountMap.find(other);
		if (it != commentDiffCountMap.end())
			return it->second;
		auto containedByCommentSet = computeContainedByCommentSet();
		auto otherContainedByCommentSet = other->computeContainedByCommentSet();
		auto intersect = intersection(containedByCommentSet, otherContainedByCommentSet);
		int result = containedByCommentSet.size() + otherContainedByCommentSet.size() - 2 * intersect.size();
		commentDiffCountMap[other] = result;
		return result;
	}

	int getOrComputeHeaderCount(const ACommentAndHeaderDiffCounter* other)
	{
		auto it = commentHeaderCountMap.find(other);
		if (it != commentHeaderCountMap.end())
			return it->second;
		auto diff = difference(computeContainedByCommentSet(), other->computeContainedByCommentSet());
		int result = diff.size();
		commentHeaderCountMap[other] = result;
		return result;
	}

	Box extendBoxWithComment(const Box& boxToExtend, ACommentAndHeaderDiffCounter* other, Vector2 commentSpacing)
	{
		int nodeCommentHeaderCount = getOrComputeHeaderCount(other);
		return boxToExtend.extendBox((COMMENT_HEADER + commentSpacing) * nodeCommentHeaderCount * 0.5)
					.offsetBox(-COMMENT_HEADER * nodeCommentHeaderCount);
	}

private:
	std::map<const ACommentAndHeaderDiffCounter*, int> commentDiffCountMap;
	std::map<const ACommentAndHeaderDiffCounter*, int> commentHeaderCountMap;
};
