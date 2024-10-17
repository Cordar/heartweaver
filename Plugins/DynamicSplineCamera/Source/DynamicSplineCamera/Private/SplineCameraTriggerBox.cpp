// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineCameraTriggerBox.h"

#include "EngineUtils.h"
#include "SplineCameraActor.h"
#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"

ASplineCameraTriggerBox::ASplineCameraTriggerBox()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASplineCameraTriggerBox::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent = Cast<UBoxComponent>(GetCollisionComponent());

	GetCollisionComponent()->OnComponentBeginOverlap.
	                         AddDynamic(this, &ASplineCameraTriggerBox::OnComponentBeginOverlap);
	GetCollisionComponent()->OnComponentEndOverlap.AddDynamic(this, &ASplineCameraTriggerBox::OnComponentEndOverlap);

	SetActorTickEnabled(false);
}

void ASplineCameraTriggerBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bCheckForReferencePosition && ReferenceActor)
	{
		if (IsPointInsideBox(ReferenceActor->GetActorLocation()) || !bActivateOnlyIfActorCenterIsInsideBox)
		{
			// Activamos la cámara
			UWorld* World = GetWorld();

			if (World)
			{
				for (TActorIterator<AActor> It(World, ASplineCameraActor::StaticClass()); It; ++It)
				{
					AActor* Actor = *It;
					if (ASplineCameraActor* CameraActor = Cast<ASplineCameraActor>(Actor))
					{
						CameraActor->SetActiveSpline(CameraSpline);
						break;
					}
				}
			}

			bCheckForReferencePosition = false;
			SetActorTickEnabled(false);
		}
	}
}

void ASplineCameraTriggerBox::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                      bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlapFilter.Contains(OtherActor->GetClass()))
	{
		// Nos lo guardamos y comprobamos el momento en el que esté el centro dentro
		ReferenceActor = OtherActor;

		bCheckForReferencePosition = true;
		SetActorTickEnabled(true);
	}
}

void ASplineCameraTriggerBox::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == ReferenceActor)
	{
		ReferenceActor = nullptr;
		bCheckForReferencePosition = false;
		SetActorTickEnabled(false);
	}
}

bool ASplineCameraTriggerBox::IsPointInsideBox(const FVector& Point) const
{
	if (BoxComponent)
	{
		FVector LocalPoint = BoxComponent->GetComponentTransform().InverseTransformPosition(Point);

		FVector BoxExtent = BoxComponent->GetUnscaledBoxExtent();
		return FMath::Abs(LocalPoint.X) <= BoxExtent.X &&
			FMath::Abs(LocalPoint.Y) <= BoxExtent.Y &&
			FMath::Abs(LocalPoint.Z) <= BoxExtent.Z;
	}

	return false;
}
