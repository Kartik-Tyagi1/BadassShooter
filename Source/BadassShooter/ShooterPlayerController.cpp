// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{
	
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// If the BP_ShooterHUDOverlay blueprint class is set
	if (HUDOverlayClass)
	{
		// Create the widget based on the widget in the BP_ShooterHUDOverlay (this is the widget with the canvas panel that contains the ammo counter)
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);

		// If creation of the widget is successful then add it to the viewport and show it
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}