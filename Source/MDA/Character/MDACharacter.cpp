// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDACharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/SpringArmComponent.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Input/MDAEnhancedInputComponent.h"


AMDACharacter::AMDACharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
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
		ETriggerEvent::Triggered, this, &AMDACharacter::Move);

	// Looking
	EIC->BindNativeAction(InputConfig, GameTags::InputTag_Look,
		ETriggerEvent::Triggered, this, &AMDACharacter::Look);
	
	
	
}

void AMDACharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
	UE_LOG(LogPlayerInput, Display, TEXT("Moving"));
}

void AMDACharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
	UE_LOG(LogPlayerInput, Display, TEXT("Looking"));
}

void AMDACharacter::RequestJump()
{
}

void AMDACharacter::RequestJumpEnd()
{
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
