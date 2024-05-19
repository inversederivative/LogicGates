// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/LogicGates/TriStateBufferGate.h"

#include "LogicGates/LogicGatesCharacter.h"

ATriStateBufferGate::ATriStateBufferGate()
{
	SetNodeName("TriStateBufferGate");
	
	SetHasOutputNode(true);
	
	TriStateBufferGateRoot = CreateDefaultSubobject<USceneComponent>("TriState SceneComponent");
	RootComponent = TriStateBufferGateRoot;
	
	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(RootComponent);

	InputPortData = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortData"));
	InputPortData->SetupAttachment(RootComponent);

	InputPortEnable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortEnable"));
	InputPortEnable->SetupAttachment(RootComponent);
	
	OutputPortX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutputPortX"));
	OutputPortX->SetupAttachment(RootComponent);

	CableConnector = CreateDefaultSubobject<USceneComponent>("CableConnector");
	CableConnector->SetupAttachment(OutputPortX);

	OutputCableX = CreateDefaultSubobject<UCableComponent>(TEXT("TriStateBufferGate"));
	OutputCableX->AttachToComponent(CableConnector, FAttachmentTransformRules::KeepWorldTransform);
	OutputCableX->SetAttachEndTo(this, "Cable Connector");

	SetCableConnectNumber(GetSerialNumber());
	SetCableConnectString("Cable Connector");
	SetCableConnectFrom("X");
	
	SetupMeshes();
	SetupMaterials();

	// Set default material for the mesh
	if (DisabledMaterial)
	{
		InputPortData->SetMaterial(0, DisabledMaterial);
		InputPortEnable->SetMaterial(0, DisabledMaterial);
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

void ATriStateBufferGate::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATriStateBufferGate::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ATriStateBufferGate::OnOverlapEnd);

}

void ATriStateBufferGate::SetupMeshes()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	AndGateAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_TriStateBufferGate'"));
	if (AndGateAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(AndGateAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned mesh asset to TriStateBufferGate!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find AndGate Asset."));
	}
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ConnectionAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_Connection'"));
	if (ConnectionAsset.Succeeded())
	{
		InputPortData->SetStaticMesh(ConnectionAsset.Object);
		InputPortEnable->SetStaticMesh(ConnectionAsset.Object);
		OutputPortX->SetStaticMesh(ConnectionAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}
}

void ATriStateBufferGate::SetupMaterials()
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

void ATriStateBufferGate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

void ATriStateBufferGate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

void ATriStateBufferGate::SetDataInput(AAbstractNode* input)
{
	dataInput = input;
	input->Attach(this);
	//connectedNodes_.push_back(input);
	connectedNodes_.push_back(input);

	dataInput->GetOutputCableX()->SetAttachEndTo(this, "InputPortData");
	eLogicState currentState = input->GetState();

	if (currentState == DISABLED)
	{
		InputPortData->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		InputPortData->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		InputPortData->SetMaterial(0, OnMaterial);
	}

	Update(GetState());
}

void ATriStateBufferGate::SetEnableInput(AAbstractNode* input)
{
	enableInput = input;
	input->Attach(this);
	//connectedNodes_.push_back(input);
	connectedNodes_.push_back(input);

	enableInput->GetOutputCableX()->SetAttachEndTo(this, "InputPortEnable");
	eLogicState currentState = input->GetState();

	if (currentState == DISABLED)
	{
		InputPortEnable->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		InputPortEnable->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		InputPortEnable->SetMaterial(0, OnMaterial);
	}
	
	Update(GetState());
}

void ATriStateBufferGate::Update(eLogicState state)
{
	outputState_ = state; // TODO: Remove this... Rethink the way this function should work from base class... Not many classes can take the state and directly set it to outputState_

	if (dataInput)
	{
		switch (dataInput->GetState())
		{
		case DISABLED:
			InputPortData->SetMaterial(0, DisabledMaterial);
			break;
		case OFF:
			InputPortData->SetMaterial(0, OffMaterial);
			break;
		case ON:
			InputPortData->SetMaterial(0, OnMaterial);
			break;
		}
	}
	if (enableInput)
	{
		switch (enableInput->GetState())
		{
		case DISABLED:
			InputPortEnable->SetMaterial(0, DisabledMaterial);
			break;
		case OFF:
			InputPortEnable->SetMaterial(0, OffMaterial);
			break;
		case ON:
			InputPortEnable->SetMaterial(0, OnMaterial);
			break;
		}
	}

	eLogicState output = GetState();
	switch(output)
	{
	case DISABLED:
		OutputPortX->SetMaterial(0, DisabledMaterial);
		break;
	case OFF:
		OutputPortX->SetMaterial(0, OffMaterial);
		break;
	case ON:
		OutputPortX->SetMaterial(0, OnMaterial);
		break;
	}
	
	Notify();
}

eLogicState ATriStateBufferGate::GetState() const
{
	eLogicState output = DISABLED;
	if (dataInput && enableInput)
	{
		if (enableInput->GetState() == OFF)
		{
			// If the enable Input is OFF, we want to set the output of the
			//  buffer to DISABLED!
			output = DISABLED;
		}
		if (enableInput->GetState() == ON && dataInput->GetState() == OFF)
		{
			output = OFF;
		}
		if (enableInput->GetState() == ON && dataInput->GetState() == ON)
		{
			output = ON;
		}
	}
	else
	{
		std::cout<<"Both Inputs of Tri State Buffer Gate weren't populated yet..." << std::endl;
		output = DISABLED;
	}
	return output;
}
