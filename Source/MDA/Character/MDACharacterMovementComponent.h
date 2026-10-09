// Copyright (c) 2026 Sidney Levin (VireliaDev). Licensed under the MIT License.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MDACharacterMovementComponent.generated.h"



UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MDA_API UMDACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UMDACharacterMovementComponent();
	
protected:
	////ListenServer Animation Fix////
	virtual void TickCharacterPose(float DeltaTime) override;
	
public:
	////Input -> Intent////
	void SetSprintHeld(bool bHeld);
	
	
	////Movement State////
	bool IsSprinting() const { return bIsSprinting; }
	
	
	////Intent byte////
	uint8 PackIntents() const;
	void UnpackIntents(uint8 Packed);
	
	
	////Base Overrides////
	virtual float GetMaxSpeed() const override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
protected:
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	virtual bool ClientUpdatePositionAfterServerUpdate() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Sprint", meta = (ClampMin = "1"))
	float SprintSpeedMultiplier = 1.3f;

	// 0.5 = within 60 degrees of forward
	UPROPERTY(EditDefaultsOnly, Category = "MDA|Sprint", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SprintForwardThreshold = 0.5f;
	
	
private:
	bool IsForwardInput() const;
	
	////Intents////
	bool bSprintHeld = false;
	bool bCrouchHeld = false;
	bool bAimHeld = false;
	bool bJumpHeld = false;
	bool bSprintAfterCrouch = false;
	bool bSprintAfterAim = false;
	bool bWeaponSuppress = false;

	//// Local-only intent to control bWeaponSuppress ////
	bool bWantsToFire = false;
	bool bWeaponBusy = false;

	////Internal Movement States////
	bool bIsSprinting = false;
	bool bIsAiming = false;
	
	
	
	// Intents byte that travels in every move.
	enum EMDAIntent : uint8
	{
		SprintHeld        = 1 << 0,
		CrouchHeld        = 1 << 1,
		AimHeld           = 1 << 2,
		JumpHeld          = 1 << 3,
		SprintAfterCrouch = 1 << 4,
		SprintAfterAim    = 1 << 5,
		WeaponSuppress    = 1 << 6,
	};
	
	// Custom move data extending the base to carry Intents byte as well
	struct FMDANetworkMoveData : public FCharacterNetworkMoveData
	{
		using Super = FCharacterNetworkMoveData;

		uint8 Intents = 0;

		virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override
		{
			Super::ClientFillNetworkMoveData(ClientMove, MoveType);
			Intents = static_cast<const FMDASavedMove&>(ClientMove).SavedIntents;
		}
		virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override
		{
			Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);
			Ar << Intents;
			return !Ar.IsError();
		}
	};

	// Custom data container replacing the base moves with my custom moves (new, pending, old).
	struct FMDANetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
	{
		FMDANetworkMoveData MDAMoves[3];

		FMDANetworkMoveDataContainer()
		{
			NewMoveData     = &MDAMoves[0];
			PendingMoveData = &MDAMoves[1];
			OldMoveData     = &MDAMoves[2];
		}
	};
	
	class FMDASavedMove : public FSavedMove_Character
	{
	public:
		using Super = FSavedMove_Character;

		uint8 SavedIntents = 0;
		bool bSavedWasSprinting = false;

		virtual void Clear() override
		{
			Super::Clear();
			SavedIntents = 0;
			bSavedWasSprinting = false;
		}

		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override
		{
			Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
			UMDACharacterMovementComponent* CMC = CastChecked<UMDACharacterMovementComponent>(C->GetCharacterMovement());
			SavedIntents = CMC->PackIntents();
			bSavedWasSprinting = CMC->IsSprinting();
		}

		virtual void PrepMoveFor(ACharacter* C) override
		{
			Super::PrepMoveFor(C);
			UMDACharacterMovementComponent* CMC = CastChecked<UMDACharacterMovementComponent>(C->GetCharacterMovement());
			CMC->UnpackIntents(SavedIntents);
			CMC->bIsSprinting = bSavedWasSprinting;
		}

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* C, float MaxDelta) const override
		{
			const FMDASavedMove* Other = static_cast<const FMDASavedMove*>(NewMove.Get());
			if (SavedIntents != Other->SavedIntents)
			{
				return false;
			}
			if (bSavedWasSprinting != Other->bSavedWasSprinting)
			{
				return false;
			}
			return Super::CanCombineWith(NewMove, C, MaxDelta);
		}
	};
	class FMDANetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		using Super = FNetworkPredictionData_Client_Character;

		explicit FMDANetworkPredictionData_Client(const UCharacterMovementComponent& CMC) : Super(CMC) {}

		virtual FSavedMovePtr AllocateNewMove() override
		{
			return MakeShared<FMDASavedMove>();
		}
	};
	
	FMDANetworkMoveDataContainer MDAMoveDataContainer;
};




