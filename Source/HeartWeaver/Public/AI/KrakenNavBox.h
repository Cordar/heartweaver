// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KrakenNavBox.generated.h"

// struct FNavMeshVoxelInfo;

struct FKSphereElem;
struct FKBoxElem;
class UBoxComponent;

UCLASS()
class HEARTWEAVER_API AKrakenNavBox : public AActor
{
	GENERATED_BODY()

	bool bCalculatingNavMesh = false;

	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	float AgentHeight;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	float AvoidanceRadius;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bUpdateNavMesh = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bGenerateOnlyFloorLevel = true;

	float DesignedZAnalisisLevel;

	UPROPERTY()
	TSet<FVector> GridMap;

	UPROPERTY()
	TSet<FVector> BlockedGridMap;

	// UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	TSet<FVector> TestGridMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	float GridDistance;

	// UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	TArray<FVector> LinealGrid;

	// Array creado aquí para ahorrar memoria
	TSet<FVector> LocalGridMap;
	TSet<FVector> TempLocalGridMap;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	UBoxComponent* Box;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bFlushPreviousDebug = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bDrawDebug = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bDrawLocalDebug = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bGenerateRandomPath = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bGenerateDebug = false;

public:
	// Sets default values for this actor's properties
	AKrakenNavBox();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	 * Genera la matriz de vóxeles para el sistema de navegación
	 */
	UFUNCTION(BlueprintCallable)
	void GenerateGridInsideBox();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetClosestPointInNavMesh(FVector Point);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FVector> GetPath(FVector StartPoint, FVector EndPoint, bool bSimplify = true, bool bEvadeObstacles = true);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetRandomPointInNavMesh();

	/*
	 * Esta función detecta si estamos en el editor y solo si estamos en un nivel y no en la previsualización del blueprint
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInWorldEditor();

	TSet<FVector>* GetGridMap() { return &GridMap; };
	TSet<FVector>* GetBlockedGridMap() { return &BlockedGridMap; };


	float GetGridDistance() const { return GridDistance; }

private:
	/*
	 * Llamado desde el generador de malla, comprueba y bloquea todos los vóxeles que estén dentro de una colisión
	 */
	void HandleStaticMeshesCollision();

	void HandleBlockingVolumeCollision();

	/*
	 * Según la orientación de la caja designamos la altura Z relativa correspondiente
	 */
	void HandleFloorCollision();

	/*
	 * Dada una caja de colisión, llena su interior de valores de bloqueo
	 */
	void GenerateBoxCollision(FVector BoxCenter, FVector BoxLocalHalfExtent, FTransform Transform);

	/*
	* Dada una esfera de colisión, llena su interior de valores de bloqueo
	*/
	void GenerateSphereCollision(FKSphereElem SphereElem, FTransform Transform);

	void HandleAvoidanceRadius();

	void TestPathfinding();

	void DrawDebugVisualization();
};
