// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Engine/DataAsset.h"
#include "MDAWeaponAsset.generated.h"


UENUM(BlueprintType)
enum class EWeaponSlot : uint8 {
		None,
		Primary,
		Secondary
};



UCLASS(Blueprintable)
class MDA_API UMDAWeaponAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	////Cosmetic////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmetic")
	FText WeaponName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmetic")
	TObjectPtr<USkeletalMesh> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmetic")
	TObjectPtr<UAnimMontage> WeaponEquipMontage;
	
	////Gameplay////
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	EWeaponSlot WeaponSlot = EWeaponSlot::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	float MagazineSize = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	float ReserveAmmoSize = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<UGameplayEffect> WeaponAmmoCostGameplayEffect;
};
