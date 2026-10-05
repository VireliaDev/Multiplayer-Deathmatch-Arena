// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerController.h"

#include "EnhancedInputSubsystems.h"

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
