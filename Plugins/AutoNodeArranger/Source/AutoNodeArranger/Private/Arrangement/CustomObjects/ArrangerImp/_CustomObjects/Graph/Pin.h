// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "../CppCommon.h"
#include "../Basic/Vector2.h"

struct CppPin
{
// ======== ATTRIBUTES ========

	int ID = -1;
	std::string name;

	Vector2 offset;
	std::vector<int> connectedToPinIDList;

// ======== CONSTRUCTOR ========

	CppPin() : CppPin(-1, std::string(), Vector2()) {}
	CppPin(int _ID, std::string _name, Vector2 _offset) : ID(_ID), name(_name), offset(_offset)
	{
		for (auto c : " \n")
			name.erase(std::remove(name.begin(), name.end(), c), name.end());
		if (name.empty())
			name = std::string("emptyName") + std::to_string(ID);
	}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, CppPin& pin)
	{
		is >> pin.ID;
		read_string(is, pin.name);
		is >> pin.offset;
		read_vector(is, pin.connectedToPinIDList);
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const CppPin& pin)
	{
		os << pin.ID << " ";
		write_string(os, pin.name);
		os << " " << pin.offset << " ";
		write_vector(os, pin.connectedToPinIDList, " ");
		return os;
	}
};
