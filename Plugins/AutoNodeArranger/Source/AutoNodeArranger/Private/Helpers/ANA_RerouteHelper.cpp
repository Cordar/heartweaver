// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "ANA_RerouteHelper.h"
#include "K2Node_Knot.h"
#include "MaterialGraphNode_Knot.h"

const ANA_RerouteHelper::IsRerouteFunc ANA_RerouteHelper::isMaterialRerouteFunc =
	[](const UEdGraphNode* node) { return node->GetClass()->IsChildOf<UMaterialGraphNode_Knot>(); };

const ANA_RerouteHelper::IsRerouteFunc ANA_RerouteHelper::isOtherRerouteFunc =
	[](const UEdGraphNode* node) { return node->GetClass()->IsChildOf<UK2Node_Knot>(); };