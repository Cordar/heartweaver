// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <iostream>
#include <vector>

#include "../CppCommon.h"

struct CommentInstruction
{
// ======== ATTRIBUTES ========

	int commentID = -1;

	std::vector<int> nodeIDUnderCommentList;
	std::vector<int> commentIDUnderCommentList;

// ======== CONSTRUCTOR ========

	CommentInstruction() : CommentInstruction(-1) {}
	explicit CommentInstruction(int _commentID) : commentID(_commentID) {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, CommentInstruction& commentInstruction)
	{
		return read_vector(
					read_vector(is >> commentInstruction.commentID,
					commentInstruction.nodeIDUnderCommentList),
				commentInstruction.commentIDUnderCommentList);
	}

	friend std::ostream& operator<<(std::ostream& os, const CommentInstruction& commentInstruction)
	{
		return write_vector(
					write_vector(os << commentInstruction.commentID << "\n\t",
					commentInstruction.nodeIDUnderCommentList, " ") << "\n\t",
				commentInstruction.commentIDUnderCommentList, " ");
	}
};
