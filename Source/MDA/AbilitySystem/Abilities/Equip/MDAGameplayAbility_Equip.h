// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "MDA/AbilitySystem/Abilities/MDAGameplayAbility.h"
#include "MDA/Weapons/MDAWeaponAsset.h"
#include "MDAGameplayAbility_Equip.generated.h"

/**
 * 
 */
UCLASS()
class MDA_API UMDAGameplayAbility_Equip : public UMDAGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMDAGameplayAbility_Equip();
	
	
	UPROPERTY(EditDefaultsOnly, Category= "Target Slot")
	EWeaponSlot TargetSlot = EWeaponSlot::None;
	
protected:
	UFUNCTION()
	void OnMontageOver();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
	void OnPredictionRejected();
	
};
