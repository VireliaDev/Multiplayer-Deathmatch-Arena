// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAWeaponButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MDA/Framework/Player/MDAPlayerState.h"
#include "MDA/Weapons/MDAWeaponAsset.h"

void UMDAWeaponButtonWidget::Init(UMDAWeaponAsset* InWeaponAsset)
{
	if (!InWeaponAsset) return;
	
	WeaponAsset = InWeaponAsset;
	WeaponName->SetText(WeaponAsset->WeaponName);
}

void UMDAWeaponButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	WeaponButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
}

void UMDAWeaponButtonWidget::HandleClicked() const
{
	if (AMDAPlayerState* PlayerState = GetOwningPlayerState<AMDAPlayerState>())
	{
		PlayerState->RequestNewWeapon(WeaponAsset);
	}
}
