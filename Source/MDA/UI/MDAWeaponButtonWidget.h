// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MDAWeaponButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UMDAWeaponAsset;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class MDA_API UMDAWeaponButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UMDAWeaponAsset* InWeaponAsset);

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta=(BindWidget)) 
	TObjectPtr<UButton> WeaponButton;
	UPROPERTY(meta=(BindWidget)) 
	TObjectPtr<UTextBlock> WeaponName;

private:
	UFUNCTION() 
	void HandleClicked() const;
	UPROPERTY() 
	TObjectPtr<UMDAWeaponAsset> WeaponAsset;
};
