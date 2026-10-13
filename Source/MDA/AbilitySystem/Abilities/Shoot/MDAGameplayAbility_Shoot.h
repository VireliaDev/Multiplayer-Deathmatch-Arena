// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "MDA/AbilitySystem/Abilities/MDAGameplayAbility.h"
#include "MDAGameplayAbility_Shoot.generated.h"


UCLASS()
class MDA_API UMDAGameplayAbility_Shoot : public UMDAGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMDAGameplayAbility_Shoot();
	
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	
	// Placeholder pacing until the weapon system owns fire rate.
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Fire")
	float TestShotInterval = 0.5f;

private:
	UFUNCTION()
	void OnShotIntervalFinished();
	
	UFUNCTION()
	void OnMantle();
	
};
