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

uint8 UMDACharacterMovementComponent::PackIntents() const
{
	uint8 Packed = 0;
	if (bSprintHeld)        { Packed |= EMDAIntent::SprintHeld; }
	if (bCrouchHeld)        { Packed |= EMDAIntent::CrouchHeld; }
	if (bAimHeld)           { Packed |= EMDAIntent::AimHeld; }
	if (bJumpHeld)          { Packed |= EMDAIntent::JumpHeld; }
	if (bSprintAfterCrouch) { Packed |= EMDAIntent::SprintAfterCrouch; }
	if (bSprintAfterAim)    { Packed |= EMDAIntent::SprintAfterAim; }
	if (bWeaponSuppress)    { Packed |= EMDAIntent::WeaponSuppress; }
	return Packed;
}

void UMDACharacterMovementComponent::UnpackIntents(uint8 Packed)
{
	bSprintHeld        = (Packed & EMDAIntent::SprintHeld) != 0;
	bCrouchHeld        = (Packed & EMDAIntent::CrouchHeld) != 0;
	bAimHeld           = (Packed & EMDAIntent::AimHeld) != 0;
	bJumpHeld          = (Packed & EMDAIntent::JumpHeld) != 0;
	bSprintAfterCrouch = (Packed & EMDAIntent::SprintAfterCrouch) != 0;
	bSprintAfterAim    = (Packed & EMDAIntent::SprintAfterAim) != 0;
	bWeaponSuppress    = (Packed & EMDAIntent::WeaponSuppress) != 0;
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
			bSprintWins = bIsSprinting && !bWeaponSuppress && (!bAimHeld || bSprintAfterAim);
			bWantsToCrouch = false;
			bIsAiming = bAimHeld && !bSprintWins;
		}
		else
		{
			bSprintWins = bSprintHeld && IsForwardInput() && !bWeaponSuppress && (!bCrouchHeld || bSprintAfterCrouch) && (!bAimHeld || bSprintAfterAim);
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
