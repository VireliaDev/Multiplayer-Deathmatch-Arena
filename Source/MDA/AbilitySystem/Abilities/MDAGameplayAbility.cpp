// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "MDA/Character/Movement/MDACharacterMovementComponent.h"

UMDAGameplayAbility::UMDAGameplayAbility()
{
}

void UMDAGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	if (ActivationPolicy == EMDAGameAbilityActivationPolicy::OnSpawn)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	} 
}

UMDACharacterMovementComponent* UMDAGameplayAbility::GetMDAMovementFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo)
{
	return ActorInfo ? Cast<UMDACharacterMovementComponent>(ActorInfo->MovementComponent.Get()) : nullptr;
}
