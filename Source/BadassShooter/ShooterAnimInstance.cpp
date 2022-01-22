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
	AimingPitch(0.f),
	AimingYaw(0.f)
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

		// Determine the yaw direction of the character movement
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;;
	}

	SetAimOffsetValues();
}

void UShooterAnimInstance::SetAimOffsetValues()
{
	if (ShooterCharacter == nullptr) return;

	FRotator AimingRotation = UKismetMathLibrary::NormalizedDeltaRotator(ShooterCharacter->GetBaseAimRotation(), ShooterCharacter->GetActorRotation());
	AimingPitch = AimingRotation.Pitch;
	AimingYaw = AimingRotation.Yaw;
		
}
