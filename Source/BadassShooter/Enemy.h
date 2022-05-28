// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class BADASSSHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Override of Interface to determine what happens when enemy is hit by bullets */
	virtual void BulletHit_Implementation(FHitResult HitResult) override;

	/* Callback function to reset bCanHitReact for HitReactTimer*/
	void ResetHitReactTimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/* ShowHealthBar Shows The health bar but this one is implemented in blueprint */
	/* ShowHealthBar_Implementation Shows The health bar but this one is implemented in C++ and is called in blueprint */
	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	/* HideHealthBar Hides the health bar and is implemented in blueprint */
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	/* Enemy Death Function */
	void Die();

	/* Function to play hit react
	   TODO: Calculate Direction of hit and play correct Section
	*/
	void PlayHitMontage(FName SectionName, float PlayRate = 1.f);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	/* Callback for the RemoveHitNumberTimer in the StoreHitNumber Function */
	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	/* Function to keep hit numbers at the location of shot */
	void UpdateHitNumbers();

	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION()
	void AttackRangeSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AttackRangeSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Function to play attack anims for enemy */
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName SectionName, float PlayRate = 1.f);

	/* Tunction that returns a random integer between 1-4 to determine which attack montage section to play */
	UFUNCTION(BlueprintPure) // This one does not have an execution pin, it's just a function node that returns a value 
	FName GetAttackSectionName();

	UFUNCTION()
	void LeftWeaponCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RightWeaponCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Function to activate and deactivate weapon collision based on attack animation frame */
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void ActivateRightWeaponCollision();
	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeaponCollision();

	void DoDamage(AActor* Victim);

private:
	/* Particles to spawn when enemy is hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* HitParticles;

	/* Sound to play when enemy is hit by bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* HitSound;

	/* Current Health of the Enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	/* Max health of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	/* Name of the Head Bone of the Enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	/* Timer for showing the health bar when the enemy is hit */
	FTimerHandle HealthBarTimer;

	/* Amount of time the health bar should be displayed after the enemy is not hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	/* Montage used to play hit and death animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	/* Timer to delay when the hit react anim is played so the enemy doesn't look like its spazzing out when being shot at */
	FTimerHandle HitReactTimer;

	bool bCanHitReact;

	/* Floats to randomize the delay of the hit react animation*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactDelayMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactDelayMax;

	/* TMap to store hit number and its location */
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	/* Time after which hit number is removed from TMAP and display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestoryTime;

	/* Behavior Tree for AI Character */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	/* Location where enemy will move to and patrol */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	/* Location where enemy will move to and patrol */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	/* Reference to enemy controller class */
	class AEnemyController* EnemyController;

	/* Sphere component to determine when the enemy should become hostile (aka agro) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	/* Boolean for if the enemy is stunned after getting hit */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsStunned;

	/* Chance of stunning the enemy on a hit; 0: No stun , 1 : 100% Stun chance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChance;

	/* Boolean to determine if player is in attack range of the enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsInAttackRange;

	/* Sphere componenet to dertermine when the enemy should start attacking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* AttackRangeSphere;

	/* Montage used to play attacking animations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	/* Montage Section Names for the Attack Montage*/
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;

	/* Collision Boxes for Left Weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* LeftWeaponCollision;

	/* Collision Boxes for Right Weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	/* Base damage the enemy can inflict */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float BaseDamage;


public:	
	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumbers(float Damage, FVector HitLocation, bool bIsHeadShot);

};
