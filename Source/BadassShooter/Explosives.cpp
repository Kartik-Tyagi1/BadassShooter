// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosives.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AExplosives::AExplosives():
	ExplosiveDamage(100.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplosiveMesh"));
	SetRootComponent(ExplosiveMesh);

	ExplosiveOverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosiveOverlapSphere"));
	ExplosiveOverlapSphere->SetupAttachment(ExplosiveMesh);
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

void AExplosives::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController)
{
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, HitResult.Location, FRotator(0.f), true);
	}

	//Apply Explosive Damage
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (auto Actor : OverlappingActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("The actor effected by the explosion is: %s"), *Actor->GetName());
		UGameplayStatics::ApplyDamage(Actor, ExplosiveDamage, ShooterController, Shooter, UDamageType::StaticClass());
	}

	Destroy();
}

