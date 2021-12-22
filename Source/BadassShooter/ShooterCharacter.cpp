// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter() :
	LookAroundRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a spring arm for the camera (pulls in toward the character when collisions occur with the character)
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraSpringArm->TargetArmLength = 300.f;
	CameraSpringArm->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Create Camera attached to spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Camera does NOT rotate relative to the spring arm

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

