// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <algorithm>
#include <set>

template<typename U, typename T>
bool contains(const U& tList, const T& toFindT)
{
	return std::find(tList.begin(), tList.end(), toFindT) != tList.end();
}

template<typename U, typename T>
bool contains_if(const U& tList, const T& tFinder)
{
	return std::find_if(tList.begin(), tList.end(), tFinder) != tList.end();
}

template<typename T>
std::set<T> difference(const std::set<T>& lhsSet, const std::set<T>& rhsSet)
{
	std::set<T> result;
	std::set_difference(lhsSet.begin(), lhsSet.end(), rhsSet.begin(), rhsSet.end(), std::inserter(result, result.begin()));
	return result;
}

template<typename T>
std::set<T> intersection(const std::set<T>& lhsSet, const std::set<T>& rhsSet)
{
	std::set<T> result;
	std::set_intersection(lhsSet.begin(), lhsSet.end(), rhsSet.begin(), rhsSet.end(), std::inserter(result, result.begin()));
	return result;
}