// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"


AAmmo::AAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());

}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

