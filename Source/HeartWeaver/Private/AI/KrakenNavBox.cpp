// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/KrakenNavBox.h"

#include "AI/NavMeshVoxelInfo.h"
#include "Components/BoxComponent.h"
#include "Engine/BlockingVolume.h"
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
			GenerateGridInsideBox(true);
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

void AKrakenNavBox::GenerateGridInsideBox(bool GenerateOnlyFloorLevel)
{
	// Tenemos cuidado que no sea un número demasiado pequeño
	if (GridDistance <= 10.0f)
	{
		GridDistance = 10.0f;
	}

	FVector BoxLocalHalfExtent = Box->GetScaledBoxExtent();
	FTransform BoxGlobalTransform = Box->GetComponentTransform();

	// Primero hacemos el cálculo en local así que este valor es 0,0,0
	FVector VoxelizedWorldBoxCenter = FVector(0.0f, 0.0f, 0.0f);

	FVector InitialPoint = VoxelizedWorldBoxCenter - BoxLocalHalfExtent;
	FVector FinalPoint = VoxelizedWorldBoxCenter + BoxLocalHalfExtent;

	// Les añadimos un poco de valor para evitar que voxelice una unidad entera hacia abajo cuando
	// no nos interesa
	// InitialPoint = InitialPoint + FVector(0.0001f,0.0001f,0.0001f);
	// FinalPoint = InitialPoint + FVector(0.0001f,0.0001f,0.0001f);

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

	LocalGrid.Reserve(VoxelAmmount);
	LinealGrid.Reserve(VoxelAmmount);
	GridMap.Reserve(VoxelAmmount);
	TestGridMap.Reserve(VoxelAmmount);

	FVector CurrentPoint = InitialPoint;

	if (GenerateOnlyFloorLevel)
	{
		CurrentPoint.Z = FinalPoint.Z;
	}

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
				LocalGrid.AddUnique(CurrentPoint);

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
		FVector LocalPoint = LocalGrid[i];
		FVector GlobalPoint = BoxGlobalTransform.TransformPosition(LocalPoint);

		FVector VoxelizedPoint = GlobalPoint - FVector(
			(UKismetMathLibrary::GenericPercent_FloatFloat((GlobalPoint.X), GridDistance)),
			(UKismetMathLibrary::GenericPercent_FloatFloat((GlobalPoint.Y), GridDistance)),
			(UKismetMathLibrary::GenericPercent_FloatFloat((GlobalPoint.Z), GridDistance)));


		FString InitialPointString = GlobalPoint.ToString();
		FString VoxelizedPointString = VoxelizedPoint.ToString();

		// UE_LOG(LogTemp, Warning, TEXT("InitialPointString: %s"), *InitialPointString);
		// UE_LOG(LogTemp, Warning, TEXT("VoxelizedPointString: %s"), *VoxelizedPointString);

		// if (!GridMap.Contains(VoxelizedPoint))
		{
			// FNavMeshVoxelInfo VoxelInfo;
			GridMap.Add(VoxelizedPoint);
			LinealGrid.Add(VoxelizedPoint);
		}
	}

	HandleStaticMeshesCollision();
	HandleBlockingVolumeCollision();
	
	HandleFloorCollision();

	DrawDebugVisualization();

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
	UE_LOG(LogTemp, Warning, TEXT("HandleStaticMeshesCollision"));
	// TODO 1: En vez de utilizar la caja con la que se crea el NavMesh, usar una caja formada por los vóxeles generados

	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapQuery;
	// Objectos estáticos
	OverlapQuery.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// Obtenemos todos los actores que estén dentro de nuestra caja
	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::ComponentOverlapActors(Box, Box->GetComponentTransform(), OverlapQuery, AActor::StaticClass(),
	                                             ActorsToIgnore, OverlappingActors);
	// UE_LOG(LogTemp, Warning, TEXT("Actor Overlap \n ---------------"));
	// UE_LOG(LogTemp, Warning, TEXT("Overlapping Actors length: %i"), OverlappingActors.Num());

	for (int i = 0; i < OverlappingActors.Num(); i++)
	{
		FString ActorName = OverlappingActors[i]->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *ActorName);
	}

	// Por cada actor iteramos por todos los meshes státicos que tenga
	for (int A = 0; A < OverlappingActors.Num(); A++)
	{
		TArray<UStaticMeshComponent*> StaticMeshComponents;
		OverlappingActors[A]->GetComponents(UStaticMeshComponent::StaticClass(), StaticMeshComponents);

		for (int C = 0; C < StaticMeshComponents.Num(); C++)
		{
			FString ComponentName = StaticMeshComponents[C]->GetName();
			// UE_LOG(LogTemp, Warning, TEXT("Component: %s"), *ComponentName);

			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(StaticMeshComponents[C]);
			if (UStaticMesh* Mesh = MeshComponent->GetStaticMesh())
			{
				if (UBodySetup* Body = Mesh->GetBodySetup())
				{
					TArray<FKBoxElem> BoxElems = Body->AggGeom.BoxElems;
					TArray<FKSphereElem> SphereElems = Body->AggGeom.SphereElems;
					// UE_LOG(LogTemp, Warning, TEXT("Box Elems Length: %i"), BoxElems.Num());
					for (int B = 0; B < BoxElems.Num(); B++)
					{
						FVector BoxLocalHalfExtent = FVector(BoxElems[B].X / 2.0f, BoxElems[B].Y / 2.0f, BoxElems[B].Z / 2.0f);
						FVector BoxCenter = BoxElems[B].Center;
						GenerateBoxCollision(BoxCenter, BoxLocalHalfExtent, MeshComponent->GetComponentTransform());
					}

					for (int S = 0; S < SphereElems.Num(); S++)
					{
						// GenerateSphereCollision(SphereElems[S], MeshComponent->GetComponentTransform());
					}
				}
			}
		}
	}
}

void AKrakenNavBox::HandleBlockingVolumeCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("HandleBlockingVolumeCollision"));


		TArray<TEnumAsByte<EObjectTypeQuery>> OverlapQuery;
	// Objectos estáticos
	OverlapQuery.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// Obtenemos todos los actores que estén dentro de nuestra caja
	TArray<AActor*> OverlappingBlockingVolumes;
	UKismetSystemLibrary::ComponentOverlapActors(Box, Box->GetComponentTransform(), OverlapQuery, ABlockingVolume::StaticClass(),
	                                             ActorsToIgnore, OverlappingBlockingVolumes);
	// UE_LOG(LogTemp, Warning, TEXT("Actor Overlap \n ---------------"));
	// UE_LOG(LogTemp, Warning, TEXT("Overlapping Actors length: %i"), OverlappingActors.Num());

	for (int i = 0; i < OverlappingBlockingVolumes.Num(); i++)
	{
		FString ActorName = OverlappingBlockingVolumes[i]->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *ActorName);
	}

	// Por cada actor iteramos por todos los meshes státicos que tenga
	for (int A = 0; A < OverlappingBlockingVolumes.Num(); A++)
	{
		ABlockingVolume* Volume = Cast<ABlockingVolume>( OverlappingBlockingVolumes[A]);
			// Volume->Brush->Bounds.

		FVector BoxLocalHalfExtent = Volume->Brush->Bounds.BoxExtent;
		FVector BoxCenter = FVector(0.0f,0.0f,0.0f);
		GenerateBoxCollision(BoxCenter, BoxLocalHalfExtent, Volume->GetTransform());
	}
	
}

void AKrakenNavBox::HandleFloorCollision()
{
	UWorld* World = GetWorld();
	FVector DownVector = FVector() - GetActorUpVector();

	TArray<FVector> DeleteGridMap;
	TSet<FVector> FloorGridMap;

	for (const FVector Voxel : GridMap)
	{
		FVector Point = Voxel;
		FHitResult Hit;

		FVector Start = Point;
		FVector End = Point + (DownVector * AgentHeight);
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
		{
			FVector VoxelizedPoint = Hit.ImpactPoint - FVector(
				UKismetMathLibrary::GenericPercent_FloatFloat(Hit.ImpactPoint.X, GridDistance),
				UKismetMathLibrary::GenericPercent_FloatFloat(Hit.ImpactPoint.Y, GridDistance),
				UKismetMathLibrary::GenericPercent_FloatFloat(Hit.ImpactPoint.Z, GridDistance));

			// if (!GridMap.Contains(VoxelizedPoint))
			{
				FloorGridMap.Add(VoxelizedPoint);
				DeleteGridMap.Add(Point);
			}
		}else
		{
			DeleteGridMap.Add(Point);
		}
	}

	for (int i = 0; i < DeleteGridMap.Num(); i++)
	{
		GridMap.Remove(DeleteGridMap[i]);
	}

	for (const FVector Voxel : FloorGridMap)
	{
		GridMap.Add(Voxel);
	}
}

void AKrakenNavBox::GenerateBoxCollision(FVector BoxCenter, FVector BoxLocalHalfExtent, FTransform Transform)
{
	FVector ComponentScale = Transform.GetScale3D();

	BoxLocalHalfExtent = BoxLocalHalfExtent * ComponentScale;

	FVector InitialPoint = BoxCenter - BoxLocalHalfExtent;
	FVector FinalPoint = BoxCenter + BoxLocalHalfExtent;

	// La cantidad de puntos que va a tener la caja es igual al volumen dividido por la distancia
	int VoxelAmmount = ((BoxLocalHalfExtent.X * 2 + GridDistance) * (BoxLocalHalfExtent.Y * 2 + GridDistance) * (
		BoxLocalHalfExtent.Z * 2 + GridDistance)) / (GridDistance * 10000);
	UE_LOG(LogTemp, Warning, TEXT("Voxel Ammount: %i"), VoxelAmmount);

	if (VoxelAmmount > 50000)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Cálculo de vóxeles excedido en caja de colisión. Intentando calcular %i en un límite de 50000"),
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
				FVector TransformedPoint = Transform.TransformPosition(CurrentPoint / ComponentScale);

				FVector VoxelizedPoint = TransformedPoint - FVector(
					UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.X, GridDistance),
					UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.Y, GridDistance),
					UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.Z, GridDistance));

				// Si tenemos el punto VOXELIZADO
				if (GridMap.Contains(VoxelizedPoint))
				{
					TestGridMap.Add(VoxelizedPoint);
					GridMap.Remove(VoxelizedPoint);
				}

				CurrentPoint.X += (GridDistance );
			}
			CurrentPoint.X = InitialPoint.X;
			CurrentPoint.Y += (GridDistance );
		}
		CurrentPoint.X = InitialPoint.X;
		CurrentPoint.Y = InitialPoint.Y;
		CurrentPoint.Z += (GridDistance );
	}
}

void AKrakenNavBox::GenerateSphereCollision(FKSphereElem SphereElem, FTransform Transform)
{
	// TODO: Que esto funcione bien, por alguna razón aumentar el radio empeora los resultados
	
	FVector ComponentScale = Transform.GetScale3D();
	float SphereRadius = /*FMath::Min(FMath::Min(ComponentScale.X, ComponentScale.Y), ComponentScale.Z) **/10.0f * SphereElem.Radius;

	UE_LOG(LogTemp, Warning, TEXT("Sphere radius: %f"), SphereRadius);
	
	FVector BoxLocalHalfExtent = FVector(1.0f, 1.0f, 1.0f) * SphereRadius;

	BoxLocalHalfExtent = BoxLocalHalfExtent * ComponentScale;

	FVector SphereCenter = SphereElem.Center;

	FString SphereCenterText = SphereCenter.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Local Sphere center: %s"), *SphereCenterText);

	FVector InitialPoint = SphereCenter - BoxLocalHalfExtent;
	FVector FinalPoint = SphereCenter + BoxLocalHalfExtent;

	// La cantidad de puntos que va a tener la caja es igual al volumen dividido por la distancia
	int VoxelAmmount = ((BoxLocalHalfExtent.X * 2 + GridDistance) * (BoxLocalHalfExtent.Y * 2 + GridDistance) * (
		BoxLocalHalfExtent.Z * 2 + GridDistance)) / (GridDistance * 10000);
	UE_LOG(LogTemp, Warning, TEXT("Voxel Ammount: %i"), VoxelAmmount);

	FString InitialPointText = InitialPoint.ToString();
	FString FinalPointText = FinalPoint.ToString();
	UE_LOG(LogTemp, Warning, TEXT("InitialPoint On Sphere: %s"), *InitialPointText);
	UE_LOG(LogTemp, Warning, TEXT("InitialPoint On Sphere: %s"), *FinalPointText);


	FVector TransformedCenter = Transform.TransformPosition(SphereCenter);
	DrawDebugSphere(GetWorld(), TransformedCenter, SphereRadius, 20, FColor::Emerald, false, 50.0f, 0, 3.0f);

	FString SphereCenterGlobalText = TransformedCenter.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Global Sphere center: %s"), *SphereCenterGlobalText);
	
	if (VoxelAmmount > 50000)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("Cálculo de vóxeles excedido en caja de colisión. Intentando calcular %i en un límite de 50000"),
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
				if (FVector().Distance(SphereCenter, CurrentPoint) < SphereElem.Radius)
				{
					FVector TransformedPoint = Transform.TransformPosition(CurrentPoint / ComponentScale);

					FVector VoxelizedPoint = TransformedPoint - FVector(
						UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.X, GridDistance),
						UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.Y, GridDistance),
						UKismetMathLibrary::GenericPercent_FloatFloat(TransformedPoint.Z, GridDistance));

					TestGridMap.Add(VoxelizedPoint);
					// Si tenemos el punto VOXELIZADO
					if (GridMap.Contains(VoxelizedPoint))
					{
						GridMap.Remove(VoxelizedPoint);
					}

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

void AKrakenNavBox::DrawDebugVisualization()
{
	UWorld* World = GetWorld();
	FlushPersistentDebugLines(World);
	for (FVector Pos : GridMap)
	{
		DrawDebugSphere(World, Pos, 10.0f, 4, FColor::Yellow, false, 50.0f);
	}
	for (FVector Pos : TestGridMap)
	{
		DrawDebugSphere(World, Pos, 10.0f, 4, FColor::Red, false, 50.0f);
	}
}
