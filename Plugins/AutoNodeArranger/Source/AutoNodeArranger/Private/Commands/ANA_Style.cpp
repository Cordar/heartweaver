// Copyright 2022 bstt, Inc. All Rights Reserved.


#include "ANA_Style.h"
#include "Styling/SlateStyleRegistry.h"

ANA_Style::ANA_Style() : FSlateStyleSet("ANA_Style")
{
	FSlateStyleRegistry::RegisterSlateStyle(*this);
}

ANA_Style::~ANA_Style()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*this);
}

ANA_Style& ANA_Style::Get()
{
	static ANA_Style Inst;
	return Inst;
}