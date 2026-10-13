// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "../MDACharacter.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "Components/CapsuleComponent.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Framework/Player/MDAPlayerState.h"
#include "RootMotion/RootMotionSource_MDAMantle.h"


UMDACharacterMovementComponent::UMDACharacterMovementComponent()
{  
	SetNetworkMoveDataContainer(MDAMoveDataContainer);
	
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
	
	NavAgentProps.bCanCrouch = true;
}

void UMDACharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SprintRecoveryEndTime > 0.f && GetWorld()->GetTimeSeconds() >= SprintRecoveryEndTime)
	{
		SprintRecoveryEndTime = 0.f;
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			ASC->SetLooseGameplayTagCount(GameTags::Movement_State_SprintRecovery, 0);
		}
	}
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
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Sprinting, bIsSprinting ? 1 : 0, EGameplayTagReplicationState::None);
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Crouching, IsCrouching() ? 1 : 0, EGameplayTagReplicationState::None);
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Aiming, bIsAiming ? 1 : 0, EGameplayTagReplicationState::None);
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_Mantling, IsMantling() ? 1 : 0, EGameplayTagReplicationState::None);
	}

	if (CharacterOwner->HasAuthority())
	{
		CastChecked<AMDACharacter>(CharacterOwner)->SetReplicatedMovementState(bIsSprinting, bIsAiming, IsMantling());
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

void UMDACharacterMovementComponent::SetJumpHeld(bool bHeld)
{
	bJumpHeld = bHeld;
}

bool UMDACharacterMovementComponent::IsMantling() const
{
	for (const TSharedPtr<FRootMotionSource>& Source : CurrentRootMotion.RootMotionSources)
	{
		if (Source.IsValid() && Source->InstanceName == MDAMantleSourceName)
		{
			return true;
		}
	}
	return false;
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
	
	if (IsMantling()) //No modifiers allowed during a mantle
	{
		return Speed;
	}
	
	if (bIsSprinting)
	{
		Speed *= SprintSpeedMultiplier;
	}
	else if (bIsAiming)
	{
		Speed *= AimSpeedMultiplier;
	}
	
	return Speed;
}

void UMDACharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	const bool bSimulatedProxy = CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy;
	bool bSprintWins = false;
	const bool bWasSprinting = bIsSprinting;

	if (!bSimulatedProxy)
	{
		//Constantly check for mantling
		TryStartMantle();
		
		if (IsMantling()) // Mantling overrides other movement
		{
			bWantsToCrouch = false;
			
		}
		else if (IsFalling())
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

	if (!bSimulatedProxy)//If client or server only 
	{
		bIsSprinting = bSprintWins && !IsCrouching();
		
		if (bWasSprinting && !bIsSprinting)
		{
			OnSprintEnded();
		}
		
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

void UMDACharacterMovementComponent::OnSprintEnded()
{
	if (CharacterOwner->bClientUpdating)
	{
		return;   //Skip this during resimulation
	}

	const float Duration = CharacterOwner->IsLocallyControlled() ? ClientSprintRecoveryTime : ServerSprintRecoveryTime;
	SprintRecoveryEndTime = GetWorld()->GetTimeSeconds() + Duration;

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		ASC->SetLooseGameplayTagCount(GameTags::Movement_State_SprintRecovery, 1);
	}
}

void UMDACharacterMovementComponent::TryStartMantle()
{
	if (IsMantling() || !IsFalling() || !bJumpHeld || !IsForwardInput())
	{
		return;
	}

	FVector Target;
	if (!FindMantleTarget(Target))
	{
		return;
	}
	

	const FVector Start = UpdatedComponent->GetComponentLocation();

	TSharedPtr<FRootMotionSource_MDAMantle> Move = MakeShared<FRootMotionSource_MDAMantle>();
	Move->InstanceName = MDAMantleSourceName;
	Move->AccumulateMode = ERootMotionAccumulateMode::Override;
	Move->Duration = MantleDuration;
	Move->StartLocation = Start;
	Move->TargetLocation = Target;
	Move->Forward = (Target - Start).GetSafeNormal2D();
	Move->RiseExponent = MantleRiseExponent;

	// Keep entry momentum, but never too slow to reach the ledge in time
	const float EntrySpeed = FVector::DotProduct(Velocity, Move->Forward);
	const float NeededSpeed = FVector::Dist2D(Start, Target) / MantleDuration;
	Move->GlideSpeed = FMath::Max(EntrySpeed, NeededSpeed);

	// Exit with a small forward push instead of a dead stop
	Move->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	Move->FinishVelocityParams.SetVelocity = Move->Forward * Move->GlideSpeed * MantleExitSpeedFraction;

	ApplyRootMotionSource(Move);
}

bool UMDACharacterMovementComponent::FindMantleTarget(FVector& OutTarget) const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float Radius = Capsule->GetScaledCapsuleRadius();
	const float StandHalfHeight = CharacterOwner->GetDefaultHalfHeight();
	const FVector Feet = UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight());
	const FVector Forward = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	const ECollisionChannel Channel = UpdatedComponent->GetCollisionObjectType();
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(MDAMantle), false, CharacterOwner);

	// 1: Check for a wall in front of the player
	const FVector WallStart = Feet + FVector(0.f, 0.f, MinLedgeHeight);
	const FVector WallEnd = WallStart + Forward * (Radius + MantleReachDistance);
	FHitResult WallHit;
	if (!GetWorld()->LineTraceSingleByChannel(WallHit, WallStart, WallEnd, Channel, Params))
	{
		return false;
	}

	// 2: Find if there is a reachable ledge on the detected wall
	const FVector Inward = WallHit.ImpactPoint - WallHit.ImpactNormal.GetSafeNormal2D() * (Radius + MantleLedgeExtraDistance);
	const FVector TopStart(Inward.X, Inward.Y, Feet.Z + MaxLedgeHeight);
	const FVector TopEnd(Inward.X, Inward.Y, Feet.Z + MinLedgeHeight);
	FHitResult TopHit;
	if (!GetWorld()->LineTraceSingleByChannel(TopHit, TopStart, TopEnd, Channel, Params) || TopHit.bStartPenetrating || !IsWalkable(TopHit))
	{
		return false;
	}

	// 3: Final check to see if the player would actually fit on the ledge post mantle
	const FVector Target = TopHit.ImpactPoint + FVector(0.f, 0.f, StandHalfHeight + 2.f);
	if (GetWorld()->OverlapBlockingTestByChannel(Target, FQuat::Identity, Channel,
			FCollisionShape::MakeCapsule(Radius, StandHalfHeight), Params))
	{
		return false;
	}
	
	OutTarget = Target;
	return true;
}
