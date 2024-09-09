// Copyright Kraken's Den Studios. All rights reserved.


#include "AI/EnemyAIPerception.h"

#include "EnemyTargetInterface.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UEnemyAIPerception::UEnemyAIPerception()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UEnemyAIPerception::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComponent = Cast<UCapsuleComponent>(GetOwner()->GetComponentByClass(UCapsuleComponent::StaticClass()));

	if (CapsuleComponent)
	{
		HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight() / 2;
	}
	// ...
}

bool UEnemyAIPerception::CheckIsInSight(AActor* TargetActor)
{
	FVector TargetGlobalPosition = TargetActor->GetActorLocation();
	FVector TargetLocalPosition = GetOwner()->GetTransform().InverseTransformPosition(TargetGlobalPosition);
	
	if (TargetActor->Implements<UEnemyTargetInterface>())
	{
		FVector HighestPoint = IEnemyTargetInterface::Execute_GetHighestPoint(TargetActor);
		FVector LowestPoint = IEnemyTargetInterface::Execute_GetLowestPoint(TargetActor);

		FVector HighestLocalPoint = GetOwner()->GetTransform().InverseTransformPosition(HighestPoint);
		FVector LowestLocalPoint = GetOwner()->GetTransform().InverseTransformPosition(LowestPoint);

		// UE_LOG(LogTemp, Warning, TEXT("DETECTAMOS INTERFAZ"));
		if ((HighestLocalPoint.Z > EyesPosition.Z + HalfHeight && LowestLocalPoint.Z > EyesPosition.Z + HalfHeight) ||
			(HighestLocalPoint.Z < EyesPosition.Z - HalfHeight && LowestLocalPoint.Z < EyesPosition.Z - HalfHeight))
		{
			// Esta fuera de nuestro rango de altura
			return false;
		}
	}
	else
	{
		if (TargetLocalPosition.Z > (EyesPosition.Z + HalfHeight) || TargetLocalPosition.Z < (EyesPosition.Z -
			HalfHeight))
		{
			// Está encima nuestra, no le vemos.
			return false;
		}
	}

	FVector LocalDirection = TargetLocalPosition;
	LocalDirection.Normalize();

	// 0.0174533f -> Conversión de grados a PI radianes

	float DotProduct = FVector(1.0f, 0.0f, 0.0f).Dot(LocalDirection);

	if (DotProduct > FMath::Sin((90 - (EyeSightAngle / 2.0f)) * 0.0174533f))
	{
		FVector EyesGlobalPosition = GetOwner()->GetTransform().TransformPosition(EyesPosition);

		FVector GlobalDirection = TargetGlobalPosition - EyesGlobalPosition;
		GlobalDirection.Normalize();

		float GlobalDistance = FVector::Dist(EyesGlobalPosition, TargetGlobalPosition);

		FVector Start = EyesGlobalPosition;
		FVector ForwardVector = GlobalDirection;
		FVector End = Start + (ForwardVector * GlobalDistance);

		FHitResult OutHit;

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());

		bool bIsHit = GetWorld()->LineTraceSingleByChannel(
			OutHit,
			Start,
			End,
			ECC_Visibility,
			CollisionParams
		);
		if (bIsHit)
		{
			if (OutHit.GetActor() != TargetActor)
			{
				return false;
			}
		}

		return true;
	}
	return false;
}


// Called every frame
void UEnemyAIPerception::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckPerception(DeltaTime);

#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	DebugLines();
#endif
}

void UEnemyAIPerception::CheckPerception(float DeltaTime)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectQueryParams;

	FVector EyesPositionWorldSpace = GetOwner()->GetTransform().TransformPosition(EyesPosition);

	// Llamar a SphereOverlapActors para llenar OutActors con los actores dentro de la esfera
	UKismetSystemLibrary::SphereOverlapActors(
		this, EyesPositionWorldSpace, EyeSightRadius, ObjectQueryParams, AActor::StaticClass(), ActorsToIgnore,
		OutActors
	);

	// int numb = OutActors.Num();
	// UE_LOG(LogTemp, Warning, TEXT("OutActors: %i"), numb);


	for (int i = 0; i < OutActors.Num(); i++)
	{
		AActor* Actor = OutActors[i];
		for (int h = 0; h < ActorFilter.Num(); h++)
		{
			if (Actor->IsA(ActorFilter[h]))
			{
				if (!PerceivedActors.Contains(Actor))
				{
					// Tenemos un actor dentro del radio, comprobamos si lo estamos detectando
					if (CheckIsInSight(Actor))
					{
						FString ActorName = Actor->GetName();
						UE_LOG(LogTemp, Warning, TEXT("Añadimos Actor Percibido: %s" ), *ActorName);
						PerceivedActors.Add(Actor);

						OnActorPerceivedDelegate.Broadcast(Actor);
					}
				}
			}
		}
	}

	for (int i = 0; i < PerceivedActors.Num(); i++)
	{
		AActor* PerceivedActor = PerceivedActors[i];
		if (!OutActors.Contains(PerceivedActor) || !CheckIsInSight(PerceivedActor))
		{
			FString ActorName = PerceivedActor->GetName();
			UE_LOG(LogTemp, Warning, TEXT("Quitamos Actor Percibido: %s" ), *ActorName);

			// Le hemos dejado de percibir
			PerceivedActors.RemoveAt(i);
			i--;

			OnActorUnperceivedDelegate.Broadcast(PerceivedActor);
		}
	}
}

void UEnemyAIPerception::OnComponentCreated()
{
	Super::OnComponentCreated();
}

void UEnemyAIPerception::DebugLines(float Duration)
{
	UWorld* World = GetWorld();
	FVector EyesPositionGlobal = GetOwner()->GetTransform().TransformPosition(EyesPosition);

	// Circulo de visión
	// DrawDebugCircle(World, GetOwner()->GetActorLocation(), EyeSightRadius, 20, FColor::White, false, Duration, 0, 0.0f,
	//                 GetOwner()->GetActorForwardVector(), GetOwner()->GetActorRightVector(), false
	// );

	// Arco de visión
	float AngleInRadians = EyeSightAngle * UE_PI / 180.0f;
	DrawDebugCircleArc(World, EyesPositionGlobal, EyeSightRadius, GetOwner()->GetActorForwardVector(),
	                   AngleInRadians / 2, 20, FColor::White, false, Duration);

	FVector LookVectorDirRight = GetOwner()->GetTransform().TransformVector(
		FVector(FMath::Cos(AngleInRadians / 2), FMath::Sin(AngleInRadians / 2), 0.0f)) * EyeSightRadius;
	FVector LookVectorDirLeft = GetOwner()->GetTransform().TransformVector(
		FVector(FMath::Cos(AngleInRadians / 2), -FMath::Sin(AngleInRadians / 2), 0.0f)) * EyeSightRadius;

	DrawDebugLine(World, EyesPositionGlobal, EyesPositionGlobal + LookVectorDirRight, FColor::White, false, Duration, 0,
	              0);
	DrawDebugLine(World, EyesPositionGlobal, EyesPositionGlobal + LookVectorDirLeft, FColor::White, false, Duration, 0,
	              0);

	// float dotProduct = Vector3.Dot(forwardDir, groundDirectionToPlayer);
	// dotProduct > (Mathf.Sin((90 - angle) * 0.0174533f))
}
