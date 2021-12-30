// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class BADASSSHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Functions to move character forward, back, left and right */
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	/* Functions to move the camera around with the arrow keys and controller right stick 
	* @param Rate: Normalized movement rate in degrees/sec (Value of 1.0 is equal to 100%) which comes from axis input value
	*/
	void TurnAtRate(float Rate);
	void LookUpRate(float Rate);

	/* Called when fire button is pressed */
	void FireWeapon();
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/* Functions for aiming the weapon */
	void AimingButtonPressed();
	void AimingButtonReleased();

	/* Set Camera FOV */
	void SetCameraFOV(float DeltaTime);

	/* Set Look Rates based on if character is aiming */
	void SetLookRates();

	/*
	* Functions to control turning and looking up rates when using the mouse
	* @param Value: The input from the mouse
	*/
	void Turn(float Value);
	void LookUp(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	/* Value that determines the speed at which player can look around with arrow keys and controller right stick */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float LookAroundRate;

	/* Value that determines the speed at which player can look around with arrow keys and controller right stick when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float HipLookAroundRate;

	/* Value that determines the speed at which player can look around with arrow keys and controller right stick when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float AimingLookAroundRate;

	/* Value that determines the speed at which player can look around with the mouse when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookAroundRate;

	/* Value that determines the speed at which player can look around with the mouse when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookAroundRate;

	/* Sound Cue for the Defualt Revolver*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/* Muzzle Flash at the Barrel of right weapon or main rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash_Right;

	/* Muzzle Flash at the Barrel of left weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash_Left;

	/* Montage for firing weapon from the hip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/* Impact Particles when bullet hits something */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BulletImpactParticles;

	/* Boolean to be set when aiming button is pressed or released */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	/* Default Camera Field of View */
	float CameraDefaultFOV;

	/* Default Camera Field of View */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV;

	/* Current Camera Field of View */
	float CameraCurrentFOV;

	/* Speed to Interp between Camera FOVs */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraZoomInterpSpeed;


public:
	FORCEINLINE USpringArmComponent* GetCameraSpringArm() const { return CameraSpringArm; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

};
