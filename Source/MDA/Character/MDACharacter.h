// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MDACharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UMDAInputConfig;
class UInputAction;
struct FInputActionValue;

UCLASS()
class MDA_API AMDACharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()


public:
	AMDACharacter(const FObjectInitializer& ObjectInitializer);	
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCamera;
	
	
	
	//Input
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UMDAInputConfig> InputConfig;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	//Movement
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void RequestJump();
	void RequestJumpEnd();
	void RequestCrouch();
	void RequestCrouchEnd();
	void RequestSprint();
	void RequestSprintEnd();
	
	
public:
	//Ability System Component
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void InitAbilitySystemComponent();
	
};
