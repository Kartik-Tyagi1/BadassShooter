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

	/* Function to calculate the crosshair spread */
	void CrosshairSpread(float DeltaTime);

	/* Functions for the CrosshairShootTimer Handle*/
	void StartCrosshairShootTimer();
	UFUNCTION()
	void EndCrosshairShootTimer(); // Callback for StartCrosshairShootTimer()

	/* Functions for Automatic Fire */
	void FireButtonPressed();
	void FireButtonReleased();
	void StartAutoFireTimer();
	UFUNCTION()
	void AutoFireTimerReset(); // Callback for StartAutoFireTimer()

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/*------------------------------- CHARACTER COMPONENETS --------------------------------------------------------*/


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	/* Pickup widget for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;


	/*--------------------------------- LOOK AROUND RATES --------------------------------------------------------*/


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


	/*--------------------------------- WEAPON FIRE ------------------------------------------------------------*/


	/* Sound Cue for the Defualt Revolver*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	/* Muzzle Flash at the Barrel of right weapon or main rifle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFlash;

	/* Montage for firing weapon from the hip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HipFireMontage;

	/* Impact Particles when bullet hits something */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BulletImpactParticles;

	/* Boolean to be set when aiming button is pressed or released */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;


	/*--------------------------------- CAMERA FIELD OF VIEW --------------------------------------------------------*/


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
	

	/*--------------------------------- CROSSHAIR MOVEMENT --------------------------------------------------------*/

	/* Amount we spread the crosshairs by determined by the other factors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier;

	/* Amount we spread crosshairs by depending on movement speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/* Amount we spread crosshairs by depending on if character is in the air */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/* Amount we spread crosshairs by depending on if character is aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimingFactor;

	/* Amount we spread crosshairs by depending on if character is firing a weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshair, meta = (AllowPrivateAccess = "true"))
	float CrosshairFiringFactor;

	/* Variables to calculate the CrosshairFiringFactor*/
	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	/*--------------------------------- AUTOMATIC FIRE --------------------------------------------------------*/

	/* Boolean for if the player is pressing the fire button */
	bool bFireButtonPressed;

	/* Boolean to determine if the firing of the weapon shoudl continue */
	bool bShouldFire;

	/* Duration between each bullet fire */
	float AutomaticFireDuration;

	FTimerHandle FireTimer;



public:
	FORCEINLINE USpringArmComponent* GetCameraSpringArm() const { return CameraSpringArm; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

};
