// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerState.h"

#include "AbilitySystemComponent.h"
#include "MDA/AbilitySystem/MDAAbilitySystemComponent.h"

AMDAPlayerState::AMDAPlayerState()
{
	CreateAbilitySystemComponent();
}

void AMDAPlayerState::CreateAbilitySystemComponent()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMDAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	SetNetUpdateFrequency(100.f); //Lyra standard
}

UAbilitySystemComponent* AMDAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}
