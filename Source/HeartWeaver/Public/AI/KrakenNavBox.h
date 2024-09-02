// Copyright Kraken's Den Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NavMeshVoxelInfo.h"
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

	USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	float AgentHeight;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	bool bUpdateNavMesh = false;

	// UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	TSet<FVector> GridMap;

	// UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	TSet<FVector> TestGridMap;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	float GridDistance;

	// UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	TArray<FVector> LinealGrid;

	// Array creado aquí para ahorrar memoria
	TArray<FVector> LocalGrid;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category= "Grid")
	UBoxComponent* Box;

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
	void GenerateGridInsideBox(bool GenerateOnlyFloorLevel);

	/*
	 * Esta función detecta si estamos en el editor y solo si estamos en un nivel y no en la previsualización del blueprint
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInWorldEditor();

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

	void DrawDebugVisualization();
};
