// Copyright 2022 bstt, Inc. All Rights Reserved.

#include <fstream>
#include <cstdlib>
#include <ctime>

#include "CDebug.h"

#include "_ExtendedObjects/ExtendedGraph.h"

int main(int argc, char const *argv[])
{
	static const char* import_filename = __FILE__ "/../graph.txt";
	static const char* export_filename = __FILE__ "/../instructions.txt";

	std::ifstream ifs(import_filename);
	if (!ifs.is_open())
	{
		std::cerr << "cannot open " << import_filename << std::endl;
		return 1;
	}
	ExtendedGraph extendedGraph;
	ifs >> extendedGraph;
	ifs.close();

	extendedGraph.doAll();

	std::ofstream ofs(export_filename);
	if (!ofs)
	{
		std::cerr << "cannot open " << export_filename << std::endl;
		return 1;
	}
	ofs << extendedGraph.instructionGlobalContainer;
	ofs.close();
	return 0;
}