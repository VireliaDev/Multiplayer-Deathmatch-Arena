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


class UMDACharacterMovementComponent;

AMDACharacter::AMDACharacter(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer.SetDefaultSubobjectClass<UMDACharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	GetMesh()->EnableExternalInterpolation(true);
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
	
	if (GetNetMode() == NM_ListenServer && !IsLocallyControlled())
	{
		if (USkeletalMeshComponent* CharMesh = GetMesh())
		{
			CharMesh->bOnlyAllowAutonomousTickPose = HasAnyRootMotion();
		}
	}
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

void AMDACharacter::RequestCrouch()
{
}

void AMDACharacter::RequestCrouchEnd()
{
}

void AMDACharacter::RequestSprint()
{
}

void AMDACharacter::RequestSprintEnd()
{
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
