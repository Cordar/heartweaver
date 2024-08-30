// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

class UEdGraphNode;

class ANA_RerouteHelper
{
	ANA_RerouteHelper() = delete;

public:
	using IsRerouteFunc = bool(*)(const UEdGraphNode*);

	static IsRerouteFunc GetIsRerouteFunc(bool isMaterial) { return isMaterial ? isMaterialRerouteFunc : isOtherRerouteFunc; }

private:

	static const IsRerouteFunc isMaterialRerouteFunc;
	static const IsRerouteFunc isOtherRerouteFunc;
};
