// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAWeaponMenuWidget.h"

#include "MDAWeaponButtonWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "MDA/Weapons/MDAWeaponAsset.h"

void UMDAWeaponMenuWidget::Populate(const TArray<UMDAWeaponAsset*>& Weapons)
{
	if (!WeaponsVerticalBox || !WeaponButtonClass) return;

	WeaponsVerticalBox->ClearChildren();

	for (UMDAWeaponAsset* Weapon : Weapons)
	{
		if (!Weapon) continue;

		UMDAWeaponButtonWidget* WeaponButton = CreateWidget<UMDAWeaponButtonWidget>(GetOwningPlayer(), WeaponButtonClass);
		if (!WeaponButton) continue;

		WeaponButton->Init(Weapon);

		if (UVerticalBoxSlot* BoxSlot = WeaponsVerticalBox->AddChildToVerticalBox(WeaponButton))
		{
			BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			BoxSlot->SetHorizontalAlignment(HAlign_Fill);
			BoxSlot->SetVerticalAlignment(VAlign_Fill);
			BoxSlot->SetPadding(FMargin(0.f, 4.f));
		}
	}
}
