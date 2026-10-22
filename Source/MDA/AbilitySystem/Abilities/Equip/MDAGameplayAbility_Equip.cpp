// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAGameplayAbility_Equip.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Character/MDACharacter.h"
#include "MDA/Framework/Player/MDAPlayerState.h"

UMDAGameplayAbility_Equip::UMDAGameplayAbility_Equip()
{
	ActivationBlockedTags.AddTag(GameTags::State_Dead);
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
	const UMDAWeaponAsset* WeaponToEquip = PlayerState ? PlayerState->GetWeaponAtSlot(TargetSlot) : nullptr;

	if (!WeaponToEquip || !WeaponToEquip->WeaponEquipMontage || !PlayerState || !Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (PlayerState->GetCurrentSlot() == TargetSlot)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (HasAuthority(&ActivationInfo))
	{
		PlayerState->SetCurrentSlot(TargetSlot);
	}

	UAbilityTask_PlayMontageAndWait* MontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, WeaponToEquip->WeaponEquipMontage);
	MontageAndWait->OnCompleted.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->OnBlendOut.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->OnInterrupted.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->OnCancelled.AddDynamic(this, &UMDAGameplayAbility_Equip::OnMontageOver);
	MontageAndWait->ReadyForActivation();
}
