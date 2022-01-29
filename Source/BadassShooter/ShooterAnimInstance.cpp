// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UShooterAnimInstance::UShooterAnimInstance() :
	CharacterSpeed(0.f),
	bIsInAir(false),
	bIsMoving(false),
	bIsAiming(false),
	bIsInCombatPose(false),
	MovementOffsetYaw(0.f),
	MovementOffsetYawLastFrame(0.f),
	TIPCharacterYaw(0.f),
	TIPCharacterYawLastFrame(0.f),
	RootYawOffset(0.f),
	RotationCurve(0.f),
	RotationCurveLastFrame(0.f),
	AimingPitch(0.f),
	bIsReloading(false),
	OffsetState(EOffsetState::EOS_NonCombat),
	CharacterLean(FRotator(0.f)),
	CharacterLeanLastFrame(FRotator(0.f)),
	LeanYawDelta(0.f)
{}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		// Determine if reloading
		bIsReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;

		// Determine MovementSpeed 
		FVector Velocity = ShooterCharacter->GetCharacterMovement()->Velocity;
		Velocity.Z = 0;
		CharacterSpeed = Velocity.Size(); // Returns magnitude of lateral velocity vector

		// Determine bIsInAir
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		// Determine bIsMoving
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsMoving = true;
		}
		else
		{
			bIsMoving = false;
		}

		// Determine if Aiming
		bIsAiming = ShooterCharacter->GetIsAiming();

		bIsInCombatPose = ShooterCharacter->GetIsInCombatPose();

		// Determine the yaw direction of the character movement
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;;

		if (ShooterCharacter->GetVelocity().Size() > 0)
		{
			MovementOffsetYawLastFrame = MovementOffsetYaw;
		}

		if (bIsReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if(bIsInAir)
		{
			OffsetState = EOffsetState::EOS_Air;
		}
		else if (bIsAiming || bIsInCombatPose)
		{
			OffsetState = EOffsetState::EOS_Combat;
		}
		else
		{
			OffsetState = EOffsetState::EOS_NonCombat;
		}
	}

	TurnInPlace();
	Lean(DeltaTime);

}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	AimingPitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	if (CharacterSpeed > 0 || bIsInAir)
	{
		// Do not do any calcuations if character is moving or jumping.
		// We do not want to turn in place under these conditions
		RootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta); // Negative of Character Yaw (Description in .h file)

		const float Turning{ GetCurveValue(TEXT("Turning")) };
		if (Turning > 0)
		{
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

			/* RootYawOffset > 0 : LEFT TURN       RootYawOffset < 0 : RIGHT TURN */
			/* Once the turn in place animation triggers we need to use the delta of the curve values to orient the root bone to point in the direction of the turn */
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess{ ABSRootYawOffset - 90.f };
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;

	CharacterLeanLastFrame = CharacterLean;
	CharacterLean = ShooterCharacter->GetActorRotation();
	
	// Interp lean value to the targe delta betweeen the lean character yaws
	const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterLean, CharacterLeanLastFrame) };
	const float Target{ Delta.Yaw / DeltaTime };
	const float Interp{ FMath::FInterpTo(LeanYawDelta, Target, DeltaTime, 6.f) };

	// Clamp the value so that we don't fly off the screen
	LeanYawDelta = FMath::Clamp(Interp, -90.f, 90.f);

}


