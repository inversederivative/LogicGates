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
#include "Nodes/LogicGates/AbstractTwoInputNode.h"
#include "Nodes/LogicGates/AndGate.h"

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

		// Connect output to inputs -- Note this is a WORK IN PROGRESS
		EnhancedInputComponent->BindAction(ConnectFromOutputXAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectFromOutputX);
		EnhancedInputComponent->BindAction(ConnectToInputXAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectToInputX);
		EnhancedInputComponent->BindAction(ConnectToInputYAction, ETriggerEvent::Started, this, &ALogicGatesCharacter::ConnectToInputY);

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
	if (auto powerSource = Cast<APowerSource>(CurrentNode))
	{
		if (powerSource != LastOutputNode)
		{
			// We only reset the pointer, when we 
			LastOutputNode = powerSource;
			powerSource->GetOutputCableX()->SetAttachEndTo(this, "CharacterMesh0", "Left_Hand_Socket");
			ConnectedToHand = true;
		}
	}

	// Handle for TwoInput
	if (auto twoInputNode = Cast<AAbstractTwoInputNode>(CurrentNode))
	{
		if (twoInputNode != LastOutputNode)
		{
			LastOutputNode = twoInputNode;
			twoInputNode->GetOutputCableX()->SetAttachEndTo(this, "CharacterMesh0", "Left_Hand_Socket");
			ConnectedToHand = true;
		}
	}
}


void ALogicGatesCharacter::ConnectToInputX()
{
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

	if (ConnectedToHand)
	{
		if (LastOutputNode)
		{
			if (auto display = Cast<ADisplayOutput>(CurrentNode))
			{
			
				display->SetInput(LastOutputNode);
				LastOutputNode->GetOutputCableX()->SetAttachEndTo(display,"InputPortX");
				ConnectedToHand = false;
			
				// NOTE: Should only reset power source, when we connect to a new one.
				// NOTE NOTE: We shouldn't treat every output as a power source. Could also be an and gate.
				// else
				// {
				// 	LastPowerSource = nullptr;
				// }
			}

			if (auto twoInputNode = Cast<AAbstractTwoInputNode>(CurrentNode))
			{
				
				// We only connect, if we have a valid output cable to connect from.
				twoInputNode->SetInputX(LastOutputNode);
				// We attach the end to the Connection Port (X) of the Two Input Node
				// TODO: Get proper name for input node to attach to...
				// Should be good?


				// TO MY UTTER SURPRISE, THIS WORKED!!!!!!!
				LastOutputNode->GetOutputCableX()->SetAttachEndTo(twoInputNode, "InputPortX");
				ConnectedToHand = false;
				
				
				//if (auto andGate = Cast<AAndGate>(twoInputNode))
				// {
				// 	// Handle And gate
				// 	andGate->SetInputX(LastOutputNode);
				// 	LastOutputNode->GetOutputCableX()->SetAttachEndTo(andGate, "AndGate Component");
				// 	
				// }
				// Handle Or Gate?
				// if (auto orGate = Cast<AOrGate>(twoInputNode)
				// This shouldn't be necessary. This is the whole point of the TwoInput abstract class.
				// We should just call the abstract method SetInputX, and it resolves depending on if
				// it is actually an AndGate or an OrGate
			}
		}
	}
	
	
}


void ALogicGatesCharacter::ConnectToInputY()
{
	if (ConnectedToHand)
	{
		if (LastOutputNode)
		{
			if (auto twoInputNode = Cast<AAbstractTwoInputNode>(CurrentNode))
			{
				
				// We only connect, if we have a valid output cable to connect from.
				twoInputNode->SetInputY(LastOutputNode);
				LastOutputNode->GetOutputCableX()->SetAttachEndTo(twoInputNode, "InputPortY");
				
			}
		}
	}
	
	
}