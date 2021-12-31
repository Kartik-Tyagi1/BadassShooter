// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"

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
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(40.f),
	// Crosshair Spread Factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairVelocityFactor(0.f),
	CrosshairInAirFactor(0.f),
	CrosshairAimingFactor(0.f),
	CrosshairFiringFactor(0.f)
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
	
}




// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetCameraFOV(DeltaTime);
	SetLookRates();
	CrosshairSpread(DeltaTime);

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
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
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

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimingFactor;
}

void AShooterCharacter::FireWeapon()
{
		
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	const USkeletalMeshSocket* BarrelSocket_1 = GetMesh()->GetSocketByName(TEXT("Muzzle_01"));
	const USkeletalMeshSocket* BarrelSocket_2 = GetMesh()->GetSocketByName(TEXT("Muzzle_02"));

	if (BarrelSocket_1)
	{
		FTransform BarrelSocketTransform_1 = BarrelSocket_1->GetSocketTransform(GetMesh());
		FTransform BarrelSocketTransform_2 = BarrelSocket_2->GetSocketTransform(GetMesh());
		if (MuzzleFlash_Right && MuzzleFlash_Left)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash_Right, BarrelSocketTransform_1);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash_Left, BarrelSocketTransform_2);
		}

		FVector BeamEnd_1;
		FVector BeamEnd_2;
		bool bBeamEndLocation_1 = GetBeamEndLocation(BarrelSocketTransform_1.GetLocation(), BeamEnd_1);
		bool bBeamEndLocation_2 = GetBeamEndLocation(BarrelSocketTransform_2.GetLocation(), BeamEnd_2);

		if (bBeamEndLocation_1 && bBeamEndLocation_2)
		{
			if (BulletImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactParticles, BeamEnd_1);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactParticles, BeamEnd_2);
			}
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(TEXT("StartFire"));
	}

}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	// Get Viewport Size
	FVector2D Viewport;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(Viewport);
	}

	// Get Crosshair Location in Screen Space
	FVector2D CrosshairScreenLocation{ Viewport.X / 2, (Viewport.Y / 2) };

	// Get Crosshair Location in World Space
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairScreenLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	// If projection is successful then do a line trace from the crosshair world location
	if (bScreenToWorld)
	{
		FHitResult CrosshairHitResult;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * 50'000 };
		OutBeamLocation = End;

		GetWorld()->LineTraceSingleByChannel(CrosshairHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (CrosshairHitResult.bBlockingHit)
		{
			OutBeamLocation = CrosshairHitResult.Location;
		}

		// Do second line trace from weapon barrel so determine if anything was hit in between
		FHitResult WeaponHitResult;
		const FVector WeaponStart{ MuzzleSocketLocation };
		const FVector WeaponEnd{ OutBeamLocation };	// End of Line Trace (which is the end of the previous line trace)

		GetWorld()->LineTraceSingleByChannel(WeaponHitResult, WeaponStart, WeaponEnd, ECollisionChannel::ECC_Visibility);
		if (WeaponHitResult.bBlockingHit)
		{
			OutBeamLocation = WeaponHitResult.Location;
		}

		return true;
	}

	return false;
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



