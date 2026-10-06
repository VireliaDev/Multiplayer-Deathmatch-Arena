// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "MDAPlayerState.generated.h"

class UMDAAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class MDA_API AMDAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMDAPlayerState();
	
	
	/////////////////Ability System Component////////////////
protected:
	UPROPERTY()
	TObjectPtr<UMDAAbilitySystemComponent> AbilitySystemComponent;
	
	//Call From Constructor
	void CreateAbilitySystemComponent();
public:
	//Get the ASC from this PlayerState
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	/////////////////////////////////////////////////////////
	
};
