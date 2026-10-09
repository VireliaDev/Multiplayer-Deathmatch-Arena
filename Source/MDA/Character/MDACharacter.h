// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MDACharacter.generated.h"

class UMDACharacterMovementComponent;
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
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UMDACharacterMovementComponent> MDAMovementComponent;
	
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void RequestJump();
	void RequestJumpEnd();
	void RequestCrouch();
	void RequestCrouchEnd();
	void RequestSprint();
	void RequestSprintEnd();
	
	//Sprinting
public:
	bool IsSprinting() const;
	void SetIsSprinting(bool bNewSprinting);
protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	uint8 bIsSprinting:1;
	
	
	
public:
	//Ability System Component
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void InitAbilitySystemComponent();
	
};
