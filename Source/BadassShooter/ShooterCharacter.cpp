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
	bFireButtonPressed(false),
	bShouldFire(true),
	AutomaticFireDuration(0.1f),
	// Item trace variables
	bShouldTraceForItems(false),
	OverlappedItemCount(0),
	// Item Interpolation Variables
	CameraInterpDistance(250.f),
	CameraInterpElevation(65.f),
	// Intial ammo amounts
	StartingPistolAmmo(75),
	StartingARAmmo(120)
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
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character will move in camera direction and rotate ...
	GetCharacterMovement()->RotationRate = FRotator{ 0.f, 600.f, 0.f }; // At this rate
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.2f;


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

}


// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetCameraFOV(DeltaTime);
	SetLookRates();
	CrosshairSpread(DeltaTime);
	TraceForItems();

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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("InteractButton", IE_Pressed, this, &AShooterCharacter::InteractButtonPressed);
	PlayerInputComponent->BindAction("InteractButton", IE_Released, this, &AShooterCharacter::InteractButtonReleased);

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
	bIsAiming = true;

	// Do not rotate character with camera when aiming
	bUseControllerRotationRoll = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

}

void AShooterCharacter::AimingButtonReleased()
{
	bIsAiming = false;

	// Go back to defaults when back to normal
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

}


void AShooterCharacter::FireButtonPressed()
{
	if (WeaponHasAmmo())
	{
		bFireButtonPressed = true;
		StartAutoFireTimer();
	}
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartAutoFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(FireTimer, this, &AShooterCharacter::AutoFireTimerReset, AutomaticFireDuration);
	}
}

void AShooterCharacter::AutoFireTimerReset()
{
	if (WeaponHasAmmo())
	{
		bShouldFire = true;
		if (bFireButtonPressed)
		{
			StartAutoFireTimer();
		}
	}
}


void AShooterCharacter::FireWeapon()
{
		
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket_1 = GetMesh()->GetSocketByName(TEXT("Muzzle_01"));

	if (BarrelSocket_1)
	{
		FTransform BarrelSocketTransform_1 = BarrelSocket_1->GetSocketTransform(GetMesh());
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

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(TEXT("StartFire"));
	}

	StartCrosshairShootTimer();

	if (EquippedWeapon)
	{
		// Subtract one from ammo in the magazine
		EquippedWeapon->DecrementAmmo();
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
			}

			// We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame from last frame
					// Or AItem is null.
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
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




void AShooterCharacter::InteractButtonPressed()
{

	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurveInterpTimer(this);
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


FVector AShooterCharacter::GetCameraInterpEndLocation()
{
	const FVector CameraStartLocation{ Camera->GetComponentLocation() };
	const FVector CameraForwardDirection{ Camera->GetForwardVector() };

	return CameraStartLocation + CameraForwardDirection * CameraInterpDistance + FVector{ 0.f, 0.f, CameraInterpElevation };
}


void AShooterCharacter::GetPickupItem(AItem* Item) 
{
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
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


