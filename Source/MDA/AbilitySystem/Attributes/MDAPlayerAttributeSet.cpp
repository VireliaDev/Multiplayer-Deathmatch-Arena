// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"
#include <Net/UnrealNetwork.h>

//Init values to something plain (Initial values should actually be set with a gameplay effect applied on spawn)
UMDAPlayerAttributeSet::UMDAPlayerAttributeSet():
	Health(100.f),
	MaxHealth(100.f),
	PrimaryAmmo(0),
	PrimaryReserveAmmo(0),
	SecondaryAmmo(0),
	SecondaryReserveAmmo(0)
{
}

void UMDAPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//Make sure health values don't go below their minimums
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	//Make sure all ammo values don't go below 0
	if (Attribute == GetPrimaryAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetPrimaryAmmo());
	}
	if (Attribute == GetPrimaryReserveAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetPrimaryReserveAmmo());
	}
	if (Attribute == GetSecondaryAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetSecondaryAmmo());
	}
	if (Attribute == GetSecondaryReserveAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetSecondaryReserveAmmo());
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
	DOREPLIFETIME_CONDITION_NOTIFY(UMDAPlayerAttributeSet, PrimaryAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMDAPlayerAttributeSet, PrimaryReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMDAPlayerAttributeSet, SecondaryAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMDAPlayerAttributeSet, SecondaryReserveAmmo, COND_None, REPNOTIFY_Always);

}

void UMDAPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMDAPlayerAttributeSet, Health, Old);
}

void UMDAPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMDAPlayerAttributeSet, MaxHealth, Old);
}

void UMDAPlayerAttributeSet::OnRep_PrimaryAmmo(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMDAPlayerAttributeSet, PrimaryAmmo, Old);
}

void UMDAPlayerAttributeSet::OnRep_PrimaryReserveAmmo(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMDAPlayerAttributeSet, PrimaryReserveAmmo, Old);
}

void UMDAPlayerAttributeSet::OnRep_SecondaryAmmo(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMDAPlayerAttributeSet, SecondaryAmmo, Old);
}

void UMDAPlayerAttributeSet::OnRep_SecondaryReserveAmmo(const FGameplayAttributeData& Old)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMDAPlayerAttributeSet, SecondaryReserveAmmo, Old);
}
