// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.


#include "MDAAbilitySystemComponent.h"

#include "Abilities/MDAAbilitySet.h"
#include "Abilities/MDAGameplayAbility.h"


UMDAAbilitySystemComponent::UMDAAbilitySystemComponent()
{
}

void UMDAAbilitySystemComponent::GiveAbilitySet(const FMDAGameAbilitySet& AbilitySet, UObject* SourceObject, FGameAbilitySetHandles& GrantedHandles)
{
	if (!IsOwnerActorAuthoritative()) // ONLY THE SERVER CAN GIVE ABILITIES
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("GiveAbilitySet(): Failed, Was called without authority."))
		return;
	}   
	
	
	// APPLY START UP EFFECTS BEFORE GIVING ABILITIES
	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySet.StartupEffects)
	{
		if (!EffectClass) { continue; }

		FGameplayEffectContextHandle Ctx = MakeEffectContext();
		Ctx.AddSourceObject(SourceObject);

		const FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(EffectClass, 1.f, Ctx);
		if (Spec.IsValid())
		{
			GrantedHandles.EffectHandles.Add(ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()));
		}
	}

	// GIVE ABILITIES
	for (const FGameAbilityEntry& Entry : AbilitySet.Abilities)
	{
		if (!Entry.Ability) { continue; }

		FGameplayAbilitySpec Spec(Entry.Ability, Entry.AbilityLevel);
		Spec.SourceObject = SourceObject;

		// Add input tag too
		Spec.GetDynamicSpecSourceTags().AddTag(Entry.InputTag);

		GrantedHandles.AbilityHandles.Add(GiveAbility(Spec));
	}
	

	
	
	
}
void UMDAAbilitySystemComponent::TakeAbilitySet(FGameAbilitySetHandles& GrantedHandles)
{
	if (!IsOwnerActorAuthoritative()) // ONLY THE SERVER CAN REVOKE ABILITIES
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("TakeAbilitySet(): Failed, Was called without authority."))
		return;
	}   

	for (const FGameplayAbilitySpecHandle& H : GrantedHandles.AbilityHandles)
	{
		if (H.IsValid()) { ClearAbility(H); }
	}
	for (const FActiveGameplayEffectHandle& H : GrantedHandles.EffectHandles)
	{
		if (H.IsValid()) { RemoveActiveGameplayEffect(H); }
	}
	GrantedHandles.Reset();
	
}



void UMDAAbilitySystemComponent::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) { return; }

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			InputPressedSpecHandles.AddUnique(Spec.Handle);
			InputHeldSpecHandles.AddUnique(Spec.Handle);
		}
	}
}

void UMDAAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) { return; }

	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			InputReleasedSpecHandles.AddUnique(Spec.Handle);
			InputHeldSpecHandles.Remove(Spec.Handle);
		}
	}
}

void UMDAAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// For abilities being held down
	for (const FGameplayAbilitySpecHandle& Handle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			const UMDAGameplayAbility* CDO = Cast<UMDAGameplayAbility>(Spec->Ability);
			if (Spec->Ability && !Spec->IsActive() && CDO &&
				CDO->ActivationPolicy == EMDAGameAbilityActivationPolicy::WhileInputActive)
			{
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}

	
	// For abilities that were just pressed
	for (const FGameplayAbilitySpecHandle& Handle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			if (!Spec->Ability) { continue; }
			Spec->InputPressed = true;

			if (Spec->IsActive())
			{
				AbilitySpecInputPressed(*Spec);   // used for waiting for input style ability tasks
			}
			else if (const UMDAGameplayAbility* CDO = Cast<UMDAGameplayAbility>(Spec->Ability))
			{
				if (CDO->ActivationPolicy == EMDAGameAbilityActivationPolicy::OnInputTriggered)
				{
					AbilitiesToActivate.AddUnique(Handle);
				}
			}
		}
	}
	
	//Finally Activate the abilities
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}

	// Handle Releasing Abilities
	for (const FGameplayAbilitySpecHandle& Handle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			if (!Spec->Ability) { continue; }
			Spec->InputPressed = false;
			if (Spec->IsActive())
			{
				AbilitySpecInputReleased(*Spec);
			}
		}
	}

	//Reset the handles for this frames ability presses and releases
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

