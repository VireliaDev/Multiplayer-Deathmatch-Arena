// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "MDACharacter.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Framework/Player/MDAPlayerState.h"


UMDACharacterMovementComponent::UMDACharacterMovementComponent()
{  
	SetNetworkMoveDataContainer(MDAMoveDataContainer);
	
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
	
	NavAgentProps.bCanCrouch = true;
}

void UMDACharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	bHasReplicatedAcceleration = true;
	Acceleration = InAcceleration;
}

void UMDACharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		//prevent sim proxies from destroying the acceleration in simulation
		const FVector SavedAcceleration = Acceleration;
		Super::SimulateMovement(DeltaTime);
		Acceleration = SavedAcceleration;
	}
	else
	{
		Super::SimulateMovement(DeltaTime);
	}
}

void UMDACharacterMovementComponent::TickCharacterPose(float DeltaTime)
{
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

UAbilitySystemComponent* UMDACharacterMovementComponent::GetASC() const
{
	const AMDAPlayerState* PS = CharacterOwner ? CharacterOwner->GetPlayerState<AMDAPlayerState>() : nullptr;
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

void UMDACharacterMovementComponent::UpdateStateTags()
{
	if (CharacterOwner->bClientUpdating)
	{
		return;   // Do not update state tags during resimulation
	}

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Sprinting, bIsSprinting ? 1 : 0);
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Crouching, IsCrouching() ? 1 : 0);
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Aiming,    bIsAiming ? 1 : 0);
	}

	if (CharacterOwner->HasAuthority())
	{
		CastChecked<AMDACharacter>(CharacterOwner)->SetReplicatedMovementState(bIsSprinting, bIsAiming);
	}
}

float UMDACharacterMovementComponent::GetMaxWalkSpeed() const
{
	return MaxWalkSpeed;
}

float UMDACharacterMovementComponent::GetMaxSprintSpeed() const
{
	return MaxWalkSpeed * SprintSpeedMultiplier;
}

void UMDACharacterMovementComponent::SetSprintHeld(const bool bHeld)
{
	bSprintHeld = bHeld;
	if (bHeld)
	{
		//Sprint is now the latest input
		bSprintAfterCrouch = true; 
		bSprintAfterAim = true;
	}
}

void UMDACharacterMovementComponent::SetCrouchHeld(const bool bHeld)
{
	bCrouchHeld = bHeld;
	if (bHeld)
	{
		bSprintAfterCrouch = false;   //Crouch is now the latest input
	}
}

void UMDACharacterMovementComponent::SetAimHeld(const bool bHeld)
{
	bAimHeld = bHeld;
	if (bHeld)
	{
		bSprintAfterAim = false;
	}
}

void UMDACharacterMovementComponent::SetWantsToFire(bool bWants)
{
	bWantsToFire = bWants;
	bIsWeaponBusy = bWantsToFire || bIsReloading;
}

void UMDACharacterMovementComponent::SetIsReloading(bool bBusy)
{
	bIsReloading = bBusy;
	bIsWeaponBusy = bWantsToFire || bIsReloading;
}

uint8 UMDACharacterMovementComponent::PackIntents() const
{
	uint8 Packed = 0;
	if (bSprintHeld)        { Packed |= SprintHeld; }
	if (bCrouchHeld)        { Packed |= CrouchHeld; }
	if (bAimHeld)           { Packed |= AimHeld; }
	if (bJumpHeld)          { Packed |= JumpHeld; }
	if (bSprintAfterCrouch) { Packed |= SprintAfterCrouch; }
	if (bSprintAfterAim)    { Packed |= SprintAfterAim; }
	if (bIsWeaponBusy)      { Packed |= WeaponSuppress; }
	return Packed;
}

void UMDACharacterMovementComponent::UnpackIntents(uint8 Packed)
{
	bSprintHeld        = (Packed & SprintHeld) != 0;
	bCrouchHeld        = (Packed & CrouchHeld) != 0;
	bAimHeld           = (Packed & AimHeld) != 0;
	bJumpHeld          = (Packed & JumpHeld) != 0;
	bSprintAfterCrouch = (Packed & SprintAfterCrouch) != 0;
	bSprintAfterAim    = (Packed & SprintAfterAim) != 0;
	bIsWeaponBusy    =   (Packed & WeaponSuppress) != 0;
}

float UMDACharacterMovementComponent::GetMaxSpeed() const
{
	float Speed = Super::GetMaxSpeed();     //Base already handles crouched speed
	if (bIsSprinting)    { Speed *= SprintSpeedMultiplier; }
	else if (bIsAiming)  { Speed *= AimSpeedMultiplier; }
	return Speed;
}

void UMDACharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	const bool bSimulatedProxy = CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy;
	bool bSprintWins = false;

	if (!bSimulatedProxy)
	{
		if (IsFalling())
		{
			bSprintWins = bIsSprinting && !bIsWeaponBusy && (!bAimHeld || bSprintAfterAim);
			bWantsToCrouch = false;
			bIsAiming = bAimHeld && !bSprintWins;
		}
		else
		{
			bSprintWins = bSprintHeld && IsForwardInput() && !bIsWeaponBusy && (!bCrouchHeld || bSprintAfterCrouch) && (!bAimHeld || bSprintAfterAim);
			bWantsToCrouch = bCrouchHeld && !bSprintWins;
			bIsAiming = bAimHeld && !bSprintWins;
		}
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (!bSimulatedProxy)
	{
		bIsSprinting = bSprintWins && !IsCrouching();
		UpdateStateTags();
	}
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

void UMDACharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel)
{
	if (const FMDANetworkMoveData* MoveData = static_cast<const FMDANetworkMoveData*>(GetCurrentNetworkMoveData()))
	{
		UnpackIntents(MoveData->Intents);
	}
	Super::MoveAutonomous(ClientTimeStamp, DeltaTime, CompressedFlags, NewAccel);
}

bool UMDACharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	const uint8 LiveIntents = PackIntents();
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();
	UnpackIntents(LiveIntents);
	UpdateStateTags();
	return bResult;
}

bool UMDACharacterMovementComponent::CanAttemptJump() const
{
	//Remove bWantsToCrouch check to allow jumping from crouched
	return IsJumpAllowed() && (IsMovingOnGround() || IsFalling());
}

bool UMDACharacterMovementComponent::IsForwardInput() const
{
	if (Acceleration.IsNearlyZero())
	{
		return false;
	}
	const FVector InputDir = Acceleration.GetSafeNormal2D();
	const FVector Facing = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	return FVector::DotProduct(InputDir, Facing) > SprintForwardThreshold;
}
