// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Default")),
	ItemType(FString("Submachine Gun")),
	ItemRarity(EItemRarity::EIR_Cool),
	ItemRarityText(FString("Cool")),
	ItemAmount(0),
	ItemState(EItemState::EIS_Pickup),
	// Item Interping Variables
	ItemInterpStartLocation(FVector(0.f)),
	ItemInterpTargetLocation(FVector(0.f)),
	ItemZCurveInterpTime(0.7f),
	bIsInterping(false),
	// Item Type
	ItemPickupType(EItemType::EIT_MAX)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// Hide PickupWidget at the beginning of the game
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
	// Setup Begin and End Overlap events for the AreaSphere
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereBeginOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Determine ItemRarityText and StarCount based on enum type of Item Rarity
	SetItemRarityAndStars();

	// Set Item properties based on itemstate variable
	SetItemProperties(ItemState);

	// Turn of custom depth to start 
	InitializeCustomDepth();
}


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if bIsInterping and start interpolation
	InterpolateItemLocation(DeltaTime);

}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		// This is not ShooterCharacterRef Member. This is a local creation
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		// This is not ShooterCharacterRef Member. This is a local creation
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}

		
	}
}

void AItem::SetItemRarityAndStars()
{
	for (int32 i = 0; i < 5; i++)
	{
		ActiveStars.Add(false);
	}

	// Set from right to left so the stars are aligned to right of widget
	switch (ItemRarity)
	{
	case EItemRarity::EIR_Lame:
		ItemRarityText = FString("Lame");
		ActiveStars[4] = true;
		break;
	case EItemRarity::EIR_Okay:
		ItemRarityText = FString("Okay");
		ActiveStars[4] = true;
		ActiveStars[3] = true;
		break;
	case EItemRarity::EIR_Cool:
		ItemRarityText = FString("Cool");
		ActiveStars[4] = true;
		ActiveStars[3] = true;
		ActiveStars[2] = true;
		break;
	case EItemRarity::EIR_Crazy:
		ItemRarityText = FString("Crazy");
		ActiveStars[4] = true;
		ActiveStars[3] = true;
		ActiveStars[2] = true;
		ActiveStars[1] = true;
		break;
	case EItemRarity::EIR_Badass:
		ItemRarityText = FString("Badass");
		ActiveStars[4] = true;
		ActiveStars[3] = true;
		ActiveStars[2] = true;
		ActiveStars[1] = true;
		ActiveStars[0] = true;
		break;
	}

}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		// Set ItemMesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EItemState::EIS_Equipped:
		// Set Pickup Widget
		PickupWidget->SetVisibility(false);
		// Set ItemMesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		// Set ItemMesh Properties
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// Set AreaSphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		// Set Pickup Widget
		PickupWidget->SetVisibility(false);
		// Set ItemMesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set CollisionBox properties
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}

}


void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurveInterpTimer(AShooterCharacter* Character)
{
	// This will be called from shooter character so we have to supply the data to the reference
	ShooterCharacterRef = Character;

	InterpLocationIndex = ShooterCharacterRef->GetInterpLocationsLowestItemIndex();
	ShooterCharacterRef->IncrementInterpLocationsItemCount(InterpLocationIndex, 1);

	// Play the pickup sound here instead of in ShooterCharacter.cpp so that the auto ammo pickup plays the sound as well
	PlayPickupSound();

	// Set Item Start location
	ItemInterpStartLocation = GetActorLocation();

	// Set IsInterping and ItemState
	bIsInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	// Start the Timer
	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::EndItemInterpTimer, ItemZCurveInterpTime);
}

void AItem::PlayPickupSound()
{
	if (ShooterCharacterRef == nullptr) return;

	if (ShooterCharacterRef->ShouldPlayPickupSound())
	{
		if (PickupSound)
		{
			ShooterCharacterRef->StartPickupSoundTimer();
			UGameplayStatics::PlaySound2D(this, PickupSound);
		}
	}
}

void AItem::PlayEquipSound()
{
	if (ShooterCharacterRef == nullptr) return;

	if (ShooterCharacterRef->ShouldPlayEquipSound())
	{
		if (EquipSound)
		{
			ShooterCharacterRef->StartEquipSoundTimer();
			UGameplayStatics::PlaySound2D(this, EquipSound);
		}
	}
}


void AItem::EndItemInterpTimer()
{
	ShooterCharacterRef->IncrementInterpLocationsItemCount(InterpLocationIndex, -1);

	bIsInterping = false;
	if (ShooterCharacterRef)
	{
		ShooterCharacterRef->GetPickupItem(this);
	}
	// Set Item back to normal scale
	SetActorScale3D(FVector(1.f));
}

void AItem::InterpolateItemLocation(float DeltaTime)
{
	if (!bIsInterping) return;

	if (ShooterCharacterRef && ItemZCurve)
	{
		// Get amount of time elapsed after the item interp timer has started
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);

		// Get the value of the item z curve at the elapsed time
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		// All Z calculations are based on the Z of the start location
		FVector ItemLocation = ItemInterpStartLocation;

		// This is where the end locaiton of the camera is (a little in front and above the camera)
		//const FVector CameraLocation = ShooterCharacterRef->GetCameraInterpEndLocation();
		const FVector CameraLocation = GetInterpLocation();

		// Calculate the Delta Between the Item Location and the Camera Location so the item will rise
		const FVector ItemToCameraVector{ 0.f, 0.f, (CameraLocation - ItemLocation).Z };
		const float DeltaZ = ItemToCameraVector.Size();

		// Linear Interpolation of the X and Y value
		const FVector ItemCurrentLocation = GetActorLocation();
		const float ItemXValue = FMath::FInterpTo(ItemCurrentLocation.X, CameraLocation.X, DeltaTime, 30.f);
		const float ItemYValue = FMath::FInterpTo(ItemCurrentLocation.Y, CameraLocation.Y, DeltaTime, 30.f);

		ItemLocation.X = ItemXValue;
		ItemLocation.Y = ItemYValue;

		// Add the delta to the item location and set the actor location
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			// Shrink Item as the interpolation happens
			const float CurveScaleValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(CurveScaleValue, CurveScaleValue, CurveScaleValue));
		}

	}

}

FVector AItem::GetInterpLocation()
{
	if (ShooterCharacterRef == nullptr) return FVector(0.f);

	switch (ItemPickupType)
	{
	// Item locations are indicies 1-7
	case EItemType::EIT_Ammo:
		return ShooterCharacterRef->GetInterpLocation(InterpLocationIndex).SceneComponent->GetComponentLocation();
		break;
	// Weapon locations is index 0
	case EItemType::EIT_Weapon:
		return ShooterCharacterRef->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	}
	return FVector();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	if (MaterialInstance)
	{
		// Construct dynamic material instance based on material instance
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);

		// Set the dynamic material instance to the mesh 
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
	}
}


void AItem::EnableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(true);
}

void AItem::DisableCustomDepth()
{
	ItemMesh->SetRenderCustomDepth(false);
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}


