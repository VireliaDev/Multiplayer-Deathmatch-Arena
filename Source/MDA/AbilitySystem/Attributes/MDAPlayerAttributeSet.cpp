// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"
#include <Net/UnrealNetwork.h>

UMDAPlayerAttributeSet::UMDAPlayerAttributeSet():
	Health(100.f),
	MaxHealth(100.f)
{
}

void UMDAPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//Make sure max health cannot go below 1
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}

	//Make sure current health doesn't go below 0
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UMDAPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
}

void UMDAPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// REPNOTIFY_Always is required
	DOREPLIFETIME_CONDITION_NOTIFY(UMDAPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMDAPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

}

void UMDAPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& Old)
{

}

void UMDAPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& Old)
{

}
