// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "../CppCommon.h"
#include "../Basic/Vector2.h"
#include "../Basic/Box.h"

struct CppComment
{
// ======== ATTRIBUTES ========
	
	int ID = -1;
	std::string name;

	Vector2 pos;
	Vector2 size;

	std::vector<int> insideNodeIDList;
	std::vector<int> insideCommentIDList;

// ======== CONSTRUCTOR ========

	CppComment() : CppComment(-1, std::string(), Vector2(), Vector2()) {}
	CppComment(int _ID, std::string _name, Vector2 _pos, Vector2 _size) : ID(_ID), name(_name), pos(_pos), size(_size)
	{
		for (auto c : " \n")
			name.erase(std::remove(name.begin(), name.end(), c), name.end());
		if (name.empty())
			name = std::string("emptyName") + std::to_string(ID);
	}

	// T class must have function :
	// Box getCurrentNodeBox(int nodeID)
	template<typename T, typename U>
	Box computeCurrentCommentBox(const Vector2& commentSpacing, const std::vector<U>& commentList, T* t) const
	{
		if (insideNodeIDList.empty() && insideCommentIDList.empty())
			return Box::pos_size(pos, size);
		Box result = insideNodeIDList.empty() ? commentList[insideCommentIDList[0]].computeCurrentCommentBox(commentSpacing, commentList, t) :
			t->getCurrentNodeBox(insideNodeIDList[0]);
		for (auto insideNodeID : insideNodeIDList)
			result += t->getCurrentNodeBox(insideNodeID);
		for (auto insideCommentID : insideCommentIDList)
			result += commentList[insideCommentID].computeCurrentCommentBox(commentSpacing, commentList, t);
		return result.extendBox(commentSpacing).withCommentHeader();
	}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, CppComment& comment)
	{
		is >> comment.ID;
		read_string(is, comment.name);
		is >> comment.pos >> comment.size;
		read_vector(is, comment.insideNodeIDList),
		read_vector(is, comment.insideCommentIDList);
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const CppComment& comment)
	{
		os << comment.ID  << " ";
		write_string(os, comment.name);
		os << "\n\t" << comment.pos << "\n\t" << comment.size << "\n\t";
		write_vector(os, comment.insideNodeIDList, " ") << "\n\t";
		write_vector(os, comment.insideCommentIDList, " ");
		return os;
	}
};