// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/LogicGates/AbstractTwoInputNode.h"
#include "LogicGates/LogicGatesCharacter.h"

AAbstractTwoInputNode::AAbstractTwoInputNode()
{
	// Initialize members here.
	outputState_ = DISABLED; // Set this in constructor.

	
	LogicGateSceneComponent = CreateDefaultSubobject<USceneComponent>("Uninitialized SceneComponent");
	RootComponent = LogicGateSceneComponent;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(RootComponent);

	

	InputPortX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortX"));
	InputPortX->SetupAttachment(RootComponent);

	InputPortY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortY"));
	InputPortY->SetupAttachment(RootComponent);

	OutputPortX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutputPortX"));
	OutputPortX->SetupAttachment(RootComponent);

	CableConnector = CreateDefaultSubobject<USceneComponent>("CableConnector"); 
	CableConnector->SetupAttachment(OutputPortX);

	OutputCableX = CreateDefaultSubobject<UCableComponent>("AndGateOutput");
	OutputCableX->AttachToComponent(CableConnector, FAttachmentTransformRules::KeepWorldTransform);
	//OutputCableX->SetAttachEndToComponent(OutputPortX);
	// Connect Cable To Self
	OutputCableX->SetAttachEndTo(this, "Cable Connector"); // Don't ask me why there is a space here... :'D

	SetupMeshes();
	SetupMaterials();
	
	
	// Set default material for the mesh
	if (DisabledMaterial)
	{
		InputPortX->SetMaterial(0, DisabledMaterial);
		InputPortY->SetMaterial(0, DisabledMaterial);
		OutputPortX->SetMaterial(0, DisabledMaterial);
	}
	if (StandMaterial)
	{
		DisplayMesh->SetMaterial(0, StandMaterial);
	}

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(100.0f); // Set the radius of the collision sphere
	CollisionSphere->SetupAttachment(RootComponent);
}

void AAbstractTwoInputNode::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAbstractTwoInputNode::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AAbstractTwoInputNode::OnOverlapEnd);
}

void AAbstractTwoInputNode::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								  const FHitResult& SweepResult)
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

void AAbstractTwoInputNode::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

void AAbstractTwoInputNode::SetupMeshes()
{
	// I've moved the Mesh handling to the individual constructors.


	// NOTE: Don't need screen for AND Gate
	// static ConstructorHelpers::FObjectFinder<UStaticMesh>
	// ScreenAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_Screen'"));	
	// if (ScreenAsset.Succeeded())
	// {
	// 	ScreenMesh->SetStaticMesh(ScreenAsset.Object);
	// 	UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	// }

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ConnectionAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_Connection'"));
	if (ConnectionAsset.Succeeded())
	{
		InputPortX->SetStaticMesh(ConnectionAsset.Object);
		InputPortY->SetStaticMesh(ConnectionAsset.Object);
		OutputPortX->SetStaticMesh(ConnectionAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}
}

void AAbstractTwoInputNode::SetupMaterials()
{
	// Load the materials from asset paths
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
	DisabledMaterialAsset(TEXT("/Game/LogicGates/Materials/M_DisabledMaterial"));
	if (DisabledMaterialAsset.Succeeded())
	{
		DisabledMaterial = DisabledMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Disabled material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
	OffMaterialAsset(TEXT("/Game/LogicGates/Materials/M_OffMaterial"));
	if (OffMaterialAsset.Succeeded())
	{
		OffMaterial = OffMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Off material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
	OnMaterialAsset(TEXT("/Game/LogicGates/Materials/M_OnMaterial"));
	if (OnMaterialAsset.Succeeded())
	{
		OnMaterial = OnMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find On material asset."));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
	StandMaterialAsset(TEXT("/Game/LogicGates/Materials/M_Metal_Gold"));
	if (StandMaterialAsset.Succeeded())
	{
		StandMaterial = StandMaterialAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find On material asset."));
	}
}


void AAbstractTwoInputNode::SetInputX(AAbstractNode *input) {
	inputX = input;
	input->Attach(this);
	connectedNodes_.push_back(input);

	eLogicState currentState = input->GetState();

	if (currentState == DISABLED)
	{
		InputPortX->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		InputPortX->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		InputPortX->SetMaterial(0, OnMaterial);
	}

	// Should call the appropriate Update Function from And Gate, Or Gate, etc... 
	AAbstractTwoInputNode::Update(GetState());
}

void AAbstractTwoInputNode::SetInputY(AAbstractNode *input) {
	inputY = input;
	input->Attach(this);
	connectedNodes_.push_back(input);

	eLogicState currentState = input->GetState();


	if (currentState == DISABLED)
	{
		InputPortY->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		InputPortY->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		InputPortY->SetMaterial(0, OnMaterial);
	}
	AAbstractTwoInputNode::Update(GetState());
}

void AAbstractTwoInputNode::Update(eLogicState state) {
	// Notify observers of the new output state
	outputState_ = state;
	
	Notify();
}