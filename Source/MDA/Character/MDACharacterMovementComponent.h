// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MDACharacterMovementComponent.generated.h"


class UMDAAbilitySystemComponent;
class UAbilitySystemComponent;
struct FGameplayTag;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MDA_API UMDACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void TickCharacterPose(float DeltaTime) override;
public:
	
	//Begin play
	virtual void BeginPlay() override;
	
	
	//Movement State Gameplay Tag
	FGameplayTagContainer MovementStateTags;
	FGameplayTag CurrentLocomotionTag;
	void SyncMovementTags();
	FGameplayTag ComputeLocomotionTag() const;

	
	//Asc Cached Ptr
	TWeakObjectPtr<UAbilitySystemComponent> CharacterOwnerASC;
	UAbilitySystemComponent* GetCharacterOwnerASC();
	
	
	//Setting custom flags
	void RefreshMovementBlockedFlags();
	virtual bool ForcePositionUpdate(float DeltaTime) override;
protected:
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	class FMDASavedMove : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;

		uint8 bSavedWalkingBlocked : 1;
		uint8 bSavedJumpingBlocked : 1;

		virtual void Clear() override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
								FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	};
	class FMDANetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		typedef FNetworkPredictionData_Client_Character Super;

		FMDANetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);
		virtual FSavedMovePtr AllocateNewMove() override;
	};
	
	
	
	
	
	
	//Walking
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01"))
	float MinWalkSpeedThreshold = 10.f;
	virtual float GetMaxSpeed() const override;
	bool bIsWalkingBlocked = false;
	
	//Jumping
	virtual bool CanAttemptJump() const override;
	bool bIsJumpingBlocked = false;
	
};
