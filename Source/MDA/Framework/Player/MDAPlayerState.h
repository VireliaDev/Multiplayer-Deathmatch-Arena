// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "MDA/Weapons/MDAWeaponAsset.h"
#include "MDAPlayerState.generated.h"

class UMDAAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class MDA_API AMDAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMDAPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	

	
	/////////////////Ability System Component////////////////
protected:
	UPROPERTY()
	TObjectPtr<UMDAAbilitySystemComponent> AbilitySystemComponent;
	
	void CreateAbilitySystemComponent();
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	
	
protected:
	////Weapon Stuff////
	UPROPERTY(EditDefaultsOnly, Category = "Weapon System")
	TObjectPtr<UMDAWeaponAsset> StartingWeapon;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	TObjectPtr<UMDAWeaponAsset> PrimaryWeapon;

	UPROPERTY(Replicated, VisibleAnywhere)
	TObjectPtr<UMDAWeaponAsset> SecondaryWeapon;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, VisibleAnywhere)
	EWeaponSlot CurrentSlot = EWeaponSlot::Secondary;
	
	void InitWeaponSystem();
	
	
public:
	bool GiveNewWeapon(UMDAWeaponAsset* NewWeapon);
	
	UMDAWeaponAsset* GetCurrentWeapon() const;
	
private:
	UFUNCTION()
	void OnRep_CurrentWeapon();
};
