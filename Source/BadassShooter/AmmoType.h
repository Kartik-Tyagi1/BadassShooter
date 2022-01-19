#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_Pistol	UMETA(DisplayName = "Pistol"),
	EAT_AR		UMETA(DisplayName = "AssaultRifle"),

	EAT_MAX		UMETA(DisplayName = "DefaultMAX")
};