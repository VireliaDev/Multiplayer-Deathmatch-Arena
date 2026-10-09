// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacter.h"

#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "MDACharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/SpringArmComponent.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Framework/Player/MDAPlayerState.h"
#include "MDA/Input/MDAEnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

AMDACharacter::AMDACharacter(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UMDACharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	MDAMovementComponent = GetCharacterMovement<UMDACharacterMovementComponent>();
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	GetMesh()->EnableExternalInterpolation(true);
}

void AMDACharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMDACharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
	
}

void AMDACharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilitySystemComponent();
}

void AMDACharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//Handle listen server animation problems
	if (GetNetMode() == NM_ListenServer && !IsLocallyControlled())
	{
		if (USkeletalMeshComponent* CharMesh = GetMesh())
		{
			CharMesh->bOnlyAllowAutonomousTickPose = HasAnyRootMotion();
		}
	}
	
	//Handle Aiming FOV control
	if (IsLocallyControlled() && ThirdPersonCamera)
	{
		const float Target = MDAMovementComponent->IsAiming() ? AimFOV : DefaultFOV;
		ThirdPersonCamera->SetFieldOfView(FMath::FInterpTo(ThirdPersonCamera->FieldOfView, Target, DeltaSeconds, FOVInterpSpeed));
	}
	
}

void AMDACharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMDACharacter, bRepIsSprinting, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AMDACharacter, bRepIsAiming,    COND_SimulatedOnly);
}

void AMDACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	UMDAEnhancedInputComponent* EIC = Cast<UMDAEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		UE_LOG(LogPlayerInput, Error, TEXT("Enhanced Input Component is not MDAEnhancedInputComponent"));
		return;
	}
	
	if (!InputConfig)
	{
		UE_LOG(LogPlayerInput, Error, TEXT("Input Config is nullptr"));
		return;
	}
	
	
	
	// Moving
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Move,
		ETriggerEvent::Triggered, this, &AMDACharacter::Input_Move);

	// Looking
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Look,
		ETriggerEvent::Triggered, this, &AMDACharacter::Input_Look);
	
	// Jumping
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Jump,
		ETriggerEvent::Started, this, &AMDACharacter::RequestJump);
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Jump,
		ETriggerEvent::Completed, this, &AMDACharacter::RequestJumpEnd);
	
	// Crouching
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Crouch,
		ETriggerEvent::Started, this, &AMDACharacter::RequestCrouch);
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Crouch,
		ETriggerEvent::Completed, this, &AMDACharacter::RequestCrouchEnd);
	
	// Sprinting
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Sprint,
		ETriggerEvent::Started, this, &AMDACharacter::RequestSprint);
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Sprint,
		ETriggerEvent::Completed, this, &AMDACharacter::RequestSprintEnd);
	
	// Aiming
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Aim,
		ETriggerEvent::Started, this, &AMDACharacter::RequestAimIn);
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Aim,
		ETriggerEvent::Completed, this, &AMDACharacter::RequestAimOut);
}

void AMDACharacter::SetReplicatedMovementState(const bool bSprinting, const bool bAiming)
{
	bRepIsSprinting = bSprinting;
	bRepIsAiming = bAiming;
}

bool AMDACharacter::IsSprintingCosmetic() const
{
	return GetLocalRole() == ROLE_SimulatedProxy ? bRepIsSprinting : MDAMovementComponent->IsSprinting();
}

bool AMDACharacter::IsAimingCosmetic() const
{
	return GetLocalRole() == ROLE_SimulatedProxy ? bRepIsAiming : MDAMovementComponent->IsAiming();
}

void AMDACharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
	UE_LOG(LogPlayerInput, VeryVerbose, TEXT("Moving"));
}

void AMDACharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
	UE_LOG(LogPlayerInput, VeryVerbose, TEXT("Looking"));
}

void AMDACharacter::RequestJump()
{
	Jump();
}

void AMDACharacter::RequestJumpEnd()
{
	StopJumping();
}

bool AMDACharacter::CanJumpInternal_Implementation() const
{
	//Skip !IsCrouched check to allow jumping from crouched
	return JumpIsAllowedInternal();
}

void AMDACharacter::RequestCrouch()
{
	if (MDAMovementComponent)
	{
		MDAMovementComponent->SetCrouchHeld(true);
	}
}

void AMDACharacter::RequestCrouchEnd()
{
	if (MDAMovementComponent)
	{
		MDAMovementComponent->SetCrouchHeld(false);
	}
}

void AMDACharacter::RequestSprint()
{
	if (MDAMovementComponent)
	{
		MDAMovementComponent->SetSprintHeld(true);
	}
}

void AMDACharacter::RequestSprintEnd()
{
	if (MDAMovementComponent)
	{
		MDAMovementComponent->SetSprintHeld(false);
	}
}

void AMDACharacter::RequestAimIn()
{
	if (MDAMovementComponent)
	{
		MDAMovementComponent->SetAimHeld(true);
	}
}

void AMDACharacter::RequestAimOut()
{
	if (MDAMovementComponent)
	{
		MDAMovementComponent->SetAimHeld(false);
	}
}

UAbilitySystemComponent* AMDACharacter::GetAbilitySystemComponent() const
{
	const AMDAPlayerState* PS = GetPlayerState<AMDAPlayerState>();
	return PS ? PS->GetAbilitySystemComponent() : nullptr;
}

void AMDACharacter::InitAbilitySystemComponent()
{
	AMDAPlayerState* PS = GetPlayerState<AMDAPlayerState>();
	if (!PS) return;
	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
}
