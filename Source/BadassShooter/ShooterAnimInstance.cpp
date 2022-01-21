// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"



UShooterAnimInstance::UShooterAnimInstance() :
	CharacterSpeed(0.f),
	bIsInAir(false),
	bIsMoving(false),
	bIsAiming(false),
	CharacterYawCurrentFrame(0.f),
	CharacterYawLastFrame(0.f),
	RootYawOffset(0.f)
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
	}

	TurnInPlace();
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	if (CharacterSpeed > 0)
	{

	}
	else
	{
		CharacterYawLastFrame = CharacterYawCurrentFrame;
		CharacterYawCurrentFrame = ShooterCharacter->GetActorRotation().Yaw;
		const float YawDelta{ CharacterYawCurrentFrame - CharacterYawLastFrame };

		RootYawOffset -= YawDelta;
	}
		

}
