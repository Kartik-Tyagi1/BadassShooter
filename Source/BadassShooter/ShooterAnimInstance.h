// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
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
	
	/* Used for the aim offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming, meta = (AllowPrivateAccess = "true"))
	float CharacterYawCurrentFrame;

	/* Used to keep the values when not moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming, meta = (AllowPrivateAccess = "true"))
	float CharacterYawLastFrame;

	/* Used for the aim offset */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Aiming, meta = (AllowPrivateAccess = "true"))
	float RootYawOffset;
	




};
