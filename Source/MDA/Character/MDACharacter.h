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

	
	//Movement
	UFUNCTION(BlueprintPure, Category = "MDA|Movement")
	UMDACharacterMovementComponent* GetMDAMovement() const { return MDAMovementComponent; }
protected:
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UMDACharacterMovementComponent> MDAMovementComponent;
	
	
	//Replicated Movement state information
public:
	void SetReplicatedMovementState(bool bSprinting, bool bAiming);
	UFUNCTION(BlueprintPure, Category = "MDA|Movement") 
	bool IsSprintingCosmetic() const;
	UFUNCTION(BlueprintPure, Category = "MDA|Movement") 
	bool IsAimingCosmetic() const;
private:
	UPROPERTY(Replicated) 
	bool bRepIsSprinting = false;
	UPROPERTY(Replicated) 
	bool bRepIsAiming = false;
	
	
protected:
	//Generic Movement
	void Input_Move(const FInputActionValue& Value);
	
	//Looking
	void Input_Look(const FInputActionValue& Value);
	
	//Jumping
	void RequestJump();
	void RequestJumpEnd();
	virtual bool CanJumpInternal_Implementation() const override;
	
	//Crouching
	void RequestCrouch();
	void RequestCrouchEnd() ;
	
	//Sprinting
	void RequestSprint();
	void RequestSprintEnd();

	//Aiming
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Aim") 
	float DefaultFOV = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Aim") 
	float AimFOV = 70.f;
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Aim") 
	float FOVInterpSpeed = 12.f;
	void RequestAimIn();
	void RequestAimOut();
	
	
	
public:
	//Ability System Component
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void InitAbilitySystemComponent();
	
};
