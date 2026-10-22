// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MDA/AbilitySystem/Abilities/MDAAbilitySet.h"
#include "MDACharacter.generated.h"

class AMDAPlayerState;
struct FGameplayTag;
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
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Mesh", meta=( AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
	
	
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
	void SetReplicatedMovementState(bool bSprinting, bool bAiming, bool bMantling);
	UFUNCTION(BlueprintPure, Category = "MDA|Movement") 
	bool IsSprintingCosmetic() const;
	UFUNCTION(BlueprintPure, Category = "MDA|Movement") 
	bool IsAimingCosmetic() const;
	UFUNCTION(BlueprintPure, Category = "MDA|Movement")
	bool IsMantlingCosmetic() const;
	
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FVector_NetQuantize10 ReplicatedAcceleration;
private:
	UPROPERTY(Replicated) 
	bool bRepIsSprinting = false;
	UPROPERTY(Replicated) 
	bool bRepIsAiming = false;
	UPROPERTY(Replicated)
	bool bRepIsMantling = false;
	
	

	UFUNCTION()
	void OnRep_ReplicatedAcceleration();
	
	
	
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
	
	//Shooting
	void RequestShoot();
	void RequestShootEnd();
	
	
	//Abilities
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Abilities")
	FMDAGameAbilitySet AbilitySet;
	
	UPROPERTY()
	FGameAbilitySetHandles GrantedHandles;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "MDA|Abilities")
	void GiveAbilities();
	
	void Input_AbilityPressed(FGameplayTag InputTag);
	void Input_AbilityReleased(FGameplayTag InputTag);
	
	
	
public:
	//Ability System Component
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void InitAbilitySystemComponent();
	
	//Player State
	AMDAPlayerState* GetMDAPlayerState() const;
	
};
