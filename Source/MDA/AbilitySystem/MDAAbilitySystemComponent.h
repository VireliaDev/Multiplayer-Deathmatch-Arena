// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/MDAAbilitySet.h"
#include "MDAAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MDA_API UMDAAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMDAAbilitySystemComponent();
	void GiveAbilitySet(const FMDAGameAbilitySet& AbilitySet, UObject* SourceObject,
	                    FGameAbilitySetHandles& GrantedHandles);
	void TakeAbilitySet(FGameAbilitySetHandles& GrantedHandles);


	//Ability Activation and Input handling (LYRA BASED)
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
protected:
	TArray<FGameplayAbilitySpecHandle>
	InputPressedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle>
	InputReleasedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle>
	InputHeldSpecHandles;
	
};
