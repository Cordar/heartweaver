// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KrakenEnemy.h"

#include "AbilitySystem/KrakenAbilitySystemComponent.h"
#include "AbilitySystem/KrakenAttributeSet.h"

AKrakenEnemy::AKrakenEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UKrakenAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UKrakenAttributeSet>(TEXT("AttributeSet"));
}

void AKrakenEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
