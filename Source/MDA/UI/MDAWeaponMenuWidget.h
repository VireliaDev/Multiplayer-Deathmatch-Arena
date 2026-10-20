// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MDAWeaponMenuWidget.generated.h"

class UMDAWeaponButtonWidget;
class UUniformGridPanel;
class UMDAWeaponAsset;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class MDA_API UMDAWeaponMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//Call from OnInitialized Widget, and pass in all the possible weapons
	UFUNCTION(BlueprintCallable, Category="Weapon Menu")
	void Populate(const TArray<UMDAWeaponAsset*>& Weapons);

protected:
	UPROPERTY(meta=(BindWidget)) 
	TObjectPtr<UUniformGridPanel> WeaponGridPanel;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Menu")
	TSubclassOf<UMDAWeaponButtonWidget> WeaponButtonClass;
};
