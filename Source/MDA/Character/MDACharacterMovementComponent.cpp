// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "MDACharacter.h"
#include "GameFramework/Character.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"


void UMDACharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SyncMovementTags();
}

void UMDACharacterMovementComponent::TickCharacterPose(float DeltaTime)
{
	//If this character is remote and on the listen server,
	//and has bOnlyAllowAutonomousTickPose set to false
	//skip ticking the character pose to prevent double ticking
	//(pose ticking is handled automatically now with bOnlyAllowAutonomousTickPose being false).
	if (GetNetMode() == NM_ListenServer && !GetCharacterOwner()->IsLocallyControlled())
	{
		if (USkeletalMeshComponent* CharMesh = GetCharacterOwner()->GetMesh())
		{
			if (!CharMesh->bOnlyAllowAutonomousTickPose)
			{
				return;
			}
		}
	}
	
	Super::TickCharacterPose(DeltaTime);
}

void UMDACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	MovementStateTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GameTags::Movement_State);
}

void UMDACharacterMovementComponent::PostLoad()
{
	//Set Character Owner
	Super::PostLoad();
	
	//Set MDACharacterOwner using CharacterOwner
	MDACharacterOwner = Cast<AMDACharacter>(CharacterOwner);
}

void UMDACharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	//Set Character Owner
	Super::SetUpdatedComponent(NewUpdatedComponent);
	
	//Set MDACharacterOwner using CharacterOwner
	MDACharacterOwner = Cast<AMDACharacter>(CharacterOwner);
}

UAbilitySystemComponent* UMDACharacterMovementComponent::GetMDACharacterOwnerASC() const
{
	if (MDACharacterOwner)
	{
		return MDACharacterOwner->GetAbilitySystemComponent();
	}
	return nullptr;
}


/**
 * Compute the new locomotion tag,
 * if it's the same as the last tag don't do anything.
 * If it's different, reset all the currently applied movement tags from the ASC
 * and then add the new one, and store is as the current tag locally 
 */
void UMDACharacterMovementComponent::SyncMovementTags()
{
	UAbilitySystemComponent* ASC = GetMDACharacterOwnerASC();
	if (!ASC) return;
	
	//Compute the new tag, and check if it's the same as the last one
	const FGameplayTag NewTag = ComputeLocomotionTag();
	if (NewTag == CurrentLocomotionTag) return;
	
	
	//Reset all the currently applied tags
	for (const FGameplayTag& Tag : MovementStateTags)
	{
		if (Tag.IsValid())
		{
			ASC->SetLooseGameplayTagCount(Tag, 0, EGameplayTagReplicationState::None);
		}
	}
	
	//Add the new the tag to the ASC
	if (NewTag.IsValid())
	{
		ASC->SetLooseGameplayTagCount(NewTag, 1, EGameplayTagReplicationState::None);
	}
	
	CurrentLocomotionTag = NewTag;
}

FGameplayTag UMDACharacterMovementComponent::ComputeLocomotionTag() const
{
	if (IsFalling())
	{
		return GameTags::Movement_State_Falling;
	}
	
	if (IsMovingOnGround())
	{
		if (IsSprinting())
		{
			return GameTags::Movement_State_Sprinting;
		}
		if (Velocity.SizeSquared2D() > FMath::Square(MinWalkSpeedThreshold))
		{
			return GameTags::Movement_State_Walking;
		}
		return GameTags::Movement_State_Idle;
	}
		
	//If they aren't falling, moving on the ground, they must be in an unsupported state. //Swimming, flying, MOVE_None
	return FGameplayTag();
}

void UMDACharacterMovementComponent::RefreshMovementBlockedFlags()
{
	UAbilitySystemComponent* ASC = GetMDACharacterOwnerASC();
	if (!ASC)
	{
		bIsWalkingBlocked = false;
		bIsJumpingBlocked = false;
		bIsSprintingBlocked = false;
		return;
	}
	
	bIsWalkingBlocked = ASC->HasMatchingGameplayTag(GameTags::Movement_Blocked_Walking);
	bIsJumpingBlocked = ASC->HasMatchingGameplayTag(GameTags::Movement_Blocked_Jumping);
	bIsSprintingBlocked = ASC->HasMatchingGameplayTag(GameTags::Movement_Blocked_Sprinting);
}

void UMDACharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	////Check crouch////
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
	
	////Check sprint////
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy) // Proxies get replicated sprint state.
	{
		if (MDACharacterOwner)
		{
			MDACharacterOwner->SetIsSprinting(bWantsToSprint && CanSprintInCurrentState());
		}
	}
}

void UMDACharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	////Check crouch////
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	
	////Check sprint////
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy) // Proxies get replicated sprint state.
	{
		if (!bWantsToSprint || !CanSprintInCurrentState())
		{
			if (MDACharacterOwner)
			{
				MDACharacterOwner->SetIsSprinting(false);
			}
		}
	}
}

bool UMDACharacterMovementComponent::ForcePositionUpdate(float DeltaTime)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RefreshMovementBlockedFlags();
	}
	return Super::ForcePositionUpdate(DeltaTime);
}

void UMDACharacterMovementComponent::ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds)
{
	RefreshMovementBlockedFlags();
	Super::ControlledCharacterMove(InputVector, DeltaSeconds);
}

void UMDACharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RefreshMovementBlockedFlags();
	}
	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}

bool UMDACharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	//Stash the current intent
	const bool bRealSprint = bWantsToSprint;
	//Perform the predicted moves post server update
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();
	//Restore the original intent
	bWantsToSprint = bRealSprint;
	return bResult;
}

void UMDACharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

FNetworkPredictionData_Client* UMDACharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UMDACharacterMovementComponent* MutableThis = const_cast<UMDACharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FMDANetworkPredictionData_Client(*this);
	}
	return ClientPredictionData;
}

void UMDACharacterMovementComponent::FMDASavedMove::Clear()
{
	Super::Clear();
	bSavedWalkingBlocked = false;
	bSavedJumpingBlocked = false;
	bSavedSprintingBlocked = false;
	
	bSavedWantsToSprint = false;
}

void UMDACharacterMovementComponent::FMDASavedMove::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	if (const UMDACharacterMovementComponent* CMC = Cast<UMDACharacterMovementComponent>(C->GetCharacterMovement()))
	{
		bSavedWalkingBlocked = CMC->bIsWalkingBlocked;
		bSavedJumpingBlocked = CMC->bIsJumpingBlocked;
		bSavedSprintingBlocked = CMC->bIsSprintingBlocked;
		
		bSavedWantsToSprint = CMC->bWantsToSprint;
	}
}

void UMDACharacterMovementComponent::FMDASavedMove::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	if (UMDACharacterMovementComponent* CMC = Cast<UMDACharacterMovementComponent>(C->GetCharacterMovement()))
	{
		CMC->bIsWalkingBlocked = bSavedWalkingBlocked;
		CMC->bIsJumpingBlocked = bSavedJumpingBlocked;
		CMC->bIsSprintingBlocked = bSavedSprintingBlocked;
		
		CMC->bWantsToSprint = bSavedWantsToSprint;
	}
}

bool UMDACharacterMovementComponent::FMDASavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FMDASavedMove* NewMDAMove = static_cast<const FMDASavedMove*>(NewMove.Get());
	if (bSavedWalkingBlocked != NewMDAMove->bSavedWalkingBlocked)
	{
		return false;
	}
	if (bSavedJumpingBlocked != NewMDAMove->bSavedJumpingBlocked)
	{
		return false;
	}
	if (bSavedSprintingBlocked != NewMDAMove->bSavedSprintingBlocked)
	{
		return false;
	}
	
	//Movement intent is in compressed flags, which get evaluated in Super::CanCombineWith() already
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

uint8 UMDACharacterMovementComponent::FMDASavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	
	if (bSavedWantsToSprint) Result |= FLAG_Custom_0;
	
	return Result;
}

UMDACharacterMovementComponent::FMDANetworkPredictionData_Client::FMDANetworkPredictionData_Client( const UCharacterMovementComponent& ClientMovement) : Super (ClientMovement)
{
}

FSavedMovePtr UMDACharacterMovementComponent::FMDANetworkPredictionData_Client::AllocateNewMove()
{
	return MakeShared<FMDASavedMove>();
}

float UMDACharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovingOnGround() && bIsWalkingBlocked)
	{
		return 0.f;
	}
	
	if (IsMovingOnGround() && IsSprinting())
	{
		return MaxSprintSpeed;
	}
	
	return Super::GetMaxSpeed();
}

bool UMDACharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() && !bIsJumpingBlocked;
}

bool UMDACharacterMovementComponent::CanSprintInCurrentState() const
{
	if (!UpdatedComponent)
	{
		return false;
	}
	if (!UpdatedComponent->IsSimulatingPhysics())
	{
		return false;
	}
	
	//Check for any actual movement
	const bool bIsMoving = Velocity.SizeSquared2D() > FMath::Square(MinWalkSpeedThreshold);
	
	//Check the movement is forward within the threshold (to detemine what forward is)
	const float ForwardDot = FVector::DotProduct(Acceleration.GetSafeNormal2D(), UpdatedComponent->GetForwardVector());
	const bool bIsMovingForward = ForwardDot >= SprintForwardDotThreshold;
	
	return IsMovingOnGround() && bIsMoving && bIsMovingForward && !IsCrouching() && !bIsSprintingBlocked;
}

bool UMDACharacterMovementComponent::IsSprinting() const
{
	return MDACharacterOwner && MDACharacterOwner->IsSprinting();
}








