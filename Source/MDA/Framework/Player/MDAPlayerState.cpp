// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAPlayerState.h"

#include "AbilitySystemComponent.h"
#include "MDA/AbilitySystem/MDAAbilitySystemComponent.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Character/MDACharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogPlayerState);

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
	if (!HasAuthority() || !StartingWeapon)
	{
		return;
	}
	
	switch (StartingWeapon->WeaponSlot)
	{
	case EWeaponSlot::Primary:
		GiveNewWeapon(StartingWeapon);
		break;
	case EWeaponSlot::Secondary:
		GiveNewWeapon(StartingWeapon);
		break;
	default:
		break;
	}
	SetCurrentSlot(StartingWeapon->WeaponSlot);
	
	
	
}

void AMDAPlayerState::RequestNewWeapon_Implementation(UMDAWeaponAsset* RequestedWeapon)
{
	if (RequestedWeapon)
	{
		if (RequestedWeapon != PrimaryWeapon && RequestedWeapon != SecondaryWeapon)
		{
			const bool bSuccess = GiveNewWeapon(RequestedWeapon);
		
			if (!bSuccess)
			{
				UE_LOG(LogPlayerState, Error, TEXT("Failed To give requested weapon."))
			}
		}
	}
}


UMDAWeaponAsset* AMDAPlayerState::GetCurrentWeapon() const
{
	return GetWeaponAtSlot(GetCurrentSlot());
}

void AMDAPlayerState::SetCurrentSlot(const EWeaponSlot NewSlot)
{
	if (!HasAuthority()) return;
	CurrentSlot = NewSlot;
	UpdateWeaponMesh();
}

EWeaponSlot AMDAPlayerState::GetCurrentSlot() const
{
	if (PredictedSlot != EWeaponSlot::None)
	{
		return PredictedSlot;
	}
	return CurrentSlot;
}

void AMDAPlayerState::SetPredictedSlot(const EWeaponSlot NewSlot)
{
	if (NewSlot != CurrentSlot)
	{
		PredictedSlot = EWeaponSlot::None;
	}
	else
	{
		PredictedSlot = NewSlot;
	}
	UpdateWeaponMesh();
}

UMDAWeaponAsset* AMDAPlayerState::GetWeaponAtSlot(const EWeaponSlot Slot) const
{
	switch (Slot)
	{
	case EWeaponSlot::Primary:
		return PrimaryWeapon;
	
	case EWeaponSlot::Secondary:
		return SecondaryWeapon;
	
	default:
		return nullptr;
		
	}
}

bool AMDAPlayerState::GiveNewWeapon(UMDAWeaponAsset* NewWeapon)
{
	if (NewWeapon)
	{
		if(NewWeapon->WeaponSlot == EWeaponSlot::None)
		{
			return false;
		}
		if (NewWeapon->WeaponSlot == EWeaponSlot::Primary)
		{
			PrimaryWeapon = NewWeapon;
			SetCurrentSlot(EWeaponSlot::Primary);
			if (APawn* Pawn = GetPawn())
			{
				FGameplayEventData Payload;
				Payload.OptionalObject = NewWeapon;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, GameTags::Event_Weapon_EquipPrimary, Payload);
			}	
			SetNewWeaponAmmoAttributeValues(NewWeapon);
			return true;
		}
		if (NewWeapon->WeaponSlot == EWeaponSlot::Secondary)
		{
			SecondaryWeapon = NewWeapon;
			SetNewWeaponAmmoAttributeValues(NewWeapon);
			return true;
		}
	}
	return false;
}

void AMDAPlayerState::SetNewWeaponAmmoAttributeValues(UMDAWeaponAsset* NewWeapon)
{
	
}

void AMDAPlayerState::OnRep_CurrentSlot()
{
	if (CurrentSlot == PredictedSlot)
	{
		PredictedSlot = EWeaponSlot::None;
	}
	UpdateWeaponMesh();
}

void AMDAPlayerState::OnRep_PrimaryWeapon()
{
	UpdateWeaponMesh();
}

void AMDAPlayerState::OnRep_SecondaryWeapon()
{
	UpdateWeaponMesh();
}

void AMDAPlayerState::UpdateWeaponMesh() const
{
	const AMDACharacter* MDACharacter = GetPawn<AMDACharacter>();
	if (!MDACharacter)
	{
		return; //No character exists, or the character is unpossessed (Dead) and the weapon mesh shouldn't change
	}
	
	if (const UMDAWeaponAsset* CurrentWeapon = GetCurrentWeapon())
	{
			MDACharacter->WeaponMeshComponent->SetSkeletalMeshAsset(CurrentWeapon->WeaponMesh);
	}
	else
	{
		MDACharacter->WeaponMeshComponent->SetSkeletalMeshAsset(nullptr);
	}
}
