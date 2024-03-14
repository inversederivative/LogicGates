// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/PowerSource.h"
#include "LogicGates/LogicGatesCharacter.h"

APowerSource::APowerSource()
{
	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));

	RootComponent = DisplayMesh;
	// Set initial shape to Square. Can change mesh later...
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube'"));	
	if (CubeMeshAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(CubeMeshAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find cube mesh asset."));
	}

	// Load the materials from asset paths
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DisabledMaterialAsset(TEXT("/Game/LogicGates/Materials/M_DisabledMaterial"));
	if (DisabledMaterialAsset.Succeeded())
	{
		DisabledMaterial = DisabledMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Disabled material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OffMaterialAsset(TEXT("/Game/LogicGates/Materials/M_OffMaterial"));
	if (OffMaterialAsset.Succeeded())
	{
		OffMaterial = OffMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Off material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OnMaterialAsset(TEXT("/Game/LogicGates/Materials/M_OnMaterial"));
	if (OnMaterialAsset.Succeeded())
	{
		OnMaterial = OnMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find On material asset."));
	}

	// Set default material for the mesh
	if (DisabledMaterial)
	{
		DisplayMesh->SetMaterial(0, DisabledMaterial);
	}
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(100.0f); // Set the radius of the collision sphere
	CollisionSphere->SetupAttachment(RootComponent);
	
	state_ = DISABLED;
	InteractionDistance = 100.0f; // Set the interaction distance
}

// Sets default values
// Do I even use this constructor anywhere?
APowerSource::APowerSource(eLogicState state) {

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	RootComponent = DisplayMesh;

	// Set initial color to Black, since initial state is Disabled!
	// Load the materials from asset paths
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DisabledMaterialAsset(TEXT("/Game/LogicGates/Materials/M_DisabledMaterial"));
	if (DisabledMaterialAsset.Succeeded())
	{
		DisabledMaterial = DisabledMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Disabled material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OffMaterialAsset(TEXT("/Game/LogicGates/Materials/M_OffMaterial"));
	if (OffMaterialAsset.Succeeded())
	{
		OffMaterial = OffMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Off material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OnMaterialAsset(TEXT("/Game/LogicGates/Materials/M_OnMaterial"));
	if (OnMaterialAsset.Succeeded())
	{
		OnMaterial = OnMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find On material asset."));
	}
	// Don't need to set Display Mesh color here, because we do it in the ChangeState function
	ChangeState(state);
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(100.0f); // Set the radius of the collision sphere
	CollisionSphere->SetupAttachment(RootComponent);
	
	state_ = DISABLED;
	
	// I don't think I need this anymore...
	InteractionDistance = 100.0f; // Set the interaction distance
}


void APowerSource::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APowerSource::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &APowerSource::OnOverlapEnd);
}

void APowerSource::ChangeState(eLogicState state) {
	state_ = state;

	// Handle Color changes
	if (state == DISABLED)
	{
		UE_LOG(LogTemp, Display, TEXT("****The state was changed. The current state is: Disabled"));
		DisplayMesh->SetMaterial(0, DisabledMaterial);
	}
	if (state == OFF)
	{
		
		UE_LOG(LogTemp, Display, TEXT("****The state was changed. The current state is: Off"));
		DisplayMesh->SetMaterial(0, OffMaterial);
	}
	if (state == ON)
	{
		UE_LOG(LogTemp, Display, TEXT("****The state was changed. The current state is: On"));
		DisplayMesh->SetMaterial(0, OnMaterial);
	}
	Notify();
}

// Probably don't need this anymore either...
void APowerSource::ToggleState() {
	// We need to then check if disabled
	if (state_ != DISABLED)
	{
		if (state_ == OFF)
		{
			state_ = ON;
		}
		else
		{
			state_ = OFF;
		}
	}
	Notify();
}

eLogicState APowerSource::GetState() const {
	return state_;
}

void APowerSource::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is the player character
	if (OtherActor)
	{
		ALogicGatesCharacter* myCharacter = Cast<ALogicGatesCharacter>(OtherActor);
		if (myCharacter)
		{
			myCharacter->SetCurrentNode(this);
		}
	}
}

void APowerSource::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ALogicGatesCharacter* myCharacter = Cast<ALogicGatesCharacter>(OtherActor);
		if (myCharacter)
		{
			myCharacter->SetCurrentNode(nullptr);
		}
	}
}