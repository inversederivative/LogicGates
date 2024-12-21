// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/Adders/FullAdder.h"
#include "LogicGates/LogicGatesCharacter.h"

AFullAdder::AFullAdder()
{
	SetNodeName("FullAdder");
	
	SetHasOutputNode(true);
	SetHasOutputY(true);
	outputSumState_ = DISABLED;
	outputCarryState_ = DISABLED;
	inputCarryState_ = DISABLED;

	xorGateA = CreateDefaultSubobject<AXorGate>("XorGateA");
	xorGateB = CreateDefaultSubobject<AXorGate>("XorGateB");
	andGateA = CreateDefaultSubobject<AAndGate>("AndGateA");
	andGateB = CreateDefaultSubobject<AAndGate>("AndGateB");
	orGate = CreateDefaultSubobject<AOrGate>("OrGate");

	carryOutNode = CreateDefaultSubobject<ABufferGate>("CarryOutNode");

	xorGateA->SetIsNodeForOtherNodes(true);
	xorGateB->SetIsNodeForOtherNodes(true);
	andGateA->SetIsNodeForOtherNodes(true);
	andGateB->SetIsNodeForOtherNodes(true);
	orGate->SetIsNodeForOtherNodes(true);

	xorGateA->SetNodeName("Internal");
	xorGateB->SetNodeName("Internal");
	andGateA->SetNodeName("Internal");
	andGateB->SetNodeName("Internal");
	orGate->SetNodeName("Internal");
	
	
	AdderSceneComponent = CreateDefaultSubobject<USceneComponent>("Adder SceneComponent");
	RootComponent = AdderSceneComponent;

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(RootComponent);

	InputPortX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortX"));
	InputPortX->SetupAttachment(RootComponent);
	InputPortY = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortY"));
	InputPortY->SetupAttachment(RootComponent);
	InputPortZ = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPortCarry"));
	InputPortZ->SetupAttachment(RootComponent);

	OutputPortX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutputPortX"));
	OutputPortX->SetupAttachment(RootComponent);
	// TODO: Bug-> The output cable Y has a starting location of output cableX i.e. outputportx...
	OutputPortCarry = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutputPortCarry"));
	OutputPortCarry->SetupAttachment(RootComponent);

	CableConnector = CreateDefaultSubobject<USceneComponent>("CableConnector");
	CableConnector->SetupAttachment(OutputPortX);

	CableConnectorCarry = CreateDefaultSubobject<USceneComponent>("CableCarry");
	CableConnectorCarry->SetupAttachment(OutputPortCarry);
	
	OutputCableX = CreateDefaultSubobject<UCableComponent>("AdderOutputCableX");
	//OutputCableX->AttachToComponent(CableConnector, FAttachmentTransformRules::KeepWorldTransform);

	OutputCableX->SetupAttachment(CableConnector);
	
	OutputCableX->SetAttachEndTo(this, "Cable Component"); // Why the space, I wish I knew...
	// TODO: Why is this Cable Component and not Cable Connector? 
	
	OutputCableY = CreateDefaultSubobject<UCableComponent>("AdderOutputCableY");
	//OutputCableY->AttachToComponent(CableConnectorCarry, FAttachmentTransformRules::KeepWorldTransform);

	OutputCableY->SetupAttachment(CableConnectorCarry);

	OutputCableY->SetAttachEndTo(this, "Cable Connector Carry"); // Why the space, I wish I knew...

	carryOutNode->SetOutputCableX(OutputCableY);
	
	SetCableConnectNumber(GetSerialNumber());
	SetCableConnectString("Cable Component");
	SetCableConnectFrom("X");

	SetCableConnectNumber(GetSerialNumber());
	SetCableConnectString("Cable Component");
	SetCableConnectFrom("X");
	
	SetupMeshes();
	SetupMaterials();

	// Set default material for the mesh
	if (DisabledMaterial)
	{
		InputPortX->SetMaterial(0, DisabledMaterial);
		InputPortY->SetMaterial(0, DisabledMaterial);
		InputPortZ->SetMaterial(0, DisabledMaterial);
		OutputPortX->SetMaterial(0, DisabledMaterial);
		OutputPortCarry->SetMaterial(0, DisabledMaterial);
	}
	if (StandMaterial)
	{
		DisplayMesh->SetMaterial(0, StandMaterial);
	}

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(100.0f); // Set the radius of the collision sphere
	CollisionSphere->SetupAttachment(RootComponent);
}


void AFullAdder::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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


void AFullAdder::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
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

void AFullAdder::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFullAdder::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AFullAdder::OnOverlapEnd);
}

void AFullAdder::SetInputX(AAbstractNode* input)
{
	inputX = input;
	input->Attach(this);
	connectedNodes_.push_back(input);

	xorGateA->SetInputX(input);
	andGateA->SetInputX(input);

	inputX->GetOutputCableX()->SetAttachEndTo(this, "InputPortX");

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
	
	Update(GetState());
}

void AFullAdder::SetInputY(AAbstractNode* input)
{
	inputY = input;
	input->Attach(this);
	connectedNodes_.push_back(input);

	xorGateA->SetInputY(inputY);
	andGateA->SetInputY(inputY);

	inputY->GetOutputCableX()->SetAttachEndTo(this, "InputPortY");

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
	
	Update(GetState());
}

void AFullAdder::SetInputCarry(AAbstractNode* input)
{
	if (auto fullAdder = Cast<AFullAdder>(input))
	{
		inputCarry = fullAdder->GetCarryOutNode();
		inputCarry->SetOutputCableX(fullAdder->GetOutputCableY());
		inputCarry->GetOutputCableX()->SetAttachEndTo(this, "InputPortCarry"); //InputPortZ does not work
	}
	else
	{
		inputCarry = input;
		inputCarry->GetOutputCableX()->SetAttachEndTo(this, "InputPortCarry"); //InputPortZ does not work
	}
	
	input->Attach(this);
	connectedNodes_.push_back(input);

	xorGateB->SetInputY(inputCarry);
	andGateB->SetInputY(inputCarry);

	eLogicState currentState = inputCarry->GetState();

	if (currentState == DISABLED)
	{
		InputPortZ->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		InputPortZ->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		InputPortZ->SetMaterial(0, OnMaterial);
	}
	
	AFullAdder::Update(GetState());
}

AAbstractNode* AFullAdder::GetCarryOutNode() const
{
	return carryOutNode;
}

// TODO: implement remove functions for FullAdder. Perhaps with LB + X,Y,Z. Can we do key chords in Unreal?
void AFullAdder::RemoveInputX()
{
}

void AFullAdder::RemoveInputY()
{
}

void AFullAdder::RemoveInputCarry()
{
}

void AFullAdder::Update(eLogicState state)
{
	xorGateA->Update(xorGateA->GetState());
	andGateA->Update(andGateA->GetState());
	xorGateB->SetInputX(xorGateA);
	andGateB->SetInputX(xorGateA);
	orGate->SetInputX(andGateB);
	orGate->SetInputY(andGateA);
	outputSumState_ = xorGateB->GetState();
	outputCarryState_ = orGate->GetState();
	carryOutNode->Update(outputCarryState_);

	if (inputX)
	{
		switch (inputX->GetState())
		{
		case DISABLED:
			InputPortX->SetMaterial(0, DisabledMaterial);
			break;
		case OFF:
			InputPortX->SetMaterial(0, OffMaterial);
			break;
		case ON:
			InputPortX->SetMaterial(0, OnMaterial);
			break;
		}
	}
	if (inputY)
	{
		switch (inputY->GetState())
		{
		case DISABLED:
			InputPortY->SetMaterial(0, DisabledMaterial);
			break;
		case OFF:
			InputPortY->SetMaterial(0, OffMaterial);
			break;
		case ON:
			InputPortY->SetMaterial(0, OnMaterial);
			break;
		}
	}
	if (inputCarry)
	{
		switch (inputCarry->GetState())
		{
		case DISABLED:
			InputPortZ->SetMaterial(0, DisabledMaterial);
			break;
		case OFF:
			InputPortZ->SetMaterial(0, OffMaterial);
			break;
		case ON:
			InputPortZ->SetMaterial(0, OnMaterial);
			break;
		}
	}
	eLogicState outputSum = GetState();
	switch(outputSum)
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
	eLogicState outputCarry = GetCarryOut();
	switch(outputCarry)
	{
	case DISABLED:
		OutputPortCarry->SetMaterial(0, DisabledMaterial);
		break;
	case OFF:
		OutputPortCarry->SetMaterial(0, OffMaterial);
		break;
	case ON:
		OutputPortCarry->SetMaterial(0, OnMaterial);
		break;
	}
	Notify();
}

eLogicState AFullAdder::GetCarryOut() const
{
	return orGate->GetState();
}

eLogicState AFullAdder::GetState() const
{
	if (inputX && inputY) {
		return xorGateB->GetState();
	}
	return DISABLED;
}

FString AFullAdder::SerializeNode()
{
	// Create a JSON array to store node entries
	//TArray<TSharedPtr<FJsonValue>> connectedNodesArray;
	//TArray<TSharedPtr<FJsonValue>> observersArray;

	// Create a JSON array to store node entries
	TArray<TSharedPtr<FJsonValue>> connectionsArray;
	

	for (const auto& Pair : GetConnectionSerializationArray())
	{
		// Create a JSON object for each node
		TSharedPtr<FJsonObject> NodeObject = MakeShareable(new FJsonObject);
		NodeObject->SetNumberField(TEXT("serialNumber"), Pair.Key);
		NodeObject->SetStringField(TEXT("xOrY"), Pair.Value);

		// Add the node object to the array
		TSharedPtr<FJsonValueObject> NodeJsonValue = MakeShareable(new FJsonValueObject(NodeObject));
		connectionsArray.Add(NodeJsonValue);
	}

	// TODO: Add and implement connections array with pairs of key sttring
	// could do something like outx,inx but for now is implemented
	// elsewhere as inx and iny for example

	// Report Transform information
	FTransform NodeTransform = GetActorTransform();
	FVector NodePosition = NodeTransform. GetTranslation();
	FRotator NodeRotation = NodeTransform.Rotator();
	

	// Create a JSON object to hold the array of nodes
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	
	RootObject->SetStringField(TEXT("nodeName"), GetNodeName());
	RootObject->SetNumberField(TEXT("serialNumber"), GetSerialNumber());

	TSharedPtr<FJsonObject> PositionObject = MakeShareable(new FJsonObject);
	PositionObject->SetNumberField(TEXT("x"), NodePosition.X);
	PositionObject->SetNumberField(TEXT("y"), NodePosition.Y);
	PositionObject->SetNumberField(TEXT("z"), NodePosition.Z);

	TSharedPtr<FJsonObject> RotationObject = MakeShareable(new FJsonObject);
	RotationObject->SetNumberField(TEXT("pitch"), NodeRotation.Pitch);
	RotationObject->SetNumberField(TEXT("yaw"), NodeRotation.Yaw);
	RotationObject->SetNumberField(TEXT("roll"), NodeRotation.Roll);

	RootObject->SetObjectField(TEXT("position"), PositionObject);
	RootObject->SetObjectField(TEXT("rotation"), RotationObject);
	RootObject->SetArrayField(TEXT("connections"), connectionsArray);
	
	// Create a writer and write JSON to string
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), JsonWriter);

	return OutputString;
}

void AFullAdder::SetupMeshes()
{

	// TODO: Remove log statements below...
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ConnectionAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Meshes/Mesh_Connection'"));
	if (ConnectionAsset.Succeeded())
	{
		InputPortX->SetStaticMesh(ConnectionAsset.Object);
		InputPortY->SetStaticMesh(ConnectionAsset.Object);
		InputPortZ->SetStaticMesh(ConnectionAsset.Object);
		OutputPortX->SetStaticMesh(ConnectionAsset.Object);
		OutputPortCarry->SetStaticMesh(ConnectionAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	FullAdderAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Meshes/Mesh_FullAdder'"));
	if (FullAdderAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(FullAdderAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned mesh asset to AndGate!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find AndGate Asset."));
	}
}

void AFullAdder::SetupMaterials()
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
