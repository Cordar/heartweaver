// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/KrakenDynamicObstacle.h"

#include "AI/KrakenNavBox.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UKrakenDynamicObstacle::UKrakenDynamicObstacle()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;

	// SetupAttachment(GetOwner()->GetRootComponent());

	// BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Nav Mesh Collision"));
	// BoxCollision->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	//
	// BoxCollision->SetupAttachment(this);


	// BoxCollision->SetupAttachment(this);
}


// Called when the game starts
void UKrakenDynamicObstacle::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UKrakenDynamicObstacle::OnRegister()
{
	AActor* MyOwner = GetOwner();

	if (BoxCollision == nullptr && MyOwner)
	{
		BoxCollision = NewObject<UBoxComponent>(MyOwner, NAME_None, RF_Transactional | RF_TextExportTransient);
		BoxCollision->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f), false);
		BoxCollision->SetupAttachment(this);
		BoxCollision->SetIsVisualizationComponent(true);
		BoxCollision->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		BoxCollision->bHiddenInGame = true;
		BoxCollision->CastShadow = false;
		BoxCollision->CreationMethod = CreationMethod;
		BoxCollision->RegisterComponentWithWorld(GetWorld());
	}


	Super::OnRegister();
}


// Called every frame
void UKrakenDynamicObstacle::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateBoxCollision();
}


void UKrakenDynamicObstacle::UpdateBoxCollision()
{
	if (GetOwner() && BoxCollision)
	{
		FVector BoxExtent = BoxCollision->GetScaledBoxExtent();

		// TArray<TEnumAsByte<EObjectTypeQuery>> OverlapQuery;
		// // Objectos estáticos
		// OverlapQuery.Add(EObjectTypeQuery::ObjectTypeQuery1);
		// TArray<AActor*> ActorsToIgnore;
		// ActorsToIgnore.Add(GetOwner());
		//
		// // Obtenemos todos los actores que estén dentro de nuestra caja
		// TArray<AActor*> OverlappingActors;
		// UKismetSystemLibrary::ComponentOverlapActors(BoxCollision, GetComponentTransform(), OverlapQuery,
		//                                              AActor::StaticClass(),
		//                                              ActorsToIgnore, OverlappingActors);

		// int numb = OverlappingActors.Num();
		// UE_LOG(LogTemp, Warning, TEXT("Length: %i"), numb);

		// Esto no es lo más eficiente. Debe de ser cambiado a futuro y tener todos los nav mesh registrados en un sitio global que no sea necesario buscar
		TArray<AActor*> NavMeshBoxActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKrakenNavBox::StaticClass(), NavMeshBoxActors);

		TSet<FVector> NewBlockedSet;
		for (int i = 0; i < NavMeshBoxActors.Num(); i++)
		{
			if (AKrakenNavBox* Box = Cast<AKrakenNavBox>(NavMeshBoxActors[i]))
			{
				float GridDistance = Box->GetGridDistance();
				int VoxelAmmount = (BoxExtent.X + GridDistance) * (BoxExtent.Y + GridDistance) * (
					BoxExtent.Z + GridDistance) / (GridDistance * 10000);

				NewBlockedSet.Reserve(VoxelAmmount);

				TSet<FVector>* GridMap = Box->GetGridMap();
				TSet<FVector>* BlockedGridMap = Box->GetBlockedGridMap();

				TSet<FVector> PreviousBlockedSet;
				if (PreviousBlockedSetTMap.Contains(Box))
				{
					PreviousBlockedSet = PreviousBlockedSetTMap[Box];
				}

				for (const FVector& Voxel : *GridMap)
				{
					if (IsPointInsideBox(Voxel))
					{
						NewBlockedSet.Add(Voxel);
					}
					else if (PreviousBlockedSet.Contains(Voxel) && BlockedGridMap->Contains(Voxel))
					{
						BlockedGridMap->Remove(Voxel);
					}
				}

				for (FVector Voxel : NewBlockedSet)
				{
					if (!BlockedGridMap->Contains(Voxel))
					{
						BlockedGridMap->Add(Voxel);
					}
				}

				// Nos lo guardamos para poder recorrerlo después
				if (PreviousBlockedSetTMap.Contains(Box))
				{
					PreviousBlockedSetTMap[Box] = NewBlockedSet;
				}
				else
				{
					PreviousBlockedSetTMap.Add(Box, NewBlockedSet);
				}

				// for (FVector Pos : NewBlockedSet)
				// {
				// 	DrawDebugSphere(GetWorld(), Pos, 5.0f, 3, FColor::Red, false, 0.5f);
				// }

				
			}
		}


		DrawDebugBox(GetWorld(), GetComponentLocation(), BoxCollision->GetScaledBoxExtent(),GetComponentRotation().Quaternion(),
		             FColor::Green, false, 0.5f, 0, 2.0f);
	}
}

bool UKrakenDynamicObstacle::IsPointInsideBox(const FVector& Point) const
{
	if (!BoxCollision) return false;

	FTransform BoxTransform = BoxCollision->GetComponentTransform();
	FVector LocalPoint = BoxTransform.InverseTransformPosition(Point);

	FVector BoxExtent = BoxCollision->GetUnscaledBoxExtent();

	return FMath::Abs(LocalPoint.X) <= BoxExtent.X &&
		FMath::Abs(LocalPoint.Y) <= BoxExtent.Y &&
		FMath::Abs(LocalPoint.Z) <= BoxExtent.Z;
}

void UKrakenDynamicObstacle::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	UKrakenDynamicObstacle* This = CastChecked<UKrakenDynamicObstacle>(InThis);
	Collector.AddReferencedObject(This->BoxCollision);

	Super::AddReferencedObjects(InThis, Collector);
}
