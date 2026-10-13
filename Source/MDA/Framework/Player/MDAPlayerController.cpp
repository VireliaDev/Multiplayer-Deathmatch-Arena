// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "MDAPlayerState.h"
#include "MDA/AbilitySystem/MDAAbilitySystemComponent.h"

void AMDAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (const UInputMappingContext* CurrentContext : PlayerInputMappingContext)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void AMDAPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (const AMDAPlayerState* PS = GetPlayerState<AMDAPlayerState>())
	{
		if (UMDAAbilitySystemComponent* ASC = Cast<UMDAAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
		{
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
	Super::PostProcessInput(DeltaTime, bGamePaused);
}
