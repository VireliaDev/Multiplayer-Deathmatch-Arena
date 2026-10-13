#pragma once

#include "CoreMinimal.h"
#include "GameFramework/RootMotionSource.h"
#include "RootMotionSource_MDAMantle.generated.h"

///MoveToForce, except better movement for mantling
USTRUCT()
struct FRootMotionSource_MDAMantle : public FRootMotionSource_MoveToForce
{
	GENERATED_BODY()

	// Flat direction towards the ledge. Horizontal motion follows this at GlideSpeed.
	UPROPERTY()
	FVector Forward = FVector::ForwardVector;

	// Constant horizontal speed through the mantle. 
	UPROPERTY()
	float GlideSpeed = 300.f;

	// Ease-out strength for the rise. Higher = more height gained early. 
	UPROPERTY()
	float RiseExponent = 3.f;

	virtual FRootMotionSource* Clone() const override;
	virtual bool Matches(const FRootMotionSource* Other) const override;
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FString ToSimpleString() const override;
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual void PrepareRootMotion(float SimulationTime, float MovementTickTime,
		const ACharacter& Character, const UCharacterMovementComponent& MoveComponent) override;
};

template<>
struct TStructOpsTypeTraits<FRootMotionSource_MDAMantle> : TStructOpsTypeTraitsBase2<FRootMotionSource_MDAMantle>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};