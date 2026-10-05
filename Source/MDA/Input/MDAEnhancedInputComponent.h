// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputModule.h"
#include "MDAInputConfig.h"
#include "MDAEnhancedInputComponent.generated.h"



UCLASS(Config = Input)
class MDA_API UMDAEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(const UMDAInputConfig* Config, const FGameplayTag& Tag, ETriggerEvent Event, UserClass* Object, FuncType Func)
	{
		if (const UInputAction* InputAction = Config->FindNativeActionForTag(Tag))
		{
			BindAction(InputAction, Event, Object, Func);
			UE_LOG(LogEnhancedInput, Log, TEXT("Binded Native Action: %s"), *InputAction->GetName());
		}
	}
	
	
	
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UMDAInputConfig* Config, UserClass* Object, PressedFuncType Pressed, ReleasedFuncType Released)
	{
		for (const FMDATaggedInputAction& Entry : Config->AbilityInputActions)
		{
			if (Entry.InputAction && Entry.InputTag.IsValid())
			{
				BindAction(Entry.InputAction, ETriggerEvent::Triggered,
					Object, Pressed, Entry.InputTag).GetHandle();
				BindAction(Entry.InputAction, ETriggerEvent::Completed,
					Object, Released, Entry.InputTag).GetHandle();
			}
		}
	}
	
	
};
