// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAWeaponMenuWidget.h"

#include "MDAWeaponButtonWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "MDA/Weapons/MDAWeaponAsset.h"

void UMDAWeaponMenuWidget::Populate(const TArray<UMDAWeaponAsset*>& Weapons)
{
	if (!WeaponButtonClass) return;
	
	WeaponGridPanel->ClearChildren();
	
	for (UMDAWeaponAsset* Weapon : Weapons)
	{
		if (Weapon)
		{
			UMDAWeaponButtonWidget* WeaponButton = CreateWidget<UMDAWeaponButtonWidget>(GetOwningPlayer(), WeaponButtonClass);
			WeaponButton->Init(Weapon);
			UUniformGridSlot* GridSlot = WeaponGridPanel->AddChildToUniformGrid(WeaponButton);
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
	
}
