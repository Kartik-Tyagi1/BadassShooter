// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_NonCombat	UMETA(DisplayName = "Non Combat"),
	EOS_Combat		UMETA(DisplayName = "Combat"),
	EOS_Reloading	UMETA(DisplayName = "Reloading"),
	EOS_Air			UMETA(DisplayName = "In Air"),

	EOS_MAX			UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BADASSSHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UShooterAnimInstance();

	/* Begin Play of AnimInstance class */
	virtual void NativeInitializeAnimation() override;

	/* Tick of the AnimInstance Class */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

protected:
	void TurnInPlace();
	void Lean(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CharacterSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	/* Switch between combat and noncombat poses */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInCombatPose;

	/* True when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	/* True when reloading to prevent aim offset bullshit when reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;

	/* True when Equipping to prevent aim offset bullshit when reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bIsEquipping;

	/* Offset Yaw used for strafing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	/* Offset Yaw used for strafing right before movement stops so we can play the correct stopping animation  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYawLastFrame;

	/* Character Yaws used for Turn In Place Animations */
	float TIPCharacterYaw;
	float TIPCharacterYawLastFrame;

	/* 
	* This is the negative of the delta between the CharacterYaw and CharacterYawLastFrame to reorient the root bone when rotating the camera 
	* so that the character does not move with the camera until the turn in place animation plays 
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;

	/* Rotation curve values for turn in place */
	float RotationCurve;
	float RotationCurveLastFrame;

	/* Aiming Pitch for aim offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float AimingPitch;

	
	/* Offset State determines what aim offset to use in animation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState;

	/* Character Yaws used for Lean Animations */
	FRotator CharacterLean;
	FRotator CharacterLeanLastFrame;

	/* Delta for Leaning Animations */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Lean, meta = (AllowPrivateAccess = "true"))
	float LeanYawDelta;

	/* Recoil weight when firing weapon. Higher values mean that the recoil will be more and the reload animation will play properly) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight;

	/* Boolean to determine if the character is turning */
	bool bIsTurning;

};
