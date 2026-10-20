// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAHUD.h"
#include "MDAWeaponMenuWidget.h"
#include "Blueprint/UserWidget.h"


void AMDAHUD::BeginPlay()
{
	Super::BeginPlay();
	
	
	//Create weapon menu
	if (WeaponMenuClass)
	{
		WeaponMenu = CreateWidget<UMDAWeaponMenuWidget>(GetOwningPlayerController(), WeaponMenuClass);
	}
}

void AMDAHUD::OpenWeaponMenu()
{
	if (IsWeaponMenuOpen())
	{
		return;
	}
	
	WeaponMenu->AddToViewport();
	FInputModeGameAndUI InputModeGameAndUI;
	InputModeGameAndUI.SetHideCursorDuringCapture(false);
	GetOwningPlayerController()->SetInputMode(InputModeGameAndUI);
	GetOwningPlayerController()->SetShowMouseCursor(true);
}

void AMDAHUD::CloseWeaponMenu()
{
	if (!IsWeaponMenuOpen())
	{
		return;
	}
	
	WeaponMenu->RemoveFromParent();
	GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
	GetOwningPlayerController()->SetShowMouseCursor(false);
}

bool AMDAHUD::IsWeaponMenuOpen() const
{
	return WeaponMenu && WeaponMenu->IsInViewport();
}
