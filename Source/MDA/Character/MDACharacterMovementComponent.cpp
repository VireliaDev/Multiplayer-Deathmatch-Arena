// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
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
	//skip ticking the character pose to avoid laggy animation 
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


/**
 * Compute the new locomotion tag,
 * if it's the same as the last tag don't do anything.
 * If it's different, reset all the currently applied movement tags from the ASC
 * and then add the new one, and store is as the current tag locally 
*/
void UMDACharacterMovementComponent::SyncMovementTags()
{
	UAbilitySystemComponent* ASC = GetCharacterOwnerASC();
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
		if (Velocity.SizeSquared2D() > FMath::Square(MinWalkSpeedThreshold))
		{
			return GameTags::Movement_State_Walking;
		}
		return GameTags::Movement_State_Idle;
	}
		
	//If they aren't falling, moving on the ground, they must be in an unsupported state. //Swimming, flying, MOVE_None
	return FGameplayTag();
}



UAbilitySystemComponent* UMDACharacterMovementComponent::GetCharacterOwnerASC()
{
	if (CharacterOwnerASC.IsValid())
	{
		return CharacterOwnerASC.Get();
	}
	CharacterOwnerASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner.Get());
	return CharacterOwnerASC.Get();
}

void UMDACharacterMovementComponent::RefreshMovementBlockedFlags()
{
	UAbilitySystemComponent* ASC = GetCharacterOwnerASC();
	if (!ASC)
	{
		bIsWalkingBlocked = false;
		bIsJumpingBlocked = false;
		return;
	}
	
	bIsWalkingBlocked = ASC->HasMatchingGameplayTag(GameTags::Movement_Blocked_Walking);
	bIsJumpingBlocked = ASC->HasMatchingGameplayTag(GameTags::Movement_Blocked_Jumping);
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

void UMDACharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags,
	const FVector& NewAccel)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RefreshMovementBlockedFlags();
	}
	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
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
}

void UMDACharacterMovementComponent::FMDASavedMove::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	if (const UMDACharacterMovementComponent* CMC = Cast<UMDACharacterMovementComponent>(C->GetCharacterMovement()))
	{
		bSavedWalkingBlocked = CMC->bIsWalkingBlocked;
		bSavedJumpingBlocked = CMC->bIsJumpingBlocked;
	}
}

void UMDACharacterMovementComponent::FMDASavedMove::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	if (UMDACharacterMovementComponent* CMC = Cast<UMDACharacterMovementComponent>(C->GetCharacterMovement()))
	{
		CMC->bIsWalkingBlocked = bSavedWalkingBlocked;
		CMC->bIsJumpingBlocked = bSavedJumpingBlocked;
	}
}

bool UMDACharacterMovementComponent::FMDASavedMove::CanCombineWith(const FSavedMovePtr& NewMove,
	ACharacter* InCharacter, float MaxDelta) const
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
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

UMDACharacterMovementComponent::FMDANetworkPredictionData_Client::FMDANetworkPredictionData_Client(
	const UCharacterMovementComponent& ClientMovement) : Super (ClientMovement)
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
	return Super::GetMaxSpeed();
}

bool UMDACharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() && !bIsJumpingBlocked;
}








