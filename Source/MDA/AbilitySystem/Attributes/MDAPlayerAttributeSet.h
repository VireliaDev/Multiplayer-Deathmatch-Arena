// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "MDAPlayerAttributeSet.generated.h"
#include <MDA/AbilitySystem/MDAAbilitySystemComponent.h>

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class MDA_API UMDAPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMDAPlayerAttributeSet();

	UMDAAbilitySystemComponent* GetGameAbilitySystemComponent() const
	{
		return Cast<UMDAAbilitySystemComponent>(GetOwningAbilitySystemComponent());
	}


	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



	////Replicated////
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMDAPlayerAttributeSet, Health)

		UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMDAPlayerAttributeSet, MaxHealth)




protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& Old);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& Old);


};
