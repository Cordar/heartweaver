// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <iostream>
#include <string>

int cdebug_count = 0;

#define CDEBUG(text) { std::string filename = std::string(__FILE__); \
	std::cout << filename.substr(filename.find_last_of("/\\") + 1) << "::" << __LINE__ << "::" << __FUNCTION__ << " : [" << cdebug_count++ << "] " << text << std::endl; }
#define CDBG CDEBUG("");

#define BOOL_TO_STRING(b) std::string(b ? "true" : "false")
