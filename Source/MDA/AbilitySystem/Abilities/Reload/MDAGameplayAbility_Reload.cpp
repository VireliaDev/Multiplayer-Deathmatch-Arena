// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAGameplayAbility_Reload.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"
#include "MDA/Character/Movement/MDACharacterMovementComponent.h"

UMDAGameplayAbility_Reload::UMDAGameplayAbility_Reload()
{
	ActivationBlockedTags.AddTag(GameTags::Movement_State_Mantling);
	ActivationBlockedTags.AddTag(GameTags::State_Equipping);
}

void UMDAGameplayAbility_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//Only set CMC intent from local player
	if (ActorInfo->IsLocallyControlled())
	{
		if (UMDACharacterMovementComponent* CMC = GetMDAMovementFromActorInfo(ActorInfo))
		{
			CMC->SetIsReloading(true);
		}
	}
	
	//Play reload montage
	UAbilityTask_PlayMontageAndWait* MontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ReloadMontage,
		1.f,
		NAME_None,
		true);
	MontageAndWait->OnCompleted.AddDynamic(this, &ThisClass::OnReloadFinished);
	MontageAndWait->ReadyForActivation();
	
	UAbilityTask_WaitGameplayTagAdded* WaitMantle = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, GameTags::Movement_State_Mantling);
	WaitMantle->Added.AddDynamic(this, &ThisClass::OnMantle);
	WaitMantle->ReadyForActivation();
	
}

void UMDAGameplayAbility_Reload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->IsLocallyControlled())
	{
		if (UMDACharacterMovementComponent* CMC = GetMDAMovementFromActorInfo(ActorInfo))
		{
			CMC->SetIsReloading(false);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMDAGameplayAbility_Reload::OnReloadFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMDAGameplayAbility_Reload::OnMantle()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
