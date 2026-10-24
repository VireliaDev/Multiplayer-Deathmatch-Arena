// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "MDA/Weapons/MDAWeaponAsset.h"
#include "MDAPlayerState.generated.h"

class UMDAAbilitySystemComponent;


DECLARE_LOG_CATEGORY_EXTERN(LogPlayerState, Log, All);


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
	

	
	////Ability System Component////
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
	
	UPROPERTY(ReplicatedUsing = OnRep_PrimaryWeapon, VisibleAnywhere)
	TObjectPtr<UMDAWeaponAsset> PrimaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon, VisibleAnywhere)
	TObjectPtr<UMDAWeaponAsset> SecondaryWeapon;
	
	void InitWeaponSystem();

public:
	
	UFUNCTION(Server, Reliable)
	void RequestNewWeapon(UMDAWeaponAsset* RequestedWeapon);
	
	UMDAWeaponAsset* GetCurrentWeapon() const;
	
	void SetCurrentSlot(EWeaponSlot NewSlot);
	EWeaponSlot GetCurrentSlot() const ;
	
	void SetPredictedSlot(EWeaponSlot NewSlot);
	
	UMDAWeaponAsset* GetWeaponAtSlot(EWeaponSlot Slot) const;
	
	
	void UpdateWeaponMesh() const;

private:
	bool GiveNewWeapon(UMDAWeaponAsset* NewWeapon);
	void SetNewWeaponAmmoAttributeValues(UMDAWeaponAsset* NewWeapon);
	
	UFUNCTION()
	void OnRep_CurrentSlot();
	
	UFUNCTION()
	void OnRep_PrimaryWeapon();
	
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSlot, VisibleAnywhere)
	EWeaponSlot CurrentSlot = EWeaponSlot::None;
	
	UPROPERTY(VisibleAnywhere)
	EWeaponSlot PredictedSlot = EWeaponSlot::None;
	
};
