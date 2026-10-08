// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MDACharacterMovementComponent.generated.h"


class AMDACharacter;
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
	
	
	//Cached Character Ptr
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<AMDACharacter> MDACharacterOwner;
	
	virtual void PostLoad() override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	
	//ASC Getter
	UAbilitySystemComponent* GetMDACharacterOwnerASC() const;
	
	
	//Movement state reporting with tags
	FGameplayTagContainer MovementStateTags;
	FGameplayTag CurrentLocomotionTag;
	void SyncMovementTags();
	FGameplayTag ComputeLocomotionTag() const;
	
	
	//Setting custom flags
	void RefreshMovementBlockedFlags();
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	virtual bool ForcePositionUpdate(float DeltaTime) override;
protected:
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	virtual bool ClientUpdatePositionAfterServerUpdate() override;
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	class FMDASavedMove : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;

		uint8 bSavedWalkingBlocked : 1;
		uint8 bSavedJumpingBlocked : 1;
		uint8 bSavedSprintingBlocked : 1;
		
		uint8 bSavedWantsToSprint : 1;

		virtual void Clear() override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
								FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		
		virtual uint8 GetCompressedFlags() const override;
	};
	class FMDANetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		typedef FNetworkPredictionData_Client_Character Super;

		FMDANetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);
		virtual FSavedMovePtr AllocateNewMove() override;
	};
protected:
	//Getting custom flags
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	
	
public:
	//For walking & sprinting
	virtual float GetMaxSpeed() const override;

	
	//Walking
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01"))
	float MinWalkSpeedThreshold = 10.f;
	bool bIsWalkingBlocked = false;
	
	
	//Jumping
	virtual bool CanAttemptJump() const override;
	bool bIsJumpingBlocked = false;
	
	
	//Sprinting
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0"))
	float MaxSprintSpeed = 600.f ;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "-1", ClampMax = "1"))
	float SprintForwardDotThreshold = 0.5f; 
	bool bIsSprintingBlocked = false;
	bool bWantsToSprint = false;
	bool CanSprintInCurrentState() const;
	bool IsSprinting() const;
	
};
