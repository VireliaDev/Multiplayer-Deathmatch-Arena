// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "MDA/AbilitySystem/Abilities/MDAGameplayAbility.h"
#include "MDAGameplayAbility_Reload.generated.h"

/**
 * 
 */
UCLASS()
class MDA_API UMDAGameplayAbility_Reload : public UMDAGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMDAGameplayAbility_Reload();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

	UPROPERTY(EditDefaultsOnly, Category = "MDA|Reload")
	UAnimMontage* ReloadMontage = nullptr;
	
private:
	UFUNCTION()
	void OnReloadFinished();
	
	UFUNCTION()
	void OnMantle();
	
};

