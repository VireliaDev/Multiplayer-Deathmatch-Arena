// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacterMovementComponent.h"


UMDACharacterMovementComponent::UMDACharacterMovementComponent()
{  
	SetNetworkMoveDataContainer(MDAMoveDataContainer);
	
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
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

void UMDACharacterMovementComponent::SetSprintHeld(bool bHeld)
{
	bSprintHeld = bHeld;
	if (bHeld)
	{
		bSprintAfterCrouch = true;
		bSprintAfterAim = true;
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
	float Speed = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Speed *= SprintSpeedMultiplier;
	}
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
			bSprintWins = bIsSprinting && !bWeaponSuppress;   // airborne: keep takeoff state
		}
		else
		{
			bSprintWins = bSprintHeld && IsForwardInput() && !bWeaponSuppress;
		}
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (!bSimulatedProxy)
	{
		bIsSprinting = bSprintWins;
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
	return bResult;
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
