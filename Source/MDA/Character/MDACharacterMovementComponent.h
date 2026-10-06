// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MDACharacterMovementComponent.generated.h"


class UAbilitySystemComponent;
struct FGameplayTag;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MDA_API UMDACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	//Movement State Gameplay Tag
	FGameplayTag CurrentLocomotionTag;
	FGameplayTag ComputeLocomotionTag() const;
	void SyncMovementTags();
	void ResetMovementTags(UAbilitySystemComponent* ASC);
	
	
	//Walking
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01"))
	float MinWalkSpeedThreshold = 10.f;
	virtual float GetMaxSpeed() const override;
	bool IsWalkingBlocked() const;
	
	
	
};
