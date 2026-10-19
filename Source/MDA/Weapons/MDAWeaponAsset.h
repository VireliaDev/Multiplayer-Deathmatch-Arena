// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MDAWeaponAsset.generated.h"


UENUM(BlueprintType)
enum class EWeaponSlot : uint8 {
		None,
		Primary,
		Secondary
};



UCLASS(Blueprintable, Const)
class MDA_API UMDAWeaponAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	////Cosmetic////
	UPROPERTY(EditDefaultsOnly, Category = "Cosmetic")
	FName WeaponName = NAME_None;
	
	
	
	
	////Gameplay////
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	EWeaponSlot WeaponSlot = EWeaponSlot::None;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float MagazineSize = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float ReserveAmmoSize = 0;
};
