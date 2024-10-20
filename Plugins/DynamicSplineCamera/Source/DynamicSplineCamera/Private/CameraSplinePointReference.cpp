// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraSplinePointReference.h"

#include "CameraSpline.h"
#include "Components/BillboardComponent.h"

// Sets default values
ACameraSplinePointReference::ACameraSplinePointReference()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

#if WITH_EDITORONLY_DATA
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetupAttachment(Root);
#else

#endif
}

#if WITH_EDITOR

void ACameraSplinePointReference::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	if (CameraSpline)
	{
		CameraSpline->UpdateReferencePointsData();
	}
}

void ACameraSplinePointReference::Destroyed()
{
	Super::Destroyed();

	if (CameraSpline)
	{
		CameraSpline->UpdateReferencePointsData();
	}
}


void ACameraSplinePointReference::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	if (CameraSpline && !GIsPlayInEditorWorld)
	{
		CameraSpline->ReestructurateArrayFromDuplicatedReferencePointActor(this);
	}
}
#endif
