// Copyright Epic Games, Inc. All Rights Reserved.

#include "LogicGatesCharacter.h"

#include "AbstractNode.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Nodes/DisplayOutput.h"
#include "Nodes/PowerSource.h"
#include "Nodes/AbstractTwoInputNode.h"
#include "Nodes/Adders/FullAdder.h"
#include "Nodes/LogicGates/TriStateBufferGate.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ALogicGatesCharacter

ALogicGatesCharacter::ALogicGatesCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	LastOutputNode = nullptr;
	ConnectedToHand = false;
}

void ALogicGatesCharacter::SetCurrentNode(AAbstractNode* currentNode)
{
	CurrentNode = currentNode;
}

void ALogicGatesCharacter::SetLastOutputNode(AAbstractNode* lastOutputNode)
{
	LastOutputNode = lastOutputNode;
}

FString ALogicGatesCharacter::SerializeAllNodes(const TArray<AAbstractNode*>& AllNodes)
{
	// Create a JSON array to store node entries
	TArray<TSharedPtr<FJsonValue>> NodesArray;

	// Iterate through each node
	for (AAbstractNode* Node : AllNodes)
	{
		FString NodeJson = Node->SerializeNode();
		TSharedPtr<FJsonObject> NodeJsonObject;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(NodeJson);
		if (FJsonSerializer::Deserialize(JsonReader, NodeJsonObject))
		{
			TSharedPtr<FJsonValueObject> NodeJsonValue = MakeShareable(new FJsonValueObject(NodeJsonObject));
			NodesArray.Add(NodeJsonValue);
		}
		else
		{
			// Failed to deserialize node JSON, handle error
		}
	}

	// Create a JSON object to hold the array of nodes
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	RootObject->SetArrayField(TEXT("nodes"), NodesArray);

	// Create a writer and write JSON to string
	FString OutputString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), JsonWriter);

	return OutputString;
}


FString ALogicGatesCharacter::ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Read String From File Failed - File doesn't exist - '%s'"), *FilePath);
		return "";
	}

	FString RetString = "";

	// Try to read the file. Output goes into RetString

	if (!FFileHelper::LoadFileToString(RetString, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Read String From File Faild - Was not able to read file. Is this a text file? - '%s'"), *FilePath);
		return "";
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Read String From File Succeeded - '%s'"), *FilePath);
	return RetString;
}

void ALogicGatesCharacter::WriteStringToFile(FString FilePath, FString String, bool& bOutSuccess,
                                             FString& OutInfoMessage)
{
	if (!FFileHelper::SaveStringToFile(String, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Write String To File Failed - Was not able to write to file. Is your file read only? Is the path valid? - '%s'"), *FilePath);
		return; // Not necessary?
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Write String To File Succeeded - '%s'"), *FilePath);
}

FString ALogicGatesCharacter::GetUniqueFilePath(const FString& BasePath)
{
	FString DirectoryPath = FPaths::GetPath(BasePath); // Get the directory path
	FString FileName = FPaths::GetCleanFilename(BasePath); // Get the file name without the directory path

	FString UniquePath = BasePath;
	int32 Index = 0;

	// Check if the base path already exists
	while (FPaths::FileExists(UniquePath))
	{
		// Increment the index and construct a new file path
		Index++;
		UniquePath = DirectoryPath + FString::Printf(TEXT("/%s%d.%s"), *FPaths::GetBaseFilename(FileName), Index, *FPaths::GetExtension(FileName));
	}

	return UniquePath;
}

AAbstractNode* ALogicGatesCharacter::DeserializeNode(FString nodeJson)
{
    // Deserialize the JSON string into a JSON object
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(nodeJson);
    if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
    {
        // Failed to deserialize JSON, handle error
        return nullptr;
    }

    // Extract node properties from the JSON object
    FString NodeName;
    int32 SerialNumber;
	FString PowerSourceState;
    FVector Position;
    FRotator Rotation;
    //TArray<TSharedPtr<FJsonValue>> ObserversJsonArray;

    if (!JsonObject->TryGetStringField(TEXT("nodeName"), NodeName) ||
        !JsonObject->TryGetNumberField(TEXT("serialNumber"), SerialNumber) ||
        !JsonObject->GetObjectField(TEXT("position"))->TryGetNumberField(TEXT("x"), Position.X) ||
        !JsonObject->GetObjectField(TEXT("position"))->TryGetNumberField(TEXT("y"), Position.Y) ||
        !JsonObject->GetObjectField(TEXT("position"))->TryGetNumberField(TEXT("z"), Position.Z) ||
        !JsonObject->GetObjectField(TEXT("rotation"))->TryGetNumberField(TEXT("pitch"), Rotation.Pitch) ||
        !JsonObject->GetObjectField(TEXT("rotation"))->TryGetNumberField(TEXT("yaw"), Rotation.Yaw) ||
        !JsonObject->GetObjectField(TEXT("rotation"))->TryGetNumberField(TEXT("roll"), Rotation.Roll))
		// Doesn't work, and we want to populate maps in another step.
        //!JsonObject->TryGetArrayField(TEXT("connectedNodes"), ConnectedNodesJsonArray) ||
        //!JsonObject->TryGetArrayField(TEXT("observers"), ObserversJsonArray))
    {
        // Failed to extract required fields, handle error
        return nullptr;
    }

	if (NodeName == "PowerSource")
	{
		JsonObject->TryGetStringField(TEXT("powerSourceState"), PowerSourceState);
	}
	
	//Deserialize connected nodes
	const TArray<TSharedPtr<FJsonValue>>* ConnectedNodesArrayPtr;
	
	
    // Create an instance of the appropriate node class based on the node name
    AAbstractNode* NewNode = nullptr;
    if (NodeName == "PowerSource")
    {
        NewNode = NewObject<APowerSource>();
    	auto ps = Cast<APowerSource>(NewNode);
    	if (PowerSourceState == "DISABLED")
    	{
    		ps->ChangeState(DISABLED);
    	}
    	if (PowerSourceState == "OFF")
    	{
    		ps->ChangeState(OFF);
    	}
    	if (PowerSourceState == "ON")
    	{
    		ps->ChangeState(ON);
    	}
    }
    else if (NodeName == "DisplayOutput")
    {
        NewNode = NewObject<ADisplayOutput>();
    }
    else if (NodeName == "AndGate")
    {
        NewNode = NewObject<AAndGate>();
    }
    // Add more cases for other node types as needed

    else if (NodeName == "XorGate")
    {
    	NewNode = NewObject<AXorGate>();
    }
    else if (NodeName == "OrGate")
    {
    	NewNode = NewObject<AOrGate>();
    }
	
if (NewNode)
    {
        // Set node properties
        NewNode->SetNodeName(NodeName);
        NewNode->SetDeserializationNumber(SerialNumber);
        NewNode->SetActorLocation(Position);
        NewNode->SetActorRotation(Rotation);

        // Deserialize connected nodes and observers if needed

    	if (JsonObject->TryGetArrayField(TEXT("connections"), ConnectedNodesArrayPtr))
    	{
    		for (const auto& ConnectedNodeValue : *ConnectedNodesArrayPtr)
    		{
    			const TSharedPtr<FJsonObject> ConnectedNodeObject = ConnectedNodeValue->AsObject();
    			if (ConnectedNodeObject.IsValid())
    			{
    				FKeyValuePair kvPair;
    				ConnectedNodeObject->TryGetNumberField(TEXT("serialNumber"), kvPair.Key);
    				ConnectedNodeObject->TryGetStringField(TEXT("xOrY"), kvPair.Value);
    				//ConnectedNodes.Add(ConnectedNode);
    				NewNode->AddNodeToConnectionSerializationArrayKV(kvPair);
    			}
    		}
    	}
    	
        // Update position and rotation if needed

        // Any other custom logic
    }

    return NewNode;
}

// AAbstractNode* DeserializeNodeFromJsonObject(const TSharedPtr<FJsonObject>& NodeJsonObject)
// {
//     // Extract node properties from JSON object
//     FString NodeName;
//     int32 SerialNumber = -1; // Set a default value to indicate an error
//     FVector NodePosition;
//     FRotator NodeRotation;
//
//     // Deserialize basic properties
//     NodeJsonObject->TryGetStringField(TEXT("nodeName"), NodeName);
//     NodeJsonObject->TryGetNumberField(TEXT("serialNumber"), SerialNumber);
//
//     // Deserialize position
//     const TSharedPtr<FJsonObject>* PositionObjectPtr;
//     if (NodeJsonObject->TryGetObjectField(TEXT("position"), PositionObjectPtr))
//     {
//         const TSharedPtr<FJsonObject>& PositionObject = *PositionObjectPtr;
//         PositionObject->TryGetNumberField(TEXT("x"), NodePosition.X);
//         PositionObject->TryGetNumberField(TEXT("y"), NodePosition.Y);
//         PositionObject->TryGetNumberField(TEXT("z"), NodePosition.Z);
//     }
//
//     // Deserialize rotation
//     const TSharedPtr<FJsonObject>* RotationObjectPtr;
//     if (NodeJsonObject->TryGetObjectField(TEXT("rotation"), RotationObjectPtr))
//     {
//         const TSharedPtr<FJsonObject>& RotationObject = *RotationObjectPtr;
//         RotationObject->TryGetNumberField(TEXT("pitch"), NodeRotation.Pitch);
//         RotationObject->TryGetNumberField(TEXT("yaw"), NodeRotation.Yaw);
//         RotationObject->TryGetNumberField(TEXT("roll"), NodeRotation.Roll);
//     }
//
//     // Deserialize connected nodes
//     // const TArray<TSharedPtr<FJsonValue>>* ConnectedNodesArrayPtr;
//     // if (NodeJsonObject->TryGetArrayField(TEXT("connectedNodes"), ConnectedNodesArrayPtr))
//     // {
//     //     for (const auto& ConnectedNodeValue : *ConnectedNodesArrayPtr)
//     //     {
//     //         const TSharedPtr<FJsonObject> ConnectedNodeObject = ConnectedNodeValue->AsObject();
//     //         if (ConnectedNodeObject.IsValid())
//     //         {
//     //             FNodeConnection ConnectedNode;
//     //             ConnectedNodeObject->TryGetNumberField(TEXT("serialNumber"), ConnectedNode.SerialNumber);
//     //             ConnectedNodeObject->TryGetStringField(TEXT("nodeName"), ConnectedNode.NodeName);
//     //             ConnectedNodes.Add(ConnectedNode);
//     //         }
//     //     }
//     // }
//
//     // Deserialize observers
//     // const TArray<TSharedPtr<FJsonValue>>* ObserversArrayPtr;
//     // if (NodeJsonObject->TryGetArrayField(TEXT("observers"), ObserversArrayPtr))
//     // {
//     //     for (const auto& ObserverValue : *ObserversArrayPtr)
//     //     {
//     //         const TSharedPtr<FJsonObject> ObserverObject = ObserverValue->AsObject();
//     //         if (ObserverObject.IsValid())
//     //         {
//     //             FNodeObserver Observer;
//     //             ObserverObject->TryGetNumberField(TEXT("serialNumber"), Observer.SerialNumber);
//     //             ObserverObject->TryGetStringField(TEXT("nodeName"), Observer.NodeName);
//     //             Observers.Add(Observer);
//     //         }
//     //     }
//     // }
//
//     // Create the appropriate node object based on the node name
//     AAbstractNode* Node = nullptr;
//     if (NodeName == "DisplayOutput")
//     {
//         // Create a DisplayOutput node
//         Node = NewObject<ADisplayOutput>();
//     }
//     else if (NodeName == "PowerSource")
//     {
//         // Create a PowerSource node
//         Node = NewObject<APowerSource>();
//     }
//     else if (NodeName == "AndGate")
//     {
//         // Create an AndGate node
//         Node = NewObject<AAndGate>();
//     }
//     // Add more conditions for other node types as needed
//
//     // Set properties of the created node
//     if (Node)
//     {
//         Node->SetNodeName(NodeName);
//         Node->SetDeserializationNumber(SerialNumber);
//     	Node->SetActorLocation(NodePosition);
//     	Node->SetActorRotation(NodeRotation);
//     }
//
//     return Node;
// }

FString SerializeJsonObjectToString(const TSharedPtr<FJsonObject>& NodeJsonObject)
{
	FString OutputString;

	// Check if the JSON object is valid
	if (NodeJsonObject.IsValid())
	{
		// Create a writer and write JSON to string
		TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(NodeJsonObject.ToSharedRef(), JsonWriter);
	}

	return OutputString;
}


TMap<int, AAbstractNode*> ALogicGatesCharacter::DeserializeAllNodes(FString fullJson)
{
	TMap<int, AAbstractNode*> NodeMap;

	// Parse the JSON string
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(fullJson);
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		// Extract node array from the JSON object
		TArray<TSharedPtr<FJsonValue>> NodeArray;
		if (JsonObject->HasField(TEXT("nodes")))
		{
			const TArray<TSharedPtr<FJsonValue>> FoundArray = JsonObject->GetArrayField(TEXT("nodes"));
			if (!FoundArray.IsEmpty())
			{
				NodeArray = FoundArray;
				// Process the array

				// Iterate over each node entry in the JSON array
				// for (const auto& NodeValue : NodeArray)
				// {
				// 	// Deserialize the node from JSON
				// 	FString NodeJsonString = NodeValue->AsString();
				// 	AAbstractNode* Node = DeserializeNode(NodeJsonString);
				//
				// 	// If deserialization was successful, add the node to the map
				// 	if (Node)
				// 	{
				// 		// Retrieve DeserializationNumber from the node and use it as the key
				// 		int32 DeserializationNumber = Node->GetDeserializationNumber();
				// 		NodeMap.Add(DeserializationNumber, Node);
				// 	}
				// }

				for (const auto& NodeValue : NodeArray)
				{
					// Get the JSON object representing a node
					const TSharedPtr<FJsonObject> NodeObject = NodeValue->AsObject();
					if (!NodeObject.IsValid())
					{
						// Handle invalid JSON object
						continue;
					}

					// Deserialize the node from JSON
					AAbstractNode* Node = DeserializeNode(SerializeJsonObjectToString(NodeObject));

					// If deserialization was successful, add the node to the map
					if (Node)
					{
						// Retrieve DeserializationNumber from the node and use it as the key
						int32 DeserializationNumber = Node->GetDeserializationNumber();
						NodeMap.Add(DeserializationNumber, Node);
					}
				}
			}
		}
	}

	return NodeMap;
}



void ALogicGatesCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALogicGatesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALogicGatesCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALogicGatesCharacter::Look);

		// Toggle Power Source
		EnhancedInputComponent->BindAction(PowerToggleAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ToggleCurrentPowerSource);

		EnhancedInputComponent->BindAction(OpenBuildMenuAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::OpenBuildMenu);
		EnhancedInputComponent->BindAction(PlaceObjectAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::PlaceObject);
		
		// Connect output to inputs -- Note this is a WORK IN PROGRESS
		EnhancedInputComponent->BindAction(ConnectFromOutputXAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectFromOutputX);
		EnhancedInputComponent->BindAction(ConnectFromOutputYAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectFromOutputY);
		EnhancedInputComponent->BindAction(ConnectToInputXAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectToInputX);
		EnhancedInputComponent->BindAction(ConnectToInputYAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectToInputY);
		EnhancedInputComponent->BindAction(ConnectToInputZAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectToInputZ);
		//EnhancedInputComponent->BindAction(SerializeAllAction, ETriggerEvent::Started, &ALogicGatesCharacter::ConnectToInputZ);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("****'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALogicGatesCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALogicGatesCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

// TODO: Remove these, implemented in Blueprint!
void ALogicGatesCharacter::OpenBuildMenu()
{
}

void ALogicGatesCharacter::PlaceObject()
{
}

void ALogicGatesCharacter::ToggleCurrentPowerSource()
{
	UE_LOG(LogTemplateCharacter, Warning, TEXT("Toggle Function is working properly, when we press the A key..."));
	
	if (CurrentNode)
	{
		// If the current node is valid, we then type cast it to a power source.
		APowerSource* toggleSwitch = Cast<APowerSource>(CurrentNode);

		if (toggleSwitch)
		{
			UE_LOG(LogTemp, Display, TEXT("****Entered radius of Power Source and pressed Power Action Button!!!"));
			if (toggleSwitch->GetState() == DISABLED)
			{
				toggleSwitch->ChangeState(OFF);
			}
			else if (toggleSwitch->GetState() == OFF)
			{
				toggleSwitch->ChangeState(ON);
			}
			else // Can only be ON at this point, provided the pointer is valid
			{
				// We change to DISABLED, so we can simply toggle between the three.
				toggleSwitch->ChangeState(DISABLED);
			}
		}
	}
}

void ALogicGatesCharacter::ConnectFromOutputX()
{
	if (CurrentNode)
	{
		if (CurrentNode->GetHasOutputNode())
		{
			LastOutputNode = CurrentNode;

			ConnectedToHand = true;
		}	
	}
	
	// TODO: Remove Connected to Hand?
	// NOTE: It seems necessary, at least to signal that I'm connecting. Perhaps a rename?
}

void ALogicGatesCharacter::ConnectFromOutputY()
{
	if (CurrentNode)
	{
		if (auto fullAdder = Cast<AFullAdder>(CurrentNode))
		{
			LastOutputNode = fullAdder;
			fullAdder->SetFromOutputY(true); // This is the perfect place to do this

			ConnectedToHand = true;
			IsFromOutputY = true;
		}
		if (CurrentNode->GetHasOutputNode())
		{
			// TODO: What was I doing here???
		}
	}
}


void ALogicGatesCharacter::ConnectToInputX()
{
	// TODO: Research Line Tracing
	// Perhaps this can only be used in a first person mode.
	// This would necessaitate a complete overhaul of the character class...
	
	// FVector Loc;
	// FRotator Rot;
	// FHitResult Hit;

	//GetController()->GetPlayerViewPoint(Loc, Rot);
	// Loc = GetFollowCamera()->GetComponentLocation();
	// Rot = GetFollowCamera()->GetComponentRotation();
	//
	// //Start = Start + FVector(0,0,50); // Get the start vector to start at around the head.
	// //Loc -= FVector(0,0,20);
	// //Rot += FRotator(20,0,0);
	// FVector Start = Loc;
	// FVector End = Start + (Rot.Vector() * 2000);
	// FCollisionQueryParams TraceParams;
	//
	// GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	//
	// // Check if it's a display input
	// if (auto display = dynamic_cast<ADisplayOutput*>(Hit.GetActor))
	// {
	// 	display->
	// }
	//
	// AAbstractNode* node = (AAbstractNode*) Hit.GetActor();
	// if (node)
	//
	// UE_LOG(LogTemplateCharacter, Warning, TEXT("Trace Function is working properly, when we press the X key..."));
	//
	//
	// DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);

	// TODO: Perhaps we don't need ConnectedToHand?
	// TODO: It seems necessary, we might need a rename.

	// TODO: Figure out how to connect a cable to player's hand?
	if (ConnectedToHand)
	{
		if (LastOutputNode && CurrentNode)
		{
			if (auto display = Cast<ADisplayOutput>(CurrentNode))
			{
				if (IsFromOutputY) // Can only be set if the last node was a FullAdder
				{
					auto fullAdder = Cast<AFullAdder>(LastOutputNode);
					//display->SetInput(fullAdder->GetCarryOutNode());
					fullAdder->SetCableConnectFrom("Y"); // TODO: Implement x and y output serialization
					display->SetInput(fullAdder);
					fullAdder->AddNodeToConnectionSerializationArray(display->GetSerialNumber(), "X");
					IsFromOutputY = false;
				}
				else
				{
					if (auto fullAdder = Cast<AFullAdder>(LastOutputNode))
					{
						fullAdder->SetCableConnectFrom("X");
						display->SetInput(fullAdder);
						fullAdder->AddNodeToConnectionSerializationArray(display->GetSerialNumber(), "X");
					}
					else
					{
						display->SetInput(LastOutputNode);
						LastOutputNode->AddNodeToConnectionSerializationArray(display->GetSerialNumber(), "X");
					}
				}
				
				ConnectedToHand = false;
			}

			if (auto twoInputNode = Cast<AAbstractTwoInputNode>(CurrentNode))
			{
				if (IsFromOutputY)
				{
					auto fullAdder = Cast<AFullAdder>(LastOutputNode);
					fullAdder->SetCableConnectFrom("Y"); // TODO: Implement x and y output serialization
					twoInputNode->SetInputX(fullAdder->GetCarryOutNode());
					fullAdder->AddNodeToConnectionSerializationArray(twoInputNode->GetSerialNumber(), "X");
					IsFromOutputY = false;
				}
				else
				{
					LastOutputNode->SetCableConnectFrom("X");
					twoInputNode->SetInputX(LastOutputNode);
					LastOutputNode->AddNodeToConnectionSerializationArray(twoInputNode->GetSerialNumber(), "X");
				}
				ConnectedToHand = false;
			}
			if (auto currentFullAdder = Cast<AFullAdder>(CurrentNode))
			{
				if (IsFromOutputY)
				{
					auto lastFullAdder = Cast<AFullAdder>(LastOutputNode);
					lastFullAdder->SetCableConnectFrom("Y");
					currentFullAdder->SetInputX(lastFullAdder->GetCarryOutNode());
					lastFullAdder->AddNodeToConnectionSerializationArray(currentFullAdder->GetSerialNumber(), "X");
					IsFromOutputY = false;
				}
				else
				{
					currentFullAdder->SetCableConnectFrom("X");
					currentFullAdder->SetInputX(LastOutputNode);
					LastOutputNode->AddNodeToConnectionSerializationArray(currentFullAdder->GetSerialNumber(), "X");
				}
				ConnectedToHand = false;
			}

			if (auto triStateNode = Cast<ATriStateBufferGate>(CurrentNode))
			{
				if (IsFromOutputY)
				{
					auto fullAdder = Cast<AFullAdder>(LastOutputNode);
					fullAdder->SetCableConnectFrom("Y");
					triStateNode->SetDataInput(fullAdder->GetCarryOutNode());
					// We're treating Data as X and Enable as Y
					fullAdder->AddNodeToConnectionSerializationArray(triStateNode->GetSerialNumber(), "X");
					IsFromOutputY = false;
				}
				else
				{
					LastOutputNode->SetCableConnectFrom("X");
					triStateNode->SetDataInput(LastOutputNode);
					LastOutputNode->AddNodeToConnectionSerializationArray(triStateNode->GetSerialNumber(), "X");
				}
				ConnectedToHand = false;
			}
		}
	}
}

void ALogicGatesCharacter::ConnectToInputY()
{
	if (ConnectedToHand)
	{
		if (LastOutputNode && CurrentNode)
		{
			if (auto twoInputNode = Cast<AAbstractTwoInputNode>(CurrentNode))
			{

				if (IsFromOutputY)
				{
					auto fullAdder = Cast<AFullAdder>(LastOutputNode);
					fullAdder->SetCableConnectFrom("Y");
					twoInputNode->SetInputY(fullAdder->GetCarryOutNode());
					fullAdder->AddNodeToConnectionSerializationArray(twoInputNode->GetSerialNumber(), "Y");
					IsFromOutputY = false;
				}
				else
				{
					LastOutputNode->SetCableConnectFrom("X");
					twoInputNode->SetInputY(LastOutputNode);
					LastOutputNode->AddNodeToConnectionSerializationArray(twoInputNode->GetSerialNumber(), "Y");
				}
			}

			if (auto currentFullAdder = Cast<AFullAdder>(CurrentNode))
			{
				if (IsFromOutputY)
				{
					auto lastFullAdder = Cast<AFullAdder>(LastOutputNode);
					lastFullAdder->SetCableConnectFrom("Y");
					currentFullAdder->SetInputY(lastFullAdder->GetCarryOutNode());
					// TODO: Implement x and y output serialization
					lastFullAdder->AddNodeToConnectionSerializationArray(currentFullAdder->GetSerialNumber(), "Y");

					IsFromOutputY = false;
				}
				else
				{
					currentFullAdder->SetCableConnectFrom("X");
					currentFullAdder->SetInputY(LastOutputNode);
					LastOutputNode->AddNodeToConnectionSerializationArray(currentFullAdder->GetSerialNumber(), "Y");
				}
				ConnectedToHand = false;
			}

			if (auto triStateNode = Cast<ATriStateBufferGate>(CurrentNode))
			{
				if (IsFromOutputY)
				{
					auto fullAdder = Cast<AFullAdder>(LastOutputNode);
					fullAdder->SetCableConnectFrom("Y");
					triStateNode->SetEnableInput(fullAdder->GetCarryOutNode());
					// We treat input X as Data and input Y as enable
					fullAdder->AddNodeToConnectionSerializationArray(triStateNode->GetSerialNumber(), "Y");

					IsFromOutputY = false;
				}
				else
				{
					LastOutputNode->SetCableConnectFrom("X");
					triStateNode->SetEnableInput(LastOutputNode);
					LastOutputNode->AddNodeToConnectionSerializationArray(triStateNode->GetSerialNumber(), "Y");
				}
				ConnectedToHand = false;
			}
		}
	}
}

void ALogicGatesCharacter::ConnectToInputZ()
{

	// TODO: implement serialization for inputZ
	// We essentially just need to change the value of ConnectionSerializationMap to Z, and check for this during serialiation/deserialization
	if (ConnectedToHand)
	{
		// Just to check that they're valid.
		if (LastOutputNode && CurrentNode)
		{
			if (IsFromOutputY)
			{
				if (auto currentFullAdder = Cast<AFullAdder>(LastOutputNode))
				{
				
					if (auto lastFullAdder = Cast<AFullAdder>(CurrentNode))
					{
						currentFullAdder->SetCableConnectFrom("Y");
						// We don't give the buffer gate here, while we need reference to the full adder.
						lastFullAdder->SetInputCarry(currentFullAdder); 
						ConnectedToHand = false;
						IsFromOutputY = false;
					}
				}
			}
			else
			{
				if (auto lastFullAdder = Cast<AFullAdder>(CurrentNode))
				{
					LastOutputNode->SetCableConnectFrom("X");
					lastFullAdder->SetInputCarry(LastOutputNode);
					ConnectedToHand = false;
				}
			}
		}
	}
}