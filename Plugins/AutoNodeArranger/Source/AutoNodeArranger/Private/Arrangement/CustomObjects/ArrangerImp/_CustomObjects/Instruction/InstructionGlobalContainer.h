// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include <map>

#include "CommentInstruction.h"
#include "LinkInstruction.h"
#include "NodeInstruction.h"
#include "RerouteInstruction.h"

struct InstructionGlobalContainer
{
// ======== ATTRIBUTES ========

	std::vector<RerouteInstruction> rerouteInstructionList;
	std::vector<LinkInstruction> linkInstructionList;
	std::vector<CommentInstruction> commentInstructionList;

	std::vector<NodeInstruction> allNodeInstructionList;
	std::vector<NodeInstruction> mergedNodeInstructionList;
public:

// ======== CONSTRUCTOR ========

	InstructionGlobalContainer() {}

// ======== READ/WRITE ========

	friend std::istream& operator>>(std::istream& is, InstructionGlobalContainer& instructionGlobalContainer)
	{
		read_vector(is, instructionGlobalContainer.rerouteInstructionList);
		read_vector(is, instructionGlobalContainer.linkInstructionList);
		read_vector(is, instructionGlobalContainer.commentInstructionList);
		read_vector(is, instructionGlobalContainer.allNodeInstructionList);
		read_vector(is, instructionGlobalContainer.mergedNodeInstructionList);
		return is;
	}

	friend std::ostream& operator<<(std::ostream& os, const InstructionGlobalContainer& instructionGlobalContainer)
	{
		write_vector(os, instructionGlobalContainer.rerouteInstructionList, " ") << "\n";
		write_vector(os, instructionGlobalContainer.linkInstructionList, "\n\t") << "\n";
		write_vector(os, instructionGlobalContainer.commentInstructionList, "\n\t") << "\n";
		write_vector(os, instructionGlobalContainer.allNodeInstructionList, "\n\t") << "\n";
		write_vector(os, instructionGlobalContainer.mergedNodeInstructionList, "\n\t");
		return os;
	}

// ======== METHODS ========

	void computeMergedNodeInstructionList()
	{
		std::map<int, std::vector<const NodeInstruction*>> nodeInstructionListMap; // key is NodeID
		for (const auto& nodeInstruction : allNodeInstructionList)
			nodeInstructionListMap[nodeInstruction.nodeID].push_back(&nodeInstruction);
		for (const auto& nodeInstructionList : nodeInstructionListMap)
		{
			mergedNodeInstructionList.emplace_back(nodeInstructionList.first,
				nodeInstructionList.second.front()->fromPos, nodeInstructionList.second.back()->toPos);
		}
	}

	int getMaxInstructionID()
	{
		return static_cast<int>(allNodeInstructionList.size()) - 1;
	}
};
