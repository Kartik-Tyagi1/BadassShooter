// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AmmoType.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AR15			UMETA(DisplayName = "AR-15"),
	EWT_AssaultRifle	UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX				UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WeaponInventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WeaponAmmoInventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;
};

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

	virtual void OnConstruction(const FTransform& Transform) override;

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

	/* This the max amoutn of ammo in the gun magazine can hold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaximumMagazineCapacity;

	/* Type of the weapon (used for initalizing weapon properties like ammo type and shit) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	/* Type of ammo for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/* Name of the section for the reload animation for this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSectionName;

	/* Name of the bone of the magazine for the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	FName WeaponMagBoneName;

	/* Boolean to set if the weapon magazine is moving */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsMagMoving;

	/* Datatable that hold properties of weapons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	/* Used to clear the material index when switching weapon types */
	int32 PreviousMaterialIndex;

public:
	void ThrowWeapon();

	/* Function to return ammo in magazine (called in shooter character class) */
	FORCEINLINE int32 GetAmmoInMagazine() const { return AmmoInMagazine; }
	FORCEINLINE int32 GetMaximumMagazineCapacity() const { return MaximumMagazineCapacity; }

	/* Decrement ammo in magazine after firing (called in shooter character class) */
	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSectionName() const { return ReloadMontageSectionName; }
	FORCEINLINE FName GetWeaponMagBoneName() const { return WeaponMagBoneName; }

	void UpdateAmmo(int32 Amount);
	FORCEINLINE void SetIsMagMoving(bool Moving) { bIsMagMoving = Moving; }

	bool ClipIsFull();

	
};
