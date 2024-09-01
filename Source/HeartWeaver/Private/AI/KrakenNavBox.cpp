// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/KrakenNavBox.h"

#include "AI/NavMeshVoxelInfo.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/BodySetup.h"

// Sets default values
AKrakenNavBox::AKrakenNavBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(Root);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(Root);

	GridDistance = 100.0f;

	Box->SetWorldScale3D(FVector(1, 1, 1));
	Box->SetRelativeLocation(FVector());
	Box->SetBoxExtent(GetActorScale3D() * 50.0f);
}
#if WITH_EDITOR
void AKrakenNavBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!bCalculatingNavMesh)
	{
		Box->SetWorldScale3D(FVector(1, 1, 1));
		Box->SetRelativeLocation(FVector());
		Box->SetBoxExtent(GetActorScale3D() * 50.0f);

		if (bUpdateNavMesh)
		{
			bUpdateNavMesh = false;
			bCalculatingNavMesh = true;
			GenerateGridInsideBox();
		}
	}
}
#endif


// Called when the game starts or when spawned
void AKrakenNavBox::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AKrakenNavBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKrakenNavBox::GenerateGridInsideBox()
{
	// Tenemos cuidado que no sea un número demasiado pequeño
	if (GridDistance <= 20.0f)
	{
		GridDistance = 20.0f;
	}

	FVector BoxLocalHalfExtent = Box->GetScaledBoxExtent();
	FTransform BoxGlobalTransform = Box->GetComponentTransform();

	// Primero hacemos el cálculo en local así que este valor es 0,0,0
	FVector VoxelizedWorldBoxCenter = FVector();

	FVector InitialPoint = VoxelizedWorldBoxCenter - BoxLocalHalfExtent;
	FVector FinalPoint = VoxelizedWorldBoxCenter + BoxLocalHalfExtent;

	FString InitialPointText = InitialPoint.ToString();
	FString FinalPointText = FinalPoint.ToString();
	UE_LOG(LogTemp, Warning, TEXT("InitialPoint: %s"), *InitialPointText);
	UE_LOG(LogTemp, Warning, TEXT("InitialPoint: %s"), *FinalPointText);

	// La cantidad de puntos que va a tener la caja es igual al volumen dividido por la distancia
	int VoxelAmmount = ((BoxLocalHalfExtent.X * 2 + GridDistance) * (BoxLocalHalfExtent.Y * 2 + GridDistance) * (
		BoxLocalHalfExtent.Z * 2 + GridDistance)) / (GridDistance * 10000);
	UE_LOG(LogTemp, Warning, TEXT("Voxel Ammount: %i"), VoxelAmmount);

	if (VoxelAmmount > 50000)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cálculo de vóxeles excedido. Intentando calcular %i en un límite de 50000"),
		       VoxelAmmount);
		bCalculatingNavMesh = false;
		return;
	}

	LocalGrid.Empty();
	LinealGrid.Empty();
	GridMap.Empty();
	TestGridMap.Empty();

	// LocalGrid.SetNum(VoxelAmmount + 1);
	// LinealGrid.SetNum(VoxelAmmount + 1);

	FVector CurrentPoint = InitialPoint;

	// Obtenemos los puntos en el espacio local SIN VOXELIZAR
	int GridIndex = 0;
	while (CurrentPoint.Z <= FinalPoint.Z)
	{
		while (CurrentPoint.Y <= FinalPoint.Y)
		{
			while (CurrentPoint.X <= FinalPoint.X)
			{
				// LocalGrid[GridIndex] = CurrentPoint;
				// GridIndex++;
				LocalGrid.Add(CurrentPoint);

				CurrentPoint.X += GridDistance;
			}
			CurrentPoint.X = InitialPoint.X;
			CurrentPoint.Y += GridDistance;
		}
		CurrentPoint.X = InitialPoint.X;
		CurrentPoint.Y = InitialPoint.Y;
		CurrentPoint.Z += GridDistance;
	}

	const int LocalGridSize = LocalGrid.Num();

	// Transformamos los puntos para que vayan en función de la rotación de la caja
	for (int i = 0; i < LocalGridSize; i++)
	{
		FVector TransformedPoint = BoxGlobalTransform.TransformPosition(LocalGrid[i]);
		LocalGrid[i] = TransformedPoint;
	}

	// Voxelizamos los puntos para que vayan en función de las coordenadas globales
	for (int i = 0; i < LocalGridSize; i++)
	{
		FVector VoxelizedPoint = LocalGrid[i] - FVector(
			UKismetMathLibrary::GenericPercent_FloatFloat(LocalGrid[i].X, GridDistance),
			UKismetMathLibrary::GenericPercent_FloatFloat(LocalGrid[i].Y, GridDistance),
			UKismetMathLibrary::GenericPercent_FloatFloat(LocalGrid[i].Z, GridDistance));


		if (!GridMap.Contains(VoxelizedPoint))
		{
			FNavMeshVoxelInfo VoxelInfo;
			GridMap.Add(VoxelizedPoint, VoxelInfo);
			// LinealGrid[i] = VoxelizedPoint;
			LinealGrid.Add(VoxelizedPoint);
		}
	}

	HandleStaticMeshesCollision();

	bCalculatingNavMesh = false;
}

bool AKrakenNavBox::IsInWorldEditor()
{
#if WITH_EDITOR
	UWorld* World = GetWorld();

	if (World)
	{
		FString MapName = World->GetName();
		FString WorldText = FString("World_");

		int Initials = MapName.Find(WorldText);

		// UE_LOG(LogTemp, Warning, TEXT("Map name: %s"), *MapName);
		// UE_LOG(LogTemp, Warning, TEXT("Map index: %i"), Initials);
		if (Initials == 0)
		{
			return false;
		}

		return true;
	}

#endif

	// No nos interesa generar esto en runtime
	return false;
}

void AKrakenNavBox::HandleStaticMeshesCollision()
{
	// TODO 1: En vez de utilizar la caja con la que se crea el NavMesh, usar una caja formada por los vóxeles generados

	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapQuery;
	// Objectos estáticos
	OverlapQuery.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// De momento esto no funciona
	// TArray<UPrimitiveComponent*> OverlappingComponents;
	// UKismetSystemLibrary::ComponentOverlapComponents(Box, Box->GetComponentTransform(), OverlapQuery, AActor::StaticClass(), ActorsToIgnore, OverlappingComponents);
	//
	// UE_LOG(LogTemp, Warning, TEXT("Component Overlap \n ---------------"));
	// UE_LOG(LogTemp, Warning, TEXT("Overlapping Components length: %i"), OverlappingComponents.Num());
	//
	// for (int i = 0; i < OverlappingComponents.Num(); i++)
	// {
	// 	FString ComponentName = OverlappingComponents[i]->GetName();
	// 	UE_LOG(LogTemp, Warning, TEXT("Componente: %s"), *ComponentName);
	// }

	// Obtenemos todos los actores que estén dentro de nuestra caja
	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::ComponentOverlapActors(Box, Box->GetComponentTransform(), OverlapQuery, AActor::StaticClass(),
	                                             ActorsToIgnore, OverlappingActors);
	UE_LOG(LogTemp, Warning, TEXT("Actor Overlap \n ---------------"));
	UE_LOG(LogTemp, Warning, TEXT("Overlapping Actors length: %i"), OverlappingActors.Num());

	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		FString ComponentName = OverlappingActors[i]->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *ComponentName);
	}

	// Por cada actor iteramos por todos los meshes státicos que tenga
	for (int A = 0; A < OverlappingActors.Num(); A++)
	{
		TArray<UStaticMeshComponent*> Components;
		OverlappingActors[A]->GetComponents(UStaticMeshComponent::StaticClass(), Components);

		for (int C = 0; C < Components.Num(); C++)
		{
			FString ComponentName = Components[C]->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Component: %s"), *ComponentName);

			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Components[C]);
			if (UStaticMesh* Mesh = MeshComponent->GetStaticMesh())
			{
				if (UBodySetup* Body = Mesh->GetBodySetup())
				{
					TArray<FKBoxElem> BoxElems = Body->AggGeom.BoxElems;
					UE_LOG(LogTemp, Warning, TEXT("Box Elems Length: %i"), BoxElems.Num());
					for (int B = 0; B < BoxElems.Num(); B++)
					{
						GenerateBoxCollision(BoxElems[B], MeshComponent->GetComponentTransform());
					}
				}
			}
			
		}
	}
}

void AKrakenNavBox::GenerateBoxCollision(FKBoxElem BoxElem, FTransform Transform)
{
	FVector ComponentScale = Transform.GetScale3D();
	FVector BoxLocalHalfExtent = FVector(BoxElem.X / 2.0f, BoxElem.Y / 2.0f,BoxElem.Z / 2.0f);

	BoxLocalHalfExtent = BoxLocalHalfExtent * ComponentScale;

	FVector VoxelizedWorldBoxCenter = BoxElem.Center;

	FVector InitialPoint = VoxelizedWorldBoxCenter - BoxLocalHalfExtent;
	FVector FinalPoint = VoxelizedWorldBoxCenter + BoxLocalHalfExtent;

	// La cantidad de puntos que va a tener la caja es igual al volumen dividido por la distancia
	int VoxelAmmount = ((BoxLocalHalfExtent.X * 2 + GridDistance) * (BoxLocalHalfExtent.Y * 2 + GridDistance) * (
		BoxLocalHalfExtent.Z * 2 + GridDistance)) / (GridDistance * 10000);
	UE_LOG(LogTemp, Warning, TEXT("Voxel Ammount: %i"), VoxelAmmount);

	if (VoxelAmmount > 50000)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cálculo de vóxeles excedido en caja de colisión. Intentando calcular %i en un límite de 50000"),
			   VoxelAmmount);
		return;
	}

	FVector CurrentPoint = InitialPoint;

	// Obtenemos los puntos en el espacio local SIN VOXELIZAR
	while (CurrentPoint.Z <= FinalPoint.Z)
	{
		while (CurrentPoint.Y <= FinalPoint.Y)
		{
			while (CurrentPoint.X <= FinalPoint.X)
			{
				FVector TransformedPoint = Transform.TransformPosition(CurrentPoint/ ComponentScale);

				FVector VoxelizedPoint = TransformedPoint - FVector(
			UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.X, GridDistance),
			UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.Y, GridDistance),
			UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.Z, GridDistance));

				TestGridMap.Add(VoxelizedPoint);
				// Si tenemos el punto VOXELIZADO
				if (GridMap.Contains(VoxelizedPoint))
				{
					// Bloquamos el punto
					// GridMap[VoxelizedPoint].Value = static_cast<unsigned char>(EVoxelValue::BLOCKED);
					GridMap.Remove(VoxelizedPoint);
				}
				
				CurrentPoint.X += GridDistance;
			}
			CurrentPoint.X = InitialPoint.X;
			CurrentPoint.Y += GridDistance;
		}
		CurrentPoint.X = InitialPoint.X;
		CurrentPoint.Y = InitialPoint.Y;
		CurrentPoint.Z += GridDistance;
	}
	
}
