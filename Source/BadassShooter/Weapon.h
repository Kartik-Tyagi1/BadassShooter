// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class BADASSSHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Tick(float DeltaTime);

protected:
	void StopFalling();

private:
	FTimerHandle ThrowWeaponTimer;

	/* Time for throw weapon timer handle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Falling, meta = (AllowPrivateAccess = "true"))
	float ThrowWeaponTime;

	/* Boolean to determine if weapon is falling */
	bool bIsFalling;

	/* Impulse amount for weapon when being thrown */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Falling, meta = (AllowPrivateAccess = "true"))
	float WeaponImpulseAmount;

	/* This the amount of ammo in the gun magazine itself (NOT CARRIED AMMO) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 AmmoInMagazine;

public:
	void ThrowWeapon();

	/* Function to return ammo in magazine (called in shooter character class) */
	FORCEINLINE int32 GetAmmoInMagazine() const { return AmmoInMagazine; }

	/* Decrement ammo in magazine after firing (called in shooter character class) */
	void DecrementAmmo();

	
};
