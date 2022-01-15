// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_Pistol	UMETA(DisplayName = "Pistol"),
	EAT_AR		UMETA(DisplayName = "AssaultRifle"),

	EAT_MAX		UMETA(DisplayName = "DefaultMAX")
};

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
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);
	void TraceForItems();

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


	/* Functions for Interacting (pickup/select) Items/Actors in game */
	void InteractButtonPressed();
	void InteractButtonReleased();

	/* Function to spawn and attach the default weapon */
	class AWeapon* SpawnDefaultWeapon();

	/* Function to equip a weapon */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/* Function to drop currently equipped weapon */
	void DropWeapon();

	/* Function to swap weapons */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/* Sets up inital ammo in the ammo map */
	void InitalizeAmmoMap();

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

	/*--------------------------------- THE WEAPON AND TRACING FOR ITEMS --------------------------------------------------------*/

	/* Currently Equipped Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	/* Reference to blueprint weapon class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/* True if we should trace every frame for items */
	bool bShouldTraceForItems;
	
	/* Number of overlapped AItems */
	int8 OverlappedItemCount;

	/* The AItem we hit last frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	/* The item currently hit by our trace in TraceForItems (could be null) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/*------------------- INTERPOLATION FOR ITEMS (THE WAY THEY MOVE UP AND DOWN WHEN EQUIPPING) -----------------------------------*/

	/* Distance forward from the camera (front of the camera where items will travel to) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance;

	/* Distance up from the camera (up from the camera where items will travel to) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation;

	/*------------------------------------------------- AMMO VARIABLES -------------------------------------------------------------*/

	/* Maps the type of weapon to the amount of ammo we have available on the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/* Maps the type of weapon to the amount of ammo we have available on the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	int32 StartingPistolAmmo;

	/* Maps the type of weapon to the amount of ammo we have available on the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo;



public:
	FORCEINLINE USpringArmComponent* GetCameraSpringArm() const { return CameraSpringArm; }
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	/* Adds/subtracts to/from OverlappedItemCount and updates bShouldTraceForItems */
	void IncrementOverlappedItemCount(int8 Amount);

	/* Get the end location for the item interpolation(basically a location in front and above the camera) */ 
	FVector GetCameraInterpEndLocation();

	/* Determine what type of item is the pickup item and call the corresponding interact function (SwapWeapon, etc.)*/
	void GetPickupItem(AItem* Item);
};
