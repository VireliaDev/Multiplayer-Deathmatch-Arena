// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerState.h"

#include "AbilitySystemComponent.h"
#include "MDA/AbilitySystem/MDAAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "ViewportInteractions/ViewportInteractionBindings.h"

AMDAPlayerState::AMDAPlayerState()
{
	CreateAbilitySystemComponent();
}

void AMDAPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMDAPlayerState, CurrentSlot);
	DOREPLIFETIME(AMDAPlayerState, PrimaryWeapon);
	DOREPLIFETIME(AMDAPlayerState, SecondaryWeapon);

}

void AMDAPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	InitWeaponSystem();
}

void AMDAPlayerState::CreateAbilitySystemComponent()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMDAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	SetNetUpdateFrequency(100.f); //Lyra standard
}

UAbilitySystemComponent* AMDAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}



void AMDAPlayerState::InitWeaponSystem()
{
	switch (StartingWeapon->WeaponSlot)
	{
	case EWeaponSlot::Primary:
		PrimaryWeapon = StartingWeapon;
		break;
	case EWeaponSlot::Secondary:
		SecondaryWeapon = StartingWeapon;
		break;
	default:
		break;
	}
	CurrentSlot = StartingWeapon->WeaponSlot;
}

bool AMDAPlayerState::GiveNewWeapon(UMDAWeaponAsset* NewWeapon)
{
	if (NewWeapon)
	{
		switch (NewWeapon->WeaponSlot)
		{
		case EWeaponSlot::None:
			return false;
		case EWeaponSlot::Primary:
			PrimaryWeapon = NewWeapon;
			CurrentSlot = EWeaponSlot::Primary;
			return true;
		case EWeaponSlot::Secondary:
			SecondaryWeapon = NewWeapon;
			return true;
		}
	}
	return false;
}

UMDAWeaponAsset* AMDAPlayerState::GetCurrentWeapon() const
{
	switch (CurrentSlot)
	{
	case EWeaponSlot::Primary:
		return PrimaryWeapon;
	case EWeaponSlot::Secondary:
		return  SecondaryWeapon;
	default:
		return nullptr;
	}
}


void AMDAPlayerState::OnRep_CurrentWeapon()
{
}
