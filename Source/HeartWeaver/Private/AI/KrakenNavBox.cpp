// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/KrakenNavBox.h"

#include "AI/KrakenPathfinding.h"
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

	GridDistance = 50.0f;
	AgentHeight = 100.0f;
	AvoidanceRadius = 100.0f;

	Box->SetWorldScale3D(FVector(1, 1, 1));
	Box->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Box->SetBoxExtent(GetActorScale3D() * 50.0f);
}
#if WITH_EDITOR
void AKrakenNavBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!bCalculatingNavMesh)
	{
		Box->SetWorldScale3D(FVector(1, 1, 1));
		Box->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		Box->SetBoxExtent(GetActorScale3D() * 50.0f);

		if (bUpdateNavMesh)
		{
			bUpdateNavMesh = false;
			bCalculatingNavMesh = true;
			GenerateGridInsideBox();
		}
	}

	if (bGenerateRandomPath)
	{
		bGenerateRandomPath = false;
		TestPathfinding();
	}

	if (bGenerateDebug)
	{
		bGenerateDebug = false;
		DrawDebugVisualization();
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

	LocalGridMap.Empty();
	TempLocalGridMap.Empty();
	LinealGrid.Empty();
	GridMap.Empty();
	TestGridMap.Empty();

	LocalGridMap.Reserve(VoxelAmmount);
	TempLocalGridMap.Reserve(VoxelAmmount);
	LinealGrid.Reserve(VoxelAmmount);
	GridMap.Reserve(VoxelAmmount);
	TestGridMap.Reserve(VoxelAmmount);

	FVector CurrentPoint = InitialPoint;

	if (bGenerateOnlyFloorLevel)
	{
		CurrentPoint.Z = FinalPoint.Z;
		DesignedZAnalisisLevel = CurrentPoint.Z;
	}

	// Obtenemos los puntos en el espacio local SIN VOXELIZAR
	while (CurrentPoint.Z <= FinalPoint.Z)
	{
		while (CurrentPoint.Y <= FinalPoint.Y)
		{
			while (CurrentPoint.X <= FinalPoint.X)
			{
				// LocalGrid[GridIndex] = CurrentPoint;
				// GridIndex++;
				TempLocalGridMap.Add(CurrentPoint);

				CurrentPoint.X += GridDistance;
			}
			CurrentPoint.X = InitialPoint.X;
			CurrentPoint.Y += GridDistance;
		}
		CurrentPoint.X = InitialPoint.X;
		CurrentPoint.Y = InitialPoint.Y;
		CurrentPoint.Z += GridDistance;
	}

	// Transformamos los puntos para que vayan en función de la rotación de la caja
	for (FVector LocalPoint : TempLocalGridMap)
	{
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

			FVector VoxelizedLocalPoint = BoxGlobalTransform.InverseTransformPosition(VoxelizedPoint);
			LocalGridMap.Add(VoxelizedLocalPoint);
		}
	}

	HandleStaticMeshesCollision();
	HandleBlockingVolumeCollision();

	HandleFloorCollision();
	HandleAvoidanceRadius();

	UE_LOG(LogTemp, Warning, TEXT("Grid Map length: %i"), GridMap.Num());
	UE_LOG(LogTemp, Warning, TEXT("Local Grid Map length: %i"), LocalGridMap.Num());


	DrawDebugVisualization();

	bCalculatingNavMesh = false;
}

FVector AKrakenNavBox::GetClosestPointInNavMesh(FVector Point)
{
	// UE_LOG(LogTemp, Warning, TEXT("Gridmap Ammount: %i"), GridMap.Num());

	float MinDistance = 9999999999.9f;
	FVector SelectedVoxel = FVector(0.0f, 0.0f, 0.0f);
	for (FVector Voxel : GridMap)
	{
		if (!BlockedGridMap.Contains(Voxel))
		{
			float Distance = FVector::Dist(Voxel, Point);
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				SelectedVoxel = Voxel;
			}
		}
	}
	return SelectedVoxel;
}

TArray<FVector> AKrakenNavBox::GetPath(FVector StartPoint, FVector EndPoint, bool Simplify)
{
	FVector InitialPoint = GetClosestPointInNavMesh(StartPoint);
	FVector FinalPoint = GetClosestPointInNavMesh(EndPoint);

	TArray<FVector> Path =
		KrakenPathfinding::GetPath(InitialPoint, FinalPoint, GridDistance, &GridMap, &BlockedGridMap);

	if (Simplify)
	{
		FVector UpVector = GetActorUpVector();

		TArray<FVector> FinalPath;
		FinalPath.Reserve(Path.Num());

		// Esto es una cutrada enorme. TODO: A futuro, cambiar la matriz de vóxeles con valores donde se pueda acceder mejor los valores tanto globales como locales (Con un buen hash) 
		TArray<FVector> ZDirections = {
			FVector(GridDistance, 0, 0),
			FVector(-GridDistance, 0, 0),
			FVector(0, GridDistance, 0),
			FVector(0, -GridDistance, 0),
		};
		TArray<FVector> YDirections = {
			FVector(GridDistance, 0, 0),
			FVector(-GridDistance, 0, 0),
			FVector(0, 0, GridDistance),
			FVector(0, 0, -GridDistance),
		};
		TArray<FVector> XDirections = {
			FVector(0, GridDistance, 0),
			FVector(0, -GridDistance, 0),
			FVector(0, 0, GridDistance),
			FVector(0, 0, -GridDistance),
		};

		TArray<FVector> Directions;
		if (FMath::Abs(UpVector.X) >= 0.7f)
		{
			Directions = XDirections;
		}
		else if (FMath::Abs(UpVector.Y) >= 0.7f)
		{
			Directions = YDirections;
		}
		else
		{
			Directions = ZDirections;
		}

		// Borramos todos los puntos que se encuentren dentro de los vóxeles y no tengan ningún hueco al lado
		for (int i = 1; i < Path.Num() - 1; i++)
		{
			bool DeletePathPoint = true;

			for (const FVector& Direction : Directions)
			{
				FVector NeighborPosition = Path[i] + Direction;

				if (!GridMap.Contains(NeighborPosition) || BlockedGridMap.Contains(NeighborPosition))
				{
					DeletePathPoint = false;
					break;
				}
			}

			if (DeletePathPoint)
			{
				Path.RemoveAt(i);;
				i--;
			}
			else
			{
				// FinalPath.Add(Path[i]);
			}
		}

		// De los puntos que quedan, limpiamos aquellos que nos proporcionen poca información (por lo general, esquinas mal puestas)

		// return FinalPath;
	}

	return Path;
}

FVector AKrakenNavBox::GetRandomPointInNavMesh()
{
	int Index = FMath::RandRange(0, GridMap.Num() - 1);
	int TempIndex = 0;
	for (FVector Voxel : GridMap)
	{
		TempIndex++;
		if (TempIndex == Index)
		{
			return Voxel;
		}
	}
	return FVector(0.0f, 0.0f, 0.0f);
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
			ECollisionEnabled::Type CollisionType = MeshComponent->GetCollisionEnabled();

			if (MeshComponent->IsCollisionEnabled() && CollisionType != ECollisionEnabled::NoCollision)
			{
				if (UStaticMesh* Mesh = MeshComponent->GetStaticMesh())
				{
					if (UBodySetup* Body = Mesh->GetBodySetup())
					{
						TArray<FKBoxElem> BoxElems = Body->AggGeom.BoxElems;
						TArray<FKSphereElem> SphereElems = Body->AggGeom.SphereElems;
						// UE_LOG(LogTemp, Warning, TEXT("Box Elems Length: %i"), BoxElems.Num());
						for (int B = 0; B < BoxElems.Num(); B++)
						{
							FVector BoxLocalHalfExtent = FVector(BoxElems[B].X / 2.0f, BoxElems[B].Y / 2.0f,
							                                     BoxElems[B].Z / 2.0f);
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
	UKismetSystemLibrary::ComponentOverlapActors(Box, Box->GetComponentTransform(), OverlapQuery,
	                                             ABlockingVolume::StaticClass(),
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
		ABlockingVolume* Volume = Cast<ABlockingVolume>(OverlappingBlockingVolumes[A]);
		// Volume->Brush->Bounds.

		FVector BoxLocalHalfExtent = Volume->Brush->Bounds.BoxExtent;
		FVector BoxCenter = FVector(0.0f, 0.0f, 0.0f);
		GenerateBoxCollision(BoxCenter, BoxLocalHalfExtent, Volume->GetTransform());
	}
}

void AKrakenNavBox::HandleFloorCollision()
{
	FTransform BoxGlobalTransform = Box->GetComponentTransform();

	UWorld* World = GetWorld();
	FVector DownVector = FVector(0.0f, 0.0f, 0.0f) - GetActorUpVector();

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
		}
		else
		{
			DeleteGridMap.Add(Point);
		}
	}

	for (int i = 0; i < DeleteGridMap.Num(); i++)
	{
		GridMap.Remove(DeleteGridMap[i]);

		FVector VoxelizedLocalPoint = BoxGlobalTransform.InverseTransformPosition(DeleteGridMap[i]);
		LocalGridMap.Remove(VoxelizedLocalPoint);
	}

	for (const FVector Voxel : FloorGridMap)
	{
		GridMap.Add(Voxel);

		FVector VoxelizedLocalPoint = BoxGlobalTransform.InverseTransformPosition(Voxel);
		LocalGridMap.Add(VoxelizedLocalPoint);
	}
}

void AKrakenNavBox::GenerateBoxCollision(FVector BoxCenter, FVector BoxLocalHalfExtent, FTransform Transform)
{
	FTransform BoxGlobalTransform = Box->GetComponentTransform();
	FVector ComponentScale = Transform.GetScale3D();

	BoxLocalHalfExtent = BoxLocalHalfExtent * ComponentScale;

	FVector InitialPoint = BoxCenter - BoxLocalHalfExtent;
	FVector FinalPoint = BoxCenter + BoxLocalHalfExtent;

	// Obtenemos los puntos en el espacio local
	TArray<FVector> BoxPoints;
	BoxPoints.Add(InitialPoint);
	BoxPoints.Add(FVector(FinalPoint.X, InitialPoint.Y, InitialPoint.Z));
	BoxPoints.Add(FVector(InitialPoint.X, FinalPoint.Y, InitialPoint.Z));
	BoxPoints.Add(FVector(InitialPoint.X, InitialPoint.Y, FinalPoint.Z));

	BoxPoints.Add(FinalPoint);
	BoxPoints.Add(FVector(InitialPoint.X, FinalPoint.Y, FinalPoint.Z));
	BoxPoints.Add(FVector(FinalPoint.X, InitialPoint.Y, FinalPoint.Z));
	BoxPoints.Add(FVector(FinalPoint.X, FinalPoint.Y, InitialPoint.Z));

	// Obtenemos el bounding box GLOBAL de la caja
	FVector MinBoundingBox = FVector(UE_BIG_NUMBER,UE_BIG_NUMBER,UE_BIG_NUMBER);
	FVector MaxBoundingBox = FVector(UE_SMALL_NUMBER,UE_SMALL_NUMBER,UE_SMALL_NUMBER);
	for (int i = 0; i < BoxPoints.Num(); i++)
	{
		MinBoundingBox = FVector(
			FMath::Min(MinBoundingBox.X, Transform.TransformPosition(BoxPoints[i] / ComponentScale).X),
			FMath::Min(MinBoundingBox.Y, Transform.TransformPosition(BoxPoints[i] / ComponentScale).Y),
			FMath::Min(MinBoundingBox.Z, Transform.TransformPosition(BoxPoints[i] / ComponentScale).Z));

		MaxBoundingBox = FVector(
			FMath::Max(MaxBoundingBox.X, Transform.TransformPosition(BoxPoints[i] / ComponentScale).X),
			FMath::Max(MaxBoundingBox.Y, Transform.TransformPosition(BoxPoints[i] / ComponentScale).Y),
			FMath::Max(MaxBoundingBox.Z, Transform.TransformPosition(BoxPoints[i] / ComponentScale).Z));
		// DrawDebugSphere(GetWorld(), Transform.TransformPosition(BoxPoints[i] / ComponentScale), 20.0f, 10, FColor::Blue,
		//                 true, 99.0f);
	}

	// DrawDebugSphere(GetWorld(), MinBoundingBox, 20.0f, 10, FColor::Blue, true, 99.0f);
	// DrawDebugSphere(GetWorld(), MaxBoundingBox, 20.0f, 10, FColor::Blue, true, 99.0f);

	FVector CurrentPoint = MinBoundingBox;

	// TODO: Técnicamente en caso de generar solo un piso se podría optimizar un poco
	// if (bGenerateOnlyFloorLevel)
	// {
	// 	
	// }

	// Obtenemos los puntos en el espacio local SIN VOXELIZAR
	while (CurrentPoint.Z <= MaxBoundingBox.Z)
	{
		while (CurrentPoint.Y <= MaxBoundingBox.Y)
		{
			while (CurrentPoint.X <= MaxBoundingBox.X)
			{
				FVector InverseTransformedPoint = Transform.InverseTransformPosition(CurrentPoint) * ComponentScale;

				if (InverseTransformedPoint.X < InitialPoint.X || InverseTransformedPoint.Y < InitialPoint.Y
					|| InverseTransformedPoint.Z < InitialPoint.Z || InverseTransformedPoint.X > FinalPoint.X ||
					InverseTransformedPoint.Y > FinalPoint.Y
					|| InverseTransformedPoint.Z > FinalPoint.Z)
				{
				}
				else
				{
					// FVector TransformedPoint = Transform.TransformPosition(CurrentPoint / ComponentScale);

					FVector VoxelizedPoint = CurrentPoint - FVector(
						UKismetMathLibrary::GenericPercent_FloatFloat(CurrentPoint.X, GridDistance),
						UKismetMathLibrary::GenericPercent_FloatFloat(CurrentPoint.Y, GridDistance),
						UKismetMathLibrary::GenericPercent_FloatFloat(CurrentPoint.Z, GridDistance));

					// Si tenemos el punto VOXELIZADO
					if (GridMap.Contains(VoxelizedPoint))
					{
						TestGridMap.Add(VoxelizedPoint);
						GridMap.Remove(VoxelizedPoint);

						FVector VoxelizedLocalPoint = BoxGlobalTransform.InverseTransformPosition(VoxelizedPoint);
						LocalGridMap.Remove(VoxelizedLocalPoint);
					}
				}


				CurrentPoint.X += (GridDistance);
			}
			CurrentPoint.X = MinBoundingBox.X;
			CurrentPoint.Y += (GridDistance);
		}
		CurrentPoint.X = MinBoundingBox.X;
		CurrentPoint.Y = MinBoundingBox.Y;
		CurrentPoint.Z += (GridDistance);
	}
}

void AKrakenNavBox::GenerateSphereCollision(FKSphereElem SphereElem, FTransform Transform)
{
	FTransform BoxGlobalTransform = Box->GetComponentTransform();

	// TODO: Que esto funcione bien, por alguna razón aumentar el radio empeora los resultados

	FVector ComponentScale = Transform.GetScale3D();
	float SphereRadius = /*FMath::Min(FMath::Min(ComponentScale.X, ComponentScale.Y), ComponentScale.Z) **/10.0f *
		SphereElem.Radius;

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
	// DrawDebugSphere(GetWorld(), TransformedCenter, SphereRadius, 20, FColor::Emerald, false, 50.0f, 0, 3.0f);

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
				if (FVector(0.0f, 0.0f, 0.0f).Distance(SphereCenter, CurrentPoint) < SphereElem.Radius)
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

						FVector VoxelizedLocalPoint = BoxGlobalTransform.InverseTransformPosition(VoxelizedPoint);
						LocalGridMap.Remove(VoxelizedLocalPoint);
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

void AKrakenNavBox::HandleAvoidanceRadius()
{
	FTransform BoxGlobalTransform = Box->GetComponentTransform();

	TSet<FVector> GridLocalBorders;
	GridLocalBorders.Reserve(GridMap.Num());


	for (const FVector LocalVoxel : LocalGridMap)
	{
		// FString LocalVoxelString = LocalVoxel.ToString();
		// UE_LOG(LogTemp, Warning, TEXT("LocalVoxelString: %s"), *LocalVoxelString);


		for (int i = 0; i < 4; i++)
		{
			FVector Voxel = LocalVoxel;
			switch (i)
			{
			case 0:
				Voxel = Voxel + FVector(GridDistance, 0.0f, 0.0f);
				break;
			case 1:
				Voxel = Voxel + FVector(-GridDistance, 0.0f, 0.0f);
				break;
			case 2:
				Voxel = Voxel + FVector(0.0f, GridDistance, 0.0f);
				break;
			case 3:
				Voxel = Voxel + FVector(0.0f, -GridDistance, 0.0f);
				break;

			default: break;
			}

			bool AddBorder = true;
			for (const FVector SecondLocalVoxel : LocalGridMap)
			{
				if (FVector::Dist(SecondLocalVoxel, Voxel) < 0.1f)
				{
					AddBorder = false;
					break;
				}
			}
			if (AddBorder)
			{
				GridLocalBorders.Add(Voxel);
			}
			// if (!LocalGridMap.Contains(Voxel))
			// {
			// 	GridLocalBorders.Add(Voxel);
			// }
		}
	}

	TArray<FVector> DeleteGridMap;

	for (const FVector LocalVoxel : GridLocalBorders)
	{
		FVector Voxel = BoxGlobalTransform.TransformPosition(LocalVoxel);

		for (const FVector GlobalVoxel : GridMap)
		{
			if (FVector::Dist(Voxel, GlobalVoxel) < AvoidanceRadius)
			{
				DeleteGridMap.Add(GlobalVoxel);
			}
		}
	}

	for (int i = 0; i < DeleteGridMap.Num(); i++)
	{
		GridMap.Remove(DeleteGridMap[i]);

		FVector VoxelizedLocalPoint = BoxGlobalTransform.InverseTransformPosition(DeleteGridMap[i]);
		LocalGridMap.Remove(VoxelizedLocalPoint);
	}

	for (const FVector LocalVoxel : GridLocalBorders)
	{
		FVector Voxel = BoxGlobalTransform.TransformPosition(LocalVoxel);
		DrawDebugSphere(GetWorld(), Voxel, 10.0f, 3, FColor::Orange, false, 50.0f);
	}
}

void AKrakenNavBox::TestPathfinding()
{
	int StartPointIndex = FMath::RandRange(0, GridMap.Num() - 1);
	int EndPointIndex = FMath::RandRange(0, GridMap.Num() - 1);

	FVector StartPoint = FVector(0, 0, 0);
	FVector EndPoint = FVector(0, 0, 0);

	int TempIndex = 0;
	for (FVector Voxel : GridMap)
	{
		TempIndex++;
		if (TempIndex == StartPointIndex)
		{
			StartPoint = Voxel;
		}
		if (TempIndex == EndPointIndex)
		{
			EndPoint = Voxel;
		}
	}

	FlushPersistentDebugLines(GetWorld());
	DrawDebugSphere(GetWorld(), StartPoint, 30.0f, 3, FColor::Green, false, 50.0f);
	DrawDebugSphere(GetWorld(), EndPoint, 30.0f, 3, FColor::Purple, false, 50.0f);

	UE_LOG(LogTemp, Warning, TEXT("========================================================================"));

	TArray<FVector> Path = KrakenPathfinding::GetPath(StartPoint, EndPoint, GridDistance, &GridMap);
	UE_LOG(LogTemp, Warning, TEXT("Path Ammount: %i"), Path.Num());

	for (int i = 0; i < Path.Num() - 1; i++)
	{
		DrawDebugLine(GetWorld(), Path[i], Path[i + 1], FColor::White, true, 99.0f, 0, 4.0f);
	}
}

void AKrakenNavBox::DrawDebugVisualization()
{
	FTransform BoxGlobalTransform = Box->GetComponentTransform();
	UWorld* World = GetWorld();
	if (bFlushPreviousDebug)
	{
		FlushPersistentDebugLines(World);
	}
	if (bDrawDebug)
	{
		if (!bDrawLocalDebug)
		{
			for (FVector Pos : GridMap)
			{
				DrawDebugSphere(World, Pos, 5.0f, 3, FColor::Yellow, false, 50.0f);
			}
		}
		else
		{
			for (FVector Pos : LocalGridMap)
			{
				FVector GlobalPoint = BoxGlobalTransform.TransformPosition(Pos);

				DrawDebugSphere(World, GlobalPoint, 5.0f, 3, FColor::Blue, false, 50.0f);
			}
		}
		for (FVector Pos : TestGridMap)
		{
			DrawDebugSphere(World, Pos, 5.0f, 3, FColor::Red, false, 50.0f);
		}
	}
}
