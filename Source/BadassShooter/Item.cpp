// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "ShooterCharacter.h"

// Sets default values
AItem::AItem():
	ItemName(FString("Default")),
	ItemType(FString("Submachine Gun")),
	ItemRarity(EItemRarity::EIR_Cool),
	ItemRarityText(FString("Cool")),
	ItemAmount(0),
	ItemState(EItemState::EIS_Pickup)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

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
}


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			PickupWidget->SetVisibility(true);
		}

		/*if (SweepResult.GetActor() != nullptr)
		{
			OverlappedActor = SweepResult.GetActor();
		}*/

	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		if (ShooterCharacter)
		{
			PickupWidget->SetVisibility(false);
		}

		// OverlappedActor = nullptr;
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
		break;
	case EItemState::EIS_Equipped:
		// Set ItemMesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Set AreaSphere Properties
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		break;
	}

}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

