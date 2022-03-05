// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"



UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied				UMETA(DisplayName = "Unoccupied"),
	ECS_FireTImerInProgress		UMETA(DisplayName = "FireTImerInProgress"),
	ECS_Reloading				UMETA(DisplayName = "Reloading"),

	ECS_MAX						UMETA(DisplayName = "DefaultMAX")

};

USTRUCT(BlueprintType) 
struct FInterpLocation
{
	GENERATED_BODY()
	
	/* Location to where item will interp to*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	/* Number of items interping to the above location */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
		
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
	void Aim();
	void StopAiming();

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

	/* Item Pickup Functions */
	void SwapWeapon(AWeapon* WeaponToSwap);
	void PickupAmmo(class AAmmo* Ammo);

	/* Sets up inital ammo in the ammo map */
	void InitalizeAmmoMap();

	/* Check if weapon has ammo */
	bool WeaponHasAmmo();

	/* Fire Weapon Functions */
	void PlayFireSound();
	void SendBullet();
	void PlayGunFireMontage();

	/* Reloading Functions */
	UFUNCTION(BlueprintCallable) // Called from blueprint (reload montage anim notify)
	void FinishReloading();

	UFUNCTION(BlueprintCallable) // Called from blueprint (reload montage anim notify)
	void GrabMagazine();

	UFUNCTION(BlueprintCallable) // Called from blueprint (reload montage anim notify)
	void ReplaceMagazine();

	/* Function for weapon reloading and ammo checking*/
	void ReloadButtonPressed();
	void ReloadWeapon();
	bool CarryingAmmo();

	/* Function to chnage from noncombat to combat poses */
	void SwitchCombatButtonPressed();

	/* Function to toggle between crouch and standing */
	void CrouchButtonPressed();

	/* Function to add own functionality to jump */
	virtual void Jump() override;

	/* Function for Capsult Half Height Interpolation */
	void InterpCapsuleHalfHeight(float DeltaTime);

	/* Intialize the Interp Location array */
	void InitializeInterpLocations();


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

	/*--------------------------------- WEAPON FIRE AND RELOADING  --------------------------------------------------------*/

	/* Combat state of the character (determines if player and shoot/reload) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	/* Boolean for if the player is pressing the fire button */
	bool bFireButtonPressed;

	/* Duration between each bullet fire */
	float AutomaticFireDuration;

	FTimerHandle FireTimer;

	/* Reloading Animation (section depends on the weapon type) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadMontage;

	/* Switch between combat and noncombat poses */
	bool bIsInCombatPose;

	bool bAimingButtonPressed;

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

	/* Transform of the weapon magazine */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponMagTransform;

	/* Scene component that will have the transform of the weapon magazine but will be attached to the hand of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;

	/*------------------------------------------------- CROUCHING / MOVEMENT VARIABLES -------------------------------------------------------------*/

	/* True when crouching */
	bool bIsCrouching;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float NonCombatSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CombatSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingSpeed;

	/* Variables to interpolate capsule height when crouching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingCapsuleHalfHeight;

	/* Change ground friction when crouching so there is not skid effect */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchingGroundFriction;

	/*------------------------------------------------- Pickup Interpolation -------------------------------------------------------------*/

	/* Interpolation Locations for Pickups */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* WeaponInterpComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp_1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp_2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp_3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp_4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp_5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USceneComponent* InterpComp_6;

	/* Array of interp location */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;



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

	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsInCombatPose() const { return bIsInCombatPose; } // Blueprint callable because it is used in crosshair HUD

	FORCEINLINE bool GetIsCrouching() const { return bIsCrouching; }

	/* Returns the InterpLocation in the InterpLocations array */
	FInterpLocation GetInterpLocation(int32 index);
	
	/* Returns the index of the InterpLocation with the lowest Item Count*/
	int32 GetInterpLocationsLowestItemIndex();

	/* Increments the item count of the InterpLocation being used when interping */
	void IncrementInterpLocationsItemCount(int32 Index, int32 Amount);
};
