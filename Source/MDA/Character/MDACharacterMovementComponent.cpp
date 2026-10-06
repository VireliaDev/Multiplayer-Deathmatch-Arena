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

void UMDACharacterMovementComponent::SyncMovementTags()
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner.Get());
	if (!ASC) return;
	
	const FGameplayTag NewTag = ComputeLocomotionTag();
	
	if (NewTag == CurrentLocomotionTag) return;
	
	if (CurrentLocomotionTag.IsValid())
	{
		ASC->SetLooseGameplayTagCount(CurrentLocomotionTag, 0, EGameplayTagReplicationState::None);
	}
	
	if (NewTag.IsValid())
	{
		ASC->SetLooseGameplayTagCount(NewTag, 1, EGameplayTagReplicationState::None);
	}
	
	CurrentLocomotionTag = NewTag;
}

void UMDACharacterMovementComponent::ResetMovementTags(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;
	
	CurrentLocomotionTag = FGameplayTag();
	
	const FGameplayTagContainer StateTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(GameTags::Movement_State);
	for (const FGameplayTag& Tag : StateTags)
	{
		ASC->SetLooseGameplayTagCount(Tag, 0, EGameplayTagReplicationState::None);
	}
}


float UMDACharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovingOnGround() && IsWalkingBlocked())
	{
		return 0.f;
	}
	return Super::GetMaxSpeed();
}

bool UMDACharacterMovementComponent::IsWalkingBlocked() const
{
	return false;
}

