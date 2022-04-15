#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AR15			UMETA(DisplayName = "AR-15"),
	EWT_AssaultRifle	UMETA(DisplayName = "AssaultRifle"),
	EWT_Pistol			UMETA(DisplayName = "Pistol"),

	EWT_MAX				UMETA(DisplayName = "DefaultMAX")
};