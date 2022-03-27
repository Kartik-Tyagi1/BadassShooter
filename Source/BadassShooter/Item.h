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

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo	UMETA(DisplayName = "Ammo"),
	EIT_Weapon	UMETA(DisplayName = "Weapon"),

	EIT_MAX		UMETA(DisplayName = "DefaultMAX")
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
	virtual void SetItemProperties(EItemState State);

	/* Call back function to finish interping after the interping timer is done */
	void EndItemInterpTimer();

	/* Function to handle the item interpolation when picking up and item */
	void InterpolateItemLocation(float DeltaTime);

	/* Function to determine item interp location from shooter character InterpLocations array */
	FVector GetInterpLocation();

	/* C++ verison of construction strip in blueprint */
	virtual void OnConstruction(const FTransform& Transform) override;

	/* Pulse Effect Functions */
	void StartPulseTimer();
	void ResetPulseTimer();
	void UpdatePulseParameters();


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

	/*------------------------------------------- END WIDGET SECTIONS -----------------------------------------------------*/

	/* Pickup widget for the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickupWidget;

	/* Name of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemName;

	/* Type of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemType;

	/* Rarity of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	/* Rarity Text of the Item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FString ItemRarityText;

	/* Amount of the Item (i.e amount of ammo) -- THIS IS ALSO USED TO TRACK AMMO AMOUNT AND STUFF NOT ONLY FOR WIDGET */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 ItemAmount;

	/* Array that holds booleans for amount of stars shown in widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/*------------------------------------------- END WIDGET SECTIONS -----------------------------------------------------*/

	/* State of the item (on ground, equppied etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState;

	/* Item Curve for the Z value of the interpolation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;

	/* Item Curve for the scale of the item of the interpolation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	/* Start Location of the item before interping */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/* Location we want to item to interp to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpTargetLocation;

	/* Length of time the timer will run (this works in the same time as the item z curve. If one changes so must the other. OR ELSE!!!! */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float ItemZCurveInterpTime;

	/* Boolean to determine if the item intepring is occuring */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	bool bIsInterping;

	/* Timer handle that start the interpolation and calls EndItemInterpTimer function callback */
	FTimerHandle ItemInterpTimer;

	/* Reference to shooter character class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class AShooterCharacter* ShooterCharacterRef;
	
	/* Sound for picking up an item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class USoundCue* PickupSound;
	
	/* Sound for equipping an item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USoundCue* EquipSound;

	/*------------------------------------------- Item Interpolation -----------------------------------------------------*/

	/* Item type that is being picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	EItemType ItemPickupType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 InterpLocationIndex;

	/*---------------------------------------------- Item Materials -----------------------------------------------------*/

	/* Material index on item that we want to set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	int32 MaterialIndex;

	/* Dynamic material instance that can be set/changed during runtime (thats why its dynamic) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	/* The material instance the dynamic material instance refers to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstance;

	/* Curve that hold that values of the dynamic material instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	class UCurveVector* PulseCurve;

	/* Curve that hold that values of the dynamic material instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UCurveVector* InterpPulseCurve;

	/* Timer Handle that controls the pulse effect */
	FTimerHandle PulseTimer;

	float PulseDuration;

	UPROPERTY(EditAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float GlowBlendAlpha;

	UPROPERTY(EditAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float FresnelExponent;

	UPROPERTY(EditAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	float FrenselReflectionFraction;


	/*----------------------------------- Custom Depth (related to public section) --------------------------------------------*/

	bool bCanChangeCustomDepth;

	/*--------------------------------------------------- Inventory --------------------------------------------------------*/

	/* Background image for items in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* BackgroundImage;

	/* Image of the item for items in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* ItemImage;

	/* Image of the Ammo Icon for items in the inventory */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoImage;

	/* The index in the inventory array (shootercharacter.h) where this weapon is stored */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 SlotIndex;

public:	
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	void StartItemCurveInterpTimer(AShooterCharacter* Character, bool bForcePlaySound = false);

	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	FORCEINLINE int32 GetItemAmount() const { return ItemAmount; }

	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }

	FORCEINLINE void SetCharacter(AShooterCharacter* Char) { ShooterCharacterRef = Char; }

	void PlayPickupSound(bool bForcePlaySound = false);
	void PlayEquipSound(bool bForcePlaySound = false);

	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	void InitializeCustomDepth();

	/* Enable and disable glow material */
	void EnableGlowMaterial();
	void DisableGlowMaterial();

	
};
