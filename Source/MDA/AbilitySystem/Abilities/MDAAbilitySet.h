#pragma once
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "MDAAbilitySet.generated.h"


USTRUCT(BlueprintType)
struct FGameAbilityEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMDAGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	/** Which input tag activates this. Leave empty for passives. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};



USTRUCT(BlueprintType)
struct FMDAGameAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FGameAbilityEntry> Abilities;

	/** Initialise attributes with these Effects (They are applied on ability granting). */
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
	
};


// Handles returned from granting, used for revoking on death / character swaps
USTRUCT()
struct FGameAbilitySetHandles
{
	GENERATED_BODY()

	UPROPERTY() 
	TArray<FGameplayAbilitySpecHandle>   AbilityHandles;
	UPROPERTY() 
	TArray<FActiveGameplayEffectHandle>  EffectHandles;
	

	bool IsValid() const
	{
		return AbilityHandles.Num() > 0 || EffectHandles.Num() > 0;
	}
	void Reset()
	{
		AbilityHandles.Reset(); 
		EffectHandles.Reset();
	}
	
};