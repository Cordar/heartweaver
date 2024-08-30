// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <iostream>
#include <vector>

template<typename T>
std::istream& read_vector(std::istream& is, std::vector<T>& tList)
{
	size_t vector_size;
	is >> vector_size;
	tList.reserve(vector_size + vector_size);
	tList.resize(vector_size);
	for (auto& t : tList)
		is >> t;
	return is;
}

template<typename T>
std::ostream& write_vector(std::ostream& os, const std::vector<T>& tList, const char* spacing)
{
	os << tList.size();
	for (auto& t : tList)
		os << spacing << t;
	return os;
}

inline std::istream& read_string(std::istream& is, std::string& s)
{
	size_t string_size;
	is >> string_size;
	s.resize(string_size);
	char tmp_c; // read space
	is.get(tmp_c);
	for (auto& c : s)
		is.get(c);
	return is;
}

inline std::ostream& write_string(std::ostream& os, const std::string& s)
{
	return os << s.size() << " " << s.c_str();
}

template<typename T>
std::istream& read_enum(std::istream& is, T& e)
{
	int i;
	is >> i;
	e = static_cast<T>(i);
	return is;
}

template<typename T>
std::ostream& write_enum(std::ostream& os, const T& e)
{
	return os << static_cast<int>(e);
}
