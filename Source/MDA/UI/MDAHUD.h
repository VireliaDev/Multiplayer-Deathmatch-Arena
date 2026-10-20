// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MDAHUD.generated.h"

class UMDAWeaponMenuWidget;
/**
 * 
 */
UCLASS()
class MDA_API AMDAHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	
	////Weapon Menu////
public:
	void OpenWeaponMenu();
	void CloseWeaponMenu();
	bool IsWeaponMenuOpen() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UMDAWeaponMenuWidget> WeaponMenuClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UMDAWeaponMenuWidget> WeaponMenu;
	
};
