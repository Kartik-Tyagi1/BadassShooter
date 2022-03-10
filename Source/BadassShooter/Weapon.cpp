// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Kismet/GameplayStatics.h"


AWeapon::AWeapon() :
	ThrowWeaponTime(1.3f),
	bIsFalling(false),
	WeaponImpulseAmount(2'000.f),
	AmmoInMagazine(30),
	MaximumMagazineCapacity(30),
	WeaponType(EWeaponType::EWT_AssaultRifle),
	AmmoType(EAmmoType::EAT_AR),
	ReloadMontageSectionName(FName(TEXT("Reload_AssaultRifle"))),
	WeaponMagBoneName(FName(TEXT("Clip_Bone"))),
	bIsMagMoving(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && bIsFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::ThrowWeapon()
{
	
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	// Direction in which we throw the Weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	float RandomRotation{ 30.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= WeaponImpulseAmount;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bIsFalling = true;

	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

	EnableGlowMaterial();
}

void AWeapon::StopFalling()
{
	bIsFalling = false;
	SetItemState(EItemState::EIS_Pickup);
	StartPulseTimer();
}

void AWeapon::DecrementAmmo()
{
	if (AmmoInMagazine - 1 <= 0)
	{
		AmmoInMagazine = 0;
	}
	else
	{
		--AmmoInMagazine;
	}
}

void AWeapon::UpdateAmmo(int32 Amount)
{
	if (AmmoInMagazine + Amount >= MaximumMagazineCapacity)
	{
		AmmoInMagazine = MaximumMagazineCapacity;
	}
	else
	{
		AmmoInMagazine += Amount;
	}

}

bool AWeapon::ClipIsFull()
{
	return AmmoInMagazine >= MaximumMagazineCapacity;
}

