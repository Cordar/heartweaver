// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include "../CppCommon.h"
#include "../Basic/Vector2.h"

struct CppNode
{
// ======== ATTRIBUTES ========
	
	int ID = -1;
	std::string name;

	Vector2 pos;
	Vector2 size;

	bool isRerouteNode;

	std::vector<int> leftExecPinIDList;
	std::vector<int> leftNonExecPinIDList;
	std::vector<int> rightExecPinIDList;
	std::vector<int> rightNonExecPinIDList;

	// order : leftExec, leftNonExec, rightExec, rightNonExec
	std::array<std::vector<int>*, 4> pinIDListArray = {0};

// ======== CONSTRUCTOR ========

	CppNode() : CppNode(-1, std::string(), Vector2(), Vector2(), false) {}
	CppNode(int _ID, std::string _name, Vector2 _pos, Vector2 _size, bool _isRerouteNode) :
		ID(_ID), name(_name), pos(_pos), size(_size), isRerouteNode(_isRerouteNode)
	{
		for (auto c : " \n")
			name.erase(std::remove(name.begin(), name.end(), c), name.end());
		if (name.empty())
			name = std::string("emptyName") + std::to_string(ID);
	}

// ======== METHODS ========

	void updatePinIDListArray()
	{
		pinIDListArray = { { &leftExecPinIDList, &leftNonExecPinIDList, &rightExecPinIDList, &rightNonExecPinIDList } };
	}

	std::vector<int>& touchPinIDList(bool bLeft, bool bExec)
	{
		return (bLeft ?
			bExec ? leftExecPinIDList : leftNonExecPinIDList :
			bExec ? rightExecPinIDList : rightNonExecPinIDList);
	}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, CppNode& node)
	{
		is >> node.ID;
		read_string(is, node.name);
		is >> node.pos >> node.size >> node.isRerouteNode;
		read_vector(is, node.leftExecPinIDList);
		read_vector(is, node.leftNonExecPinIDList);
		read_vector(is, node.rightExecPinIDList);
		read_vector(is, node.rightNonExecPinIDList);
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const CppNode& node)
	{
		os << node.ID << " ";
		write_string(os, node.name);
		os << "\n\t" << node.pos << "\n\t" << node.size << "\n\t" << node.isRerouteNode << "\n\t";
		write_vector(os, node.leftExecPinIDList, "\n\t\t") << "\n\t";
		write_vector(os, node.leftNonExecPinIDList, "\n\t\t") << "\n\t";
		write_vector(os, node.rightExecPinIDList, "\n\t\t") << "\n\t";
		write_vector(os, node.rightNonExecPinIDList, "\n\t\t");
		return os;
	}
};