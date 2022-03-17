// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BADASSSHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AShooterPlayerController();

protected:
	virtual void BeginPlay() override;

private:

	/*---------------------------------------------------- AMMO COUNTER WIDGET ------------------------------------------------*/

	/* Holds a reference of the ShooterHUDOverlay widget blueprint (BP_ShooterHUDOverlay -> Which holds the ammo counter) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDOverlayClass;

	/* Variable to hold the HUD overlay widget after creating it (this only holds the ammo counter) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;

	/*---------------------------------------------------- INVENTORY WIDGET ------------------------------------------------*/

	// TODO:: Create a new HUD blueprint class (and two more of the above) for the inventory so we can control it with a button mapping

	/* Holds a reference of the ShooterInventoryHUDOverlay widget blueprint (BP_ShooterHUDOverlay -> Which holds the Inventory) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> InventoryHUDOverlayClass;

	/* Variable to hold the InventoryHUDoverlay widget after creating it (this only holds the Inventory) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widgets, meta = (AllowPrivateAccess = "true"))
	UUserWidget* InventoryHUDOverlay;
};

