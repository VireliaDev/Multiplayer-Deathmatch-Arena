// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "MDAPlayerState.h"
#include "MDA/AbilitySystem/MDAAbilitySystemComponent.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Input/MDAEnhancedInputComponent.h"
#include "MDA/UI/MDAHUD.h"

void AMDAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (const UInputMappingContext* CurrentContext : PlayerInputMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
		
		
		//Setup controller level input bindings (UI)
		if (UMDAEnhancedInputComponent* EIC = Cast<UMDAEnhancedInputComponent>(InputComponent))
		{
			//WeaponMenu
			EIC->BindNativeAction(InputConfig, GameTags::InputTag_UI_WeaponMenu,
				ETriggerEvent::Started, this, &AMDAPlayerController::Input_ToggleWeaponMenu);
			
		}
	}
}

void AMDAPlayerController::Input_ToggleWeaponMenu()
{
	if (AMDAHUD* HUD = GetHUD<AMDAHUD>())
	{
		if (!HUD->IsWeaponMenuOpen() && CanOpenWeaponMenu())
		{
			HUD->OpenWeaponMenu();
		} 
		else
		{
			HUD->CloseWeaponMenu();
		}
	}
}

bool AMDAPlayerController::CanOpenWeaponMenu()
{
	return true;
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
