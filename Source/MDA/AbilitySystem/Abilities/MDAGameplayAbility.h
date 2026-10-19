// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MDAGameplayAbility.generated.h"


class AMDACharacter;
class UMDACharacterMovementComponent;

UENUM(BlueprintType)
enum class EMDAGameAbilityActivationPolicy : uint8
{
	OnInputTriggered,   // For abilities that instant fire
	WhileInputActive,   // For abilities that require holding down
	OnSpawn             // For passives
};

UCLASS()
class MDA_API UMDAGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	

	
public:
	UMDAGameplayAbility();
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EMDAGameAbilityActivationPolicy ActivationPolicy;
	
	
	//GAS equivalent of BeginPlay
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	
	//Helpers
	static UMDACharacterMovementComponent* GetMDAMovementFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo);
	static AMDACharacter* GetMDACharacterFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo);
};
