// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <vector>

#include "../CppCommon.h"
#include "GraphConfig.h"

template<typename PinType, typename NodeType, typename CommentType>
struct CppGraph
{
// ======== ATTRIBUTES ========

	CppGraphConfig graphConfig;
	
	std::vector<PinType> pinList;
	std::vector<NodeType> nodeList;
	std::vector<CommentType> commentList;

	std::vector<int> selectedNodeIDList;
	std::vector<int> selectedCommentIDList;

// ======== CONSTRUCTOR ========

	CppGraph() {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, CppGraph& graph)
	{
		is >> graph.graphConfig;
		read_vector(is, graph.pinList);
		read_vector(is, graph.nodeList);
		read_vector(is, graph.commentList);
		read_vector(is, graph.selectedNodeIDList);
		read_vector(is, graph.selectedCommentIDList);
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const CppGraph& graph)
	{
		os << graph.graphConfig << "\n";
		write_vector(os, graph.pinList, "\n\t") << "\n";
		write_vector(os, graph.nodeList, "\n\t") << "\n";
		write_vector(os, graph.commentList, "\n\t") << "\n";
		write_vector(os, graph.selectedNodeIDList, " ") << "\n";
		write_vector(os, graph.selectedCommentIDList, " ");
		return os;
	}
};