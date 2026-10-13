#include "RootMotionSource_MDAMantle.h"

#include "GameFramework/Character.h"


FRootMotionSource* FRootMotionSource_MDAMantle::Clone() const
{
    return new FRootMotionSource_MDAMantle(*this);
}

bool FRootMotionSource_MDAMantle::Matches(const FRootMotionSource* Other) const
{
    if (!Super::Matches(Other))
    {
        return false;
    }
    const FRootMotionSource_MDAMantle* OtherMantle = static_cast<const FRootMotionSource_MDAMantle*>(Other);

    return Forward.Equals(OtherMantle->Forward, 0.01f)
        && FMath::IsNearlyEqual(GlideSpeed, OtherMantle->GlideSpeed)
        && FMath::IsNearlyEqual(RiseExponent, OtherMantle->RiseExponent);
}

UScriptStruct* FRootMotionSource_MDAMantle::GetScriptStruct() const
{
    return StaticStruct();
}

FString FRootMotionSource_MDAMantle::ToSimpleString() const
{
    return FString::Printf(TEXT("[ID:%u]FRootMotionSource_MDAMantle %s"), LocalID, *InstanceName.GetPlainNameString());
}

bool FRootMotionSource_MDAMantle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    if (!Super::NetSerialize(Ar, Map, bOutSuccess)) { return false; }
    Ar << Forward;
    Ar << GlideSpeed;
    Ar << RiseExponent;
    bOutSuccess = true;
    return true;
}

void FRootMotionSource_MDAMantle::PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent)
{
    RootMotionParams.Clear();

    if (Duration > UE_SMALL_NUMBER && MovementTickTime > UE_SMALL_NUMBER)
    {
        const float Alpha = FMath::Clamp((GetTime() + SimulationTime) / Duration, 0.f, 1.f);

        // Vertical: position-driven with a strong ease-out, so most of the height
        // comes early and the feet clear the lip quickly
        const float ZAlpha = FMath::InterpEaseOut(0.f, 1.f, Alpha, FMath::Max(RiseExponent, 1.f));
        const float DesiredZ = FMath::Lerp(StartLocation.Z, TargetLocation.Z, ZAlpha);
        
        // Horizontal: constant momentum, no target chasing. While the wall blocks us
        // the CMC slides us up its face; once clear, we glide straight over.
        FVector Velocity;
        Velocity.X = Forward.X * GlideSpeed;
        Velocity.Y = Forward.Y * GlideSpeed;
        Velocity.Z = (DesiredZ - Character.GetActorLocation().Z) / MovementTickTime;

        RootMotionParams.Set(FTransform(Velocity));
    }

    SetTime(GetTime() + SimulationTime);
}