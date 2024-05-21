 // Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/DisplayOutput.h"

#include "LogicGates/LogicGatesCharacter.h"
#include "Nodes/Adders/FullAdder.h"

 ADisplayOutput::ADisplayOutput()
{

 	SetNodeName("DisplayOutput");

 	SetDeserializationNumber(SerialNumber);
 	
	// Set default mesh

 	DisplaySceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Display SceneComponent"));
 	RootComponent = DisplaySceneComponent;
 	
	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(RootComponent);

	// Create and attach the button mesh
	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	ScreenMesh->SetupAttachment(RootComponent);

	// Create and attach the connection port mesh
	InputPort = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputPort"));
	InputPort->SetupAttachment(RootComponent);
 	
 	CableConnector = CreateDefaultSubobject<USceneComponent>("CableConnector");
	CableConnector->SetupAttachment(InputPort);
 	
	SetupMeshes();	
	SetupMaterials();
	
	// Set default material for the mesh
	if (DisabledMaterial)
	{
		InputPort->SetMaterial(0, DisabledMaterial);
		ScreenMesh->SetMaterial(0, DisabledMaterial);
	}
	if (StandMaterial)
	{
		
		DisplayMesh->SetMaterial(0, StandMaterial);
	}
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(100.0f); // Set the radius of the collision sphere
	CollisionSphere->SetupAttachment(RootComponent);

 	//OutputCableX = nullptr; // Display has NO output cable!!!
 	OutputCableX = CreateDefaultSubobject<UCableComponent>("DisplayConnected");
    //OutputCableX->AttachToComponent(CableConnector, FAttachmentTransformRules::KeepWorldTransform);

 	OutputCableX->SetupAttachment(CableConnector);
	
 	OutputCableX->SetAttachEndToComponent(CableConnector);
 	OutputCableX->SetupAttachment(RootComponent);
 	OutputCableX->SetVisibility(false);
 	
	input_ = nullptr; // TODO: is this necessary?
	bIsLit = false;
}

void ADisplayOutput::BeginPlay()
 {
 	Super::BeginPlay();
	
 	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ADisplayOutput::OnOverlapBegin);
 	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ADisplayOutput::OnOverlapEnd);
 }


void ADisplayOutput::SetInput(AAbstractNode *input) {

 	if (auto fullAdder = Cast<AFullAdder>(input))
 	{
 		if (fullAdder->GetFromOutputY()) // TODO: What if we pass from output Y first, then OutputX. Hmm...
 		{
 			input_ = fullAdder->GetCarryOutNode();
 			fullAdder->GetOutputCableY()->SetAttachEndTo(this, "InputPort");
 			fullAdder->SetCableConnectNumber(this->GetSerialNumber());
 			fullAdder->SetCableConnectString("InputPort");

 			// TODO: handle the serialization of an output Y cable... This only helps for output X, right?
 		}
 		else
 		{
 			input_= input;
 			input_->GetOutputCableX()->SetAttachEndTo(this, "InputPort");
 			input_->SetCableConnectNumber(this->GetSerialNumber());
 			input_->SetCableConnectString("InputPort");
 		}
 	}
 	else
 	{
 		input_ = input;
 		input_->GetOutputCableX()->SetAttachEndTo(this, "InputPort");
 	}
 	
	input_->Attach(this);
 	connectedNodes_.push_back(input);
 	
	eLogicState currentState = input_->GetState();
	if (currentState == DISABLED)
	{
		ScreenMesh->SetMaterial(0, DisabledMaterial);
		InputPort->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		ScreenMesh->SetMaterial(0, OffMaterial);
		InputPort->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		ScreenMesh->SetMaterial(0, OnMaterial);
		InputPort->SetMaterial(0, OnMaterial);
	}
	
	ADisplayOutput::Update(currentState);
}

eLogicState ADisplayOutput::GetState() const {
	return state_;
}

void ADisplayOutput::Update(eLogicState state) {
	state_ = state;

	const eLogicState currentState = state;

	if (currentState == DISABLED)
	{
		ScreenMesh->SetMaterial(0, DisabledMaterial);
		InputPort->SetMaterial(0, DisabledMaterial);
	}
	if (currentState == OFF)
	{
		ScreenMesh->SetMaterial(0, OffMaterial);
		InputPort->SetMaterial(0, OffMaterial);
	}
	if (currentState == ON)
	{
		ScreenMesh->SetMaterial(0, OnMaterial);
		InputPort->SetMaterial(0, OnMaterial);
	}
	
	Notify();
}

FString ADisplayOutput::SerializeNode()
{

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
	
	// Create a writer and write JSON to string
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), JsonWriter);

	return OutputString;
}

 // TODO: Remove Log messages from Meshes
void ADisplayOutput::SetupMeshes()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	DisplayAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Meshes/Mesh_Display'"));
	if (DisplayAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(DisplayAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned mesh asset to Display!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Power Source Asset."));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ScreenAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Meshes/Mesh_Screen'"));	
	if (ScreenAsset.Succeeded())
	{
		ScreenMesh->SetStaticMesh(ScreenAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ConnectionAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Meshes/Mesh_Connection'"));
	if (ConnectionAsset.Succeeded())
	{
		InputPort->SetStaticMesh(ConnectionAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}
}

// TODO: Move SetupMaterials to AbstractNode.
void ADisplayOutput::SetupMaterials()
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

void ADisplayOutput::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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

void ADisplayOutput::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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