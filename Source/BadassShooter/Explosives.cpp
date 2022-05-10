// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosives.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AExplosives::AExplosives()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExplosives::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosives::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosives::BulletHit_Implementation(FHitResult HitResult)
{
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, HitResult.Location, FRotator(0.f), true);
	}

	// TODO: Apply Explosive Damage

	Destroy();
}

