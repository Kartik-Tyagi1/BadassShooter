// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Ammo.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	// Base Look Around Rate
	LookAroundRate(45.f),
	// Look Around Rates Keyboard/Controller
	HipLookAroundRate(90.f),
	AimingLookAroundRate(20.f),
	// Look Around Rates Mouse
	MouseHipLookAroundRate(1.f),
	MouseAimingLookAroundRate(0.2f),
	// True when Aiming
	bIsAiming(false),
	// Camera FOV
	CameraDefaultFOV(0.f),  // Set in BeginPlay
	CameraZoomedFOV(30.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(40.f),
	// Crosshair Spread Factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimingFactor(0.f),
	CrosshairFiringFactor(0.f),
	// CrosshairFiringFactor Factors
	ShootTimeDuration(0.05f),
	bFiringBullet(false),
	// Automatic Fire Variables
	CombatState(ECombatState::ECS_Unoccupied),
	bFireButtonPressed(false),
	AutomaticFireDuration(0.1f),
	bIsInCombatPose(false),
	bAimingButtonPressed(false),
	// Item trace variables
	bShouldTraceForItems(false),
	OverlappedItemCount(0),
	// Item Interpolation Variables
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	// Intial ammo amounts
	StartingPistolAmmo(75),
	StartingARAmmo(120),
	// Crouching
	bIsCrouching(false),
	// Movement Speeds
	NonCombatSpeed(600.f),
	CombatSpeed(500.f),
	CrouchingSpeed(300.f),
	// Capsule half heights
	StandingCapsuleHalfHeight(88.f),
	CrouchingCapsuleHalfHeight(44.f),
	// Ground Friction
	BaseGroundFriction(2.f),
	CrouchingGroundFriction(100.f),
	// Item Interpolation Sounds Limits
	bShouldPlayPickupSound(true),
	bShouldPlayEquipSound(true),
	PickupSoundWaitDuration(0.1f),
	EquipSoundWaitDuration(0.1f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a spring arm for the camera (pulls in toward the character when collisions occur with the character)
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 350.f;
	CameraSpringArm->SocketOffset = FVector{ 0.f, 70.f, 80.f };
	CameraSpringArm->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Create Camera attached to spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Camera does NOT rotate relative to the spring arm

	// Do not rotate character with camera
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false; // Character will move in camera direction and rotate ...
	GetCharacterMovement()->RotationRate = FRotator{ 0.f, 600.f, 0.f }; // At this rate
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Set HandSceneComponent (Attachment is setup in GrabMagazine)
	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));

	// Setup Interpolation Components
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpComp"));
	WeaponInterpComp->SetupAttachment(GetCamera());

	InterpComp_1 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp_1"));
	InterpComp_1->SetupAttachment(GetCamera());

	InterpComp_2 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp_2"));
	InterpComp_2->SetupAttachment(GetCamera());

	InterpComp_3 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp_3"));
	InterpComp_3->SetupAttachment(GetCamera());

	InterpComp_4 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp_4"));
	InterpComp_4->SetupAttachment(GetCamera());

	InterpComp_5 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp_5"));
	InterpComp_5->SetupAttachment(GetCamera());

	InterpComp_6 = CreateDefaultSubobject<USceneComponent>(TEXT("InterpComp_6"));
	InterpComp_6->SetupAttachment(GetCamera());

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->PlayerCameraManager->ViewPitchMax = 10.f;  // Bottom of Character
	}

	if (Camera)
	{
		CameraDefaultFOV = GetCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	// Spawn and attach the default weapon to the character mesh
	EquipWeapon(SpawnDefaultWeapon());

	// Set up the ammo map with the starting ammo values
	InitalizeAmmoMap();

	// Set Character Speed
	GetCharacterMovement()->MaxWalkSpeed = NonCombatSpeed;

	// Setup the interp location for weapon and item pickups
	InitializeInterpLocations();

}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetCameraFOV(DeltaTime);
	SetLookRates();
	CrosshairSpread(DeltaTime);
	TraceForItems();
	InterpCapsuleHalfHeight(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Axis Mappings
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("MouseTurn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("MouseLookUp", this, &AShooterCharacter::LookUp);

	// Action Mapping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("InteractButton", IE_Pressed, this, &AShooterCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction("InteractButton", IE_Released, this, &AShooterCharacter::InteractButtonReleased);

	PlayerInputComponent->BindAction("ReloadButton", IE_Pressed, this, &AShooterCharacter::ReloadButtonPressed);

	PlayerInputComponent->BindAction("SwitchCombatButton", IE_Pressed, this, &AShooterCharacter::SwitchCombatButtonPressed);

	PlayerInputComponent->BindAction("CrouchButton", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);


}

void AShooterCharacter::SetCameraFOV(float DeltaTime)
{
	if (bIsAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, CameraZoomInterpSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, CameraZoomInterpSpeed);
	}

	GetCamera()->SetFieldOfView(CameraCurrentFOV);

}

void AShooterCharacter::SetLookRates()
{
	if (bIsAiming)
	{
		LookAroundRate = AimingLookAroundRate;
	}
	else
	{
		LookAroundRate = HipLookAroundRate;
	}
}

void AShooterCharacter::MoveForward(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0))
	{
		// Get Rotation of Controller and get is Yaw
		FRotator ControllerRotation{ Controller->GetControlRotation() };
		FRotator CharacterYaw{ 0.f, ControllerRotation.Yaw, 0.f };

		// Create a vector pointing in the direction of the Yaw of the rotation (in this case X is forward)
		const FVector Direction{ FRotationMatrix{CharacterYaw}.GetUnitAxis(EAxis::X)};

		// Set Forward movement in the calculated direction
		AddMovementInput(Direction, AxisValue);
	}
}


void AShooterCharacter::MoveRight(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0))
	{
		// Get Rotation of Controller and get is Yaw
		FRotator ControllerRotation{ Controller->GetControlRotation() };
		FRotator CharacterYaw{ 0.f, ControllerRotation.Yaw, 0.f };

		// Create a vector pointing in the direction of the Yaw of the rotation (in this case Y is right)
		const FVector Direction{ FRotationMatrix{CharacterYaw}.GetUnitAxis(EAxis::Y)};

		// Set Forward movement in the calculated direction
		AddMovementInput(Direction, AxisValue);
	} 
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	// Calculates Yaw input in degrees/frame
	AddControllerYawInput(Rate * LookAroundRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpRate(float Rate)
{
	// Calculates Pitch input in degrees/frame
	AddControllerPitchInput(Rate * LookAroundRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::Turn(float Value)
{
	float TurnRate{};
	if (bIsAiming)
	{
		TurnRate = MouseAimingLookAroundRate;
	}
	else
	{
		TurnRate = MouseHipLookAroundRate;
	}
	AddControllerYawInput(Value * TurnRate);
}

void AShooterCharacter::LookUp(float Value)
{
	float LookUpRate{};
	if (bIsAiming)
	{
		LookUpRate = MouseAimingLookAroundRate;
	}
	else
	{
		LookUpRate = MouseHipLookAroundRate;
	}
	AddControllerPitchInput(Value * LookUpRate);
}


float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}



void AShooterCharacter::CrosshairSpread(float DeltaTime)
{
	// Calculate the Crosshair Velocity Factor by mapping walkspeed to normalized range
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityFactorRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityFactorRange, Velocity.Size());

	// Calculate CrosshairInAir Factor by interpolation when in the air
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate CrosshairAimFactor by Interpolation when aiming
	if (bIsAiming)
	{
		CrosshairAimingFactor = FMath::FInterpTo(CrosshairAimingFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimingFactor = FMath::FInterpTo(CrosshairAimingFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate CrosshairFiringFactor by interpolation based on if firing
	if (bFiringBullet)
	{
		CrosshairFiringFactor = FMath::FInterpTo(CrosshairFiringFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairFiringFactor = FMath::FInterpTo(CrosshairFiringFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimingFactor + CrosshairFiringFactor;
}

void AShooterCharacter::StartCrosshairShootTimer()
{
	bFiringBullet = true;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::EndCrosshairShootTimer, ShootTimeDuration);
}

void AShooterCharacter::EndCrosshairShootTimer()
{
	bFiringBullet = false;
}


void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading)
	{
		Aim();
	}

}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::Aim()
{
	bIsAiming = true;
}

void AShooterCharacter::StopAiming()
{
	bIsAiming = false;
}


void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	FireWeapon();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartAutoFireTimer()
{
	CombatState = ECombatState::ECS_FireTImerInProgress;
	GetWorldTimerManager().SetTimer(FireTimer, this, &AShooterCharacter::AutoFireTimerReset, AutomaticFireDuration);
}

void AShooterCharacter::AutoFireTimerReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			FireWeapon();
		}
	}
	else
	{
		ReloadWeapon();
	}
}


void AShooterCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo() && (bIsInCombatPose))
	{
		PlayFireSound();
		SendBullet();
		PlayGunFireMontage();
		EquippedWeapon->DecrementAmmo();
		StartCrosshairShootTimer();
		StartAutoFireTimer();
	}
}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
		// Check for crosshair trace hit
		FHitResult CrosshairHitResult;
		bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

		if (bCrosshairHit)
		{
			// Tentative beam location - still need to trace from gun
			OutBeamLocation = CrosshairHitResult.Location;
		}
		else // no crosshair trace hit
		{
			// OutBeamLocation is the End location for the line trace
		}

		// Do second line trace from weapon barrel so determine if anything was hit in between
		FHitResult WeaponTraceHitResult;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
		const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };	// End of Line Trace (which is the end of the previous line trace)

		GetWorld()->LineTraceSingleByChannel(WeaponTraceHitResult, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
		if (WeaponTraceHitResult.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHitResult.Location;
			return true;
		}

	return false;
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.Actor);
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				// Show Item's Pickup Widget
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();
			}

			// We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame from last frame
					// Or AItem is null.
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
			// Store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		// No longer overlapping any items,
		// Item last frame should not show widget
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AShooterCharacter::PlayFireSound()
{
	// Play Weapon Fire Sound
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}

void AShooterCharacter::SendBullet()
{
	// Send Bullet
	const USkeletalMeshSocket* BarrelSocket_1 = EquippedWeapon->GetItemMesh()->GetSocketByName(TEXT("MuzzleFlashSocket"));

	if (BarrelSocket_1)
	{
		FTransform BarrelSocketTransform_1 = BarrelSocket_1->GetSocketTransform(EquippedWeapon->GetItemMesh());
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketTransform_1);
		}

		FVector BeamEnd_1;
		FVector BeamEnd_2;
		bool bBeamEndLocation_1 = GetBeamEndLocation(BarrelSocketTransform_1.GetLocation(), BeamEnd_1);

		if (bBeamEndLocation_1)
		{
			if (BulletImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactParticles, BeamEnd_1);
			}
		}
	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	// Play Gun Fire Montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(TEXT("StartFire"));
	}
}




void AShooterCharacter::InteractButtonPressed()
{

	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurveInterpTimer(this);

		// Value would stay is user moved too quickly so the item needs to be to null manully
		TraceHitItem = nullptr; 

	}
	
}

void AShooterCharacter::InteractButtonReleased()
{
}


AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		// Spawn the default weapon in the world 
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);		
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// Get the RightHandSocket on the Mesh
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			// Attachec the weapon to the hand
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	// See if ammo map contains the type of ammo
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		// Add the amount of ammo to the amount carried 
		int32 AmmoCount = AmmoMap[Ammo->GetAmmoType()];
		AmmoCount += Ammo->GetItemAmount();
		// Set the ammo map amount to the new added amount
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		if (EquippedWeapon->GetAmmoInMagazine() == 0)
		{
			ReloadWeapon();
		}
	}


	Ammo->Destroy();
}


FVector AShooterCharacter::GetCameraInterpEndLocation()
{
	const FVector CameraStartLocation{ Camera->GetComponentLocation() };
	const FVector CameraForwardDirection{ Camera->GetForwardVector() };

	return CameraStartLocation + CameraForwardDirection * CameraInterpDistance + FVector{ 0.f, 0.f, CameraInterpElevation };
}


void AShooterCharacter::GetPickupItem(AItem* Item) 
{
	Item->PlayEquipSound();

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

void AShooterCharacter::InitalizeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_Pistol, StartingPistolAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmoInMagazine() > 0;
}



void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquippedWeapon == nullptr) return;

	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull())
	{
		if (bIsAiming) // Stop Aiming when reloading and when finished reloading the aim will go back
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadMontageSectionName());
		}
	}

}

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;

	if (bAimingButtonPressed)
	{
		Aim();
	}

	// Update the ammo map with the reloaded ammo
	const auto AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		int32 CarriedAmmo = AmmoMap[AmmoType];
		const int32 MagazineEmptySpace = EquippedWeapon->GetMaximumMagazineCapacity() - EquippedWeapon->GetAmmoInMagazine();
		if (MagazineEmptySpace > CarriedAmmo)
		{
			CarriedAmmo = 0;
			EquippedWeapon->UpdateAmmo(MagazineEmptySpace);
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			CarriedAmmo -= MagazineEmptySpace;
			EquippedWeapon->UpdateAmmo(MagazineEmptySpace);
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
	
}


bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	auto AmmoType = EquippedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}

void AShooterCharacter::GrabMagazine()
{
	if (EquippedWeapon == nullptr) return;
	if (HandSceneComponent == nullptr) return;

	// Get the magazine bone transform by getting the bone index then the transform
	int32 MagBoneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetWeaponMagBoneName());
	WeaponMagTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(MagBoneIndex);

	// Create Attachment Rules and attach the HandSceneComponent to the character's left hand
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));

	// Set the HandSceneComponent transform to the magazine so that it follows the movement of the hand
	HandSceneComponent->SetWorldTransform(WeaponMagTransform);

	EquippedWeapon->SetIsMagMoving(true);

}

void AShooterCharacter::ReplaceMagazine()
{
	EquippedWeapon->SetIsMagMoving(false);
}

void AShooterCharacter::SwitchCombatButtonPressed()
{
	if (!bIsCrouching)
	{
		bIsInCombatPose = !bIsInCombatPose;
		if (bIsInCombatPose)
		{
			GetCharacterMovement()->MaxWalkSpeed = CombatSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = NonCombatSpeed;
		}
	}
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bIsCrouching = !bIsCrouching;
		bIsInCombatPose = true;
	}
	if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = CombatSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::Jump()
{
	if (bIsCrouching)
	{
		bIsCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = CombatSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
	else
	{
		ACharacter::Jump();
	}
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bIsCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 15.f ) };

	// The mesh will dip into the floor when crouching so we need to set an offset so we can move the character out of the floor
	// Negative when crouching, Positive when standing 
	const float DeltaHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.f, 0.f, -DeltaHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::InitializeInterpLocations()
{
	InterpLocations.Add(FInterpLocation{ WeaponInterpComp, 0 });
	InterpLocations.Add(FInterpLocation{ InterpComp_1, 0 });
	InterpLocations.Add(FInterpLocation{ InterpComp_2, 0 });
	InterpLocations.Add(FInterpLocation{ InterpComp_3, 0 });
	InterpLocations.Add(FInterpLocation{ InterpComp_4, 0 });
	InterpLocations.Add(FInterpLocation{ InterpComp_5, 0 });
	InterpLocations.Add(FInterpLocation{ InterpComp_6, 0 });
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 index)
{
	if (index <= InterpLocations.Num())
	{
		return InterpLocations[index];
	}
	return FInterpLocation();
}

int32 AShooterCharacter::GetInterpLocationsLowestItemIndex()
{
	int32 LowestItemIndex = 1;
	int32 LowestItemCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestItemCount)
		{
			LowestItemCount = InterpLocations[i].ItemCount;
			LowestItemIndex = i;
		}
	}

	return LowestItemIndex;
}

void AShooterCharacter::IncrementInterpLocationsItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1 || Amount == 0) return; // Should only increment or decrement by +- 1

	InterpLocations[Index].ItemCount += Amount;
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(PickupSoundTimer, this, &AShooterCharacter::EndPickupSoundTimer, PickupSoundWaitDuration);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(EquipSoundTimer, this, &AShooterCharacter::EndEquipSoundTimer, EquipSoundWaitDuration);
}


void AShooterCharacter::EndPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::EndEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}
