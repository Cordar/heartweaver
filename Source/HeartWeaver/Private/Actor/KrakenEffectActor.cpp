// Copyright Kraken's Den Studios. All rights reserved.


#include "Actor/KrakenEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/KrakenAttributeSet.h"
#include "Components/SphereComponent.h"

AKrakenEffectActor::AKrakenEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AKrakenEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UKrakenAttributeSet* KrakenAttributeSet = Cast<UKrakenAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UKrakenAttributeSet::StaticClass()));
	}
}

void AKrakenEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AKrakenEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AKrakenEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AKrakenEffectActor::OnEndOverlap);
}

