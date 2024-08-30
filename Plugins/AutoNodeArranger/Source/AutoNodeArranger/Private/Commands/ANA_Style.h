// Copyright 2022 bstt, Inc. All Rights Reserved.

#pragma once

#include "Styling/SlateStyle.h"

/**
 * 
 */
class ANA_Style : public FSlateStyleSet
{
public:
	ANA_Style();
	~ANA_Style();
	static ANA_Style& Get();
};
