// Copyright Kraken's Den Studios. All rights reserved.


#include "Characters/KDSEnemy.h"

#include "AbilitySystem/KDSAbilitySystemComponent.h"
#include "AbilitySystem/KDSAttributeSet.h"

AKDSEnemy::AKDSEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UKDSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UKDSAttributeSet>(TEXT("AttributeSet"));
}

void AKDSEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
