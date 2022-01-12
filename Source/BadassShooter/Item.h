// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Lame	UMETA(DisplayName = "Lame"),
	EIR_Okay	UMETA(DisplayName = "Okay"),
	EIR_Cool	UMETA(DisplayName = "Cool"),
	EIR_Crazy	UMETA(DisplayName = "Crazy"),
	EIR_Badass	UMETA(DisplayName = "Badass"),

	EIR_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup			UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping	UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp		UMETA(DisplayName = "PickedUp"),
	EIS_Equipped		UMETA(DisplayName = "Equipped"),
	EIS_Falling			UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class BADASSSHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	/* Set the Item Rarity Textand Number of Stars shown in widget */
	void SetItemRarityAndStars();

	/* Set properties of the item based on the state*/
	void SetItemProperties(EItemState State);

	void EndItemInterpTimer();

private:

	/* Skeletal Mesh of the Item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	/** Line trace collides with box to show HUD widgets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;
	
	/* Area Sphere used to determine if widget should be shown when player is overlapping */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	/* Pickup widget for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

	/* Name of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/* Name of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemType;

	/* Name of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/* Name of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemRarityText;

	/* Name of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemAmount;

	/* Array that holds booleans for amount of stars shown in widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/* State of the item (on ground, equppied etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/* Item Curve for the Z value of the interpolation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;

	/* Start Location of the item before interping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/* Location we want to item to interp to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpTargetLocation;

	/* Boolean to determine if the item intepring is occuring */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsInterping;

	/* Timer handle that start the interpolation and calls EndItemInterpTimer function callback */
	FTimerHandle ItemInterpTimer;

	/* Length of time the timer will run (this works in the same time as the item z curve. If one changes so must the other. OR ELSE!!!! */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ItemZCurveInterpTime;

	/* Reference to shooter character class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacter;


public:	
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	void StartItemInterpTimer(AShooterCharacter* Character);
	
};
