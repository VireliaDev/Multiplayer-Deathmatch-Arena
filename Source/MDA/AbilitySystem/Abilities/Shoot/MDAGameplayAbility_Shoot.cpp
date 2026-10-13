// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAGameplayAbility_Shoot.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "MDA/AbilitySystem/Tags/MDAGameplayTags.h"

UMDAGameplayAbility_Shoot::UMDAGameplayAbility_Shoot()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(GameTags::Movement_State_Sprinting);
	ActivationBlockedTags.AddTag(GameTags::Movement_State_SprintRecovery);
	ActivationBlockedTags.AddTag(GameTags::Movement_State_Mantling);
}

void UMDAGameplayAbility_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Placeholder shot. The weapon system replaces this.
	UE_LOG(LogTemp, Log, TEXT("[%s] Fire: shot"), ActorInfo->IsNetAuthority() ? TEXT("Server") : TEXT("Client"));

	// One shot per activation; holding fire re-activates it after the interval.
	UAbilityTask_WaitDelay* Wait = UAbilityTask_WaitDelay::WaitDelay(this, TestShotInterval);
	Wait->OnFinish.AddDynamic(this, &ThisClass::OnShotIntervalFinished);
	Wait->ReadyForActivation();

	UAbilityTask_WaitGameplayTagAdded* WaitMantle = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, GameTags::Movement_State_Mantling);
	WaitMantle->Added.AddDynamic(this, &ThisClass::OnMantle);
	WaitMantle->ReadyForActivation();
}




void UMDAGameplayAbility_Shoot::OnShotIntervalFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMDAGameplayAbility_Shoot::OnMantle()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
