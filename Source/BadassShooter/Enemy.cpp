// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f),
	bCanHitReact(true),
	HitReactDelayMin(0.5f),
	HitReactDelayMax(0.65f),
	HitNumberDestoryTime(1.5f),
	bIsStunned(false),
	StunChance(0.5f),
	AttackLFast(TEXT("AttackLFast")),
	AttackRFast(TEXT("AttackRFast")),
	AttackL(TEXT("AttackL")),
	AttackR(TEXT("AttackR")),
	BaseDamage(10.f),
	RightWeaponTopSocket(TEXT("FX_Trail_R_01")),
	LeftWeaponTopSocket(TEXT("FX_Trail_L_01")),
	bCanAttack(true),
	AttackWaitDuration(1.f),
	bIsDying(false),
	DestroyBodyWaitTime(4.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Agro Sphere
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	// Create Attack Range Sphere
	AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
	AttackRangeSphere->SetupAttachment(GetRootComponent());

	// Create Left Weapon Collision Box
	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollisionBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));

	// Create Right Weapon Collision Box
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollisionBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Overlap Events for Agro Sphere
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	// Overlap Events for Attack Range Sphere
	AttackRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackRangeSphereBeginOverlap);
	AttackRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackRangeSphereEndOverlap);
	// Overlap Events for Left and Right Weapon Collision Boxes
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::LeftWeaponCollisionOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::RightWeaponCollisionOverlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	// The next two lines prevent the camera from being wierd when the shooter character is too close to the enemy
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Set Collision Respones for Left and Right Weapons 
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Weapon does not start off colliding with random objects in the world
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // Only Collide with world objects
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // Only overlap with Pawn types (aka the shooter character)
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); 
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Get AI Controller
	EnemyController = Cast<AEnemyController>(GetController());

	// Set Can attack to true in blackboard
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), false);
	}

	// The patrol point location is local to the enemy location. This line transforms that location from local location to world location
	FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);


	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumbers();

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, HitResult.Location, FRotator(0.f), true);
	}

	// Do not do rest of this function is the enemy is dying 
	if (bIsDying) return;

	ShowHealthBar();
	const float Stunned = FMath::RandRange(0.f, 1.f);
	// The lower the stun chance the harder it is to stun since the chance of getting a lower number from the range decreases
	if (Stunned < StunChance)
	{
		SetStunned(true);
		PlayHitMontage(FName("HitReactFront"));
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Agro Enemy when shot by shooter character
	if (DamageCauser)
	{
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), DamageCauser);
		}
	}

	if (Health - DamageAmount <= 0)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}
	
	return Health;
}

void AEnemy::ShowHealthBar_Implementation()
{
	// Reset the timer if the enemy is being shot 
	GetWorldTimerManager().ClearTimer(HealthBarTimer);

	// Only hide if the enemy is not being shot for more than 4 seconds
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

void AEnemy::Die()
{
	// Do not  do into this function unless the enemy is dying
	if (bIsDying) return;
	bIsDying = true; // Now that its true this function will not be called again

	HideHealthBar();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		// TODO: Add other death montages 
	}

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), true);
		EnemyController->StopMovement();
	}
}

void AEnemy::PlayHitMontage(FName SectionName, float PlayRate)
{
	if (bCanHitReact)
	{
		if (HitMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(HitMontage, PlayRate);
				AnimInstance->Montage_JumpToSection(SectionName, HitMontage);
			}
		}

		bCanHitReact = false;
		const float HitReactDelay{ FMath::RandRange(HitReactDelayMin, HitReactDelayMax) };
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactDelay);
	}
	
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	// Add the HitNumber widget when enemy is shot
	HitNumbers.Add(HitNumber, Location);

	// Remove the hit number after some time 
	FTimerHandle DestoryHitNumberTimer;
	FTimerDelegate DestoryHitNumberDelegate;
	DestoryHitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorldTimerManager().SetTimer(DestoryHitNumberTimer, DestoryHitNumberDelegate, HitNumberDestoryTime, false);
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };

		FVector2D ScreenLocation;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenLocation);
		HitNumber->SetPositionInViewport(ScreenLocation);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	auto Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bIsStunned = Stunned;

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}

void AEnemy::AttackRangeSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			bIsInAttackRange = true;
			if (EnemyController)
			{
				EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), bIsInAttackRange);
			}
		}
	}
}

void AEnemy::AttackRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			bIsInAttackRange = false;
			if (EnemyController)
			{
				EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), bIsInAttackRange);
			}
		}
	}
}

void AEnemy::PlayAttackMontage(FName SectionName, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}

	bCanAttack = false; // Make enemy wait for next attack
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemy::ResetCanAttack, AttackWaitDuration);

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 SectionNumber{ FMath::RandRange(1,4) };

	switch (SectionNumber)
	{
	case 1:
		SectionName = AttackLFast;
		break;
	case 2:
		SectionName = AttackRFast;
		break;
	case 3:
		SectionName = AttackL;
		break;
	case 4:
		SectionName = AttackR;
		break;
	}

	return SectionName;
}

void AEnemy::LeftWeaponCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (OtherActor == nullptr) return;

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnVictimBlood(Character, LeftWeaponTopSocket);
		StunVictim(Character);
	}
}

void AEnemy::RightWeaponCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnVictimBlood(Character, RightWeaponTopSocket);
		StunVictim(Character);
	}
}

void AEnemy::ActivateLeftWeaponCollision()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeaponCollision()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeaponCollision()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeaponCollision()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DoDamage(AShooterCharacter* Victim)
{
	if (Victim == nullptr) return;
	
	UGameplayStatics::ApplyDamage(Victim, BaseDamage, EnemyController, this, UDamageType::StaticClass());
	if (Victim->GetMeleeImpactSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Victim->GetMeleeImpactSound(), Victim->GetActorLocation());
	}

}

void AEnemy::SpawnVictimBlood(AShooterCharacter* Victim, FName SocketName)
{
	if (Victim == nullptr) return;

	const USkeletalMeshSocket* WeaponTopSocket{ GetMesh()->GetSocketByName(SocketName) };
	if (WeaponTopSocket)
	{
		const FTransform WeaponTopSocketTransform{ WeaponTopSocket->GetSocketTransform(GetMesh()) };
		if (Victim->GetBloodParticles())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Victim->GetBloodParticles(), WeaponTopSocketTransform);
		}

	}
	
}

void AEnemy::StunVictim(AShooterCharacter* Victim)
{
	if (Victim == nullptr) return;

	const float StunnedAttempt{ FMath::RandRange(0.f, 1.f) };
	if (StunnedAttempt <= Victim->GetStunChance())
	{
		Victim->Stun();
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
	}
}

void AEnemy::FinishEnemyDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DestroyBodyTimer, this, &AEnemy::DestroyEnemy, DestroyBodyWaitTime);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;

}

