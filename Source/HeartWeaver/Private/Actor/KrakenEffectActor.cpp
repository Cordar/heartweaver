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
	// TODO: Change this to apply a Gameplay Effect. For now, using const_cast as a hack!
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UKrakenAttributeSet* KrakenAttributeSet = Cast<UKrakenAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UKrakenAttributeSet::StaticClass()));

		UKrakenAttributeSet* MutableKrakenAttributeSet = const_cast<UKrakenAttributeSet*>(KrakenAttributeSet);
		MutableKrakenAttributeSet->SetHealth(KrakenAttributeSet->GetHealth() + 1.f);
		Destroy();
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

