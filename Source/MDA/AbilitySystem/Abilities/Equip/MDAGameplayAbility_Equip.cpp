// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAGameplayAbility_Equip.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Character/MDACharacter.h"
#include "MDA/Framework/Player/MDAPlayerState.h"

UMDAGameplayAbility_Equip::UMDAGameplayAbility_Equip()
{
	ActivationBlockedTags.AddTag(GameTags::State_Dead);
	ActivationOwnedTags.AddTag(GameTags::State_Equipping);
}

void UMDAGameplayAbility_Equip::OnMontageOver()
{
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(), true, false);
}

void UMDAGameplayAbility_Equip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const AMDACharacter* Character = GetMDACharacterFromActorInfo(ActorInfo);
	AMDAPlayerState* PlayerState = Character ? Character->GetMDAPlayerState() : nullptr;
	const UMDAWeaponAsset* WeaponToEquip = nullptr;
	if (TriggerEventData)
	{
		WeaponToEquip = Cast<UMDAWeaponAsset>(TriggerEventData->OptionalObject);
	}
	if (!WeaponToEquip && PlayerState)
	{
		WeaponToEquip = PlayerState->GetWeaponAtSlot(TargetSlot);
	}
	

	if (!WeaponToEquip || !PlayerState || !Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	//Check that the ability isn't changing to the same slot (and that it was activated not by an event, ie it was from user input)
	if (!TriggerEventData && PlayerState->GetCurrentSlot() == TargetSlot) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (HasAuthority(&ActivationInfo))
	{
		PlayerState->SetCurrentSlot(TargetSlot);
	}
	else if (IsPredictingClient())
	{
		PlayerState->SetPredictedSlot(TargetSlot);
		FPredictionKey Key = ActorInfo->AbilitySystemComponent->ScopedPredictionKey;
		if (Key.IsValidKey())
		{
			Key.NewRejectedDelegate().BindUObject(this, &UMDAGameplayAbility_Equip::OnPredictionRejected);
		}
	}
	
	if (!WeaponToEquip->WeaponEquipMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, WeaponToEquip->WeaponEquipMontage);
	MontageAndWait->OnCompleted.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->OnBlendOut.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->OnInterrupted.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->OnCancelled.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->ReadyForActivation();
}

void UMDAGameplayAbility_Equip::OnPredictionRejected()
{
	const AMDACharacter* Character = GetMDACharacterFromActorInfo(GetCurrentActorInfo());
	if (AMDAPlayerState* PlayerState = Character ? Character->GetMDAPlayerState() : nullptr)
	{
		PlayerState->SetPredictedSlot(EWeaponSlot::None);
	}
}
