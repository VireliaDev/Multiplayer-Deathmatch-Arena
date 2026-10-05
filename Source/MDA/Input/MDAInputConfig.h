// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MDAInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FMDATaggedInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, meta=(Categories="InputTag"))
	FGameplayTag InputTag;
};



UCLASS(BlueprintType)
class UMDAInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty="InputTag"))
	TArray<FMDATaggedInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty="InputTag"))
	TArray<FMDATaggedInputAction> AbilityInputActions;

	const UInputAction* FindNativeActionForTag(const FGameplayTag& Tag) const
	{
		for (FMDATaggedInputAction NativeInputAction : NativeInputActions)
		{
			if (NativeInputAction.InputTag == Tag)
			{
				return NativeInputAction.InputAction;
			}
		}
		return nullptr;
	}
	
};



