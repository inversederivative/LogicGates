// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/PowerSource.h"

#include "NavigationSystemTypes.h"
#include "LogicGates/LogicGatesCharacter.h"

APowerSource::APowerSource()
{
	PowerSourceSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PowerSource SceneComponent"));
	RootComponent = PowerSourceSceneComponent;

	
	
	PowerSourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerSourceMesh"));
	PowerSourceMesh->SetupAttachment(RootComponent);

	// Create and attach the button mesh
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(RootComponent);

	// Create and attach the connection port mesh
	ConnectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConnectionPortMesh"));
	ConnectionMesh->SetupAttachment(RootComponent);
	CableConnector = CreateDefaultSubobject<USceneComponent>("CableConnector");
	CableConnector->SetupAttachment(ConnectionMesh);

	OutputCableX = CreateDefaultSubobject<UCableComponent>(TEXT("PowerSource Output"));
	// Attach To Component comes from the USceneComponent class. 
	OutputCableX->AttachToComponent(CableConnector, FAttachmentTransformRules::KeepWorldTransform);
	// Setup Cable's Initial End Location
	// NOTE: We have to add the space, because the component automatically has a space in the engine.
	//      May as well name with spaces as well, but that's for later.
	OutputCableX->SetAttachEndTo(this, "Cable Connector");
	//OutputCableX->SetAttachEndToComponent(CableConnector); // Attaches cable end to self.

	// Assign meshes for Switch, Button, and Connector
	// Then setup the three materials for Disabled (Black), Off (Red), and On (Green)
	SetupMeshes();	
	SetupMaterials();

	// Set default material for the mesh
	if (DisabledMaterial)
	{
		ConnectionMesh->SetMaterial(0, DisabledMaterial);
	}
	if (OffMaterial)
	{
		ButtonMesh->SetMaterial(0, OffMaterial);
	}
	if (StandMaterial)
	{
		
		PowerSourceMesh->SetMaterial(0, StandMaterial);
	}
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(100.0f); // Set the radius of the collision sphere
	CollisionSphere->SetupAttachment(RootComponent);
	
	state_ = DISABLED;
	InteractionDistance = 100.0f; // Set the interaction distance
}

// Sets default values
// Do I even use this constructor anywhere?
APowerSource::APowerSource(eLogicState state)
{
	APowerSource();
	ChangeState(state);
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
		ConnectionMesh->SetMaterial(0, DisabledMaterial);
		AnimateButtonPosition();
	}
	if (state == OFF)
	{
		
		UE_LOG(LogTemp, Display, TEXT("****The state was changed. The current state is: Off"));
		ConnectionMesh->SetMaterial(0, OffMaterial);
		AnimateButtonPosition();
	}
	if (state == ON)
	{
		UE_LOG(LogTemp, Display, TEXT("****The state was changed. The current state is: On"));
		ConnectionMesh->SetMaterial(0, OnMaterial);
		AnimateButtonPosition();
	}
	Notify();
}

// Perhaps it'd be better simply have a priority. On, Off, and Disabled.
// Then if we're in Disabled, we go to on, if we're off we go to disabled, etc. In end effect
// this is what we already have.
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

void APowerSource::SetupMeshes()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	PowerSourceAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_PowerSource'"));
	if (PowerSourceAsset.Succeeded())
	{
		PowerSourceMesh->SetStaticMesh(PowerSourceAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Power Source Asset."));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ButtonAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_Button'"));	
	if (ButtonAsset.Succeeded())
	{
		ButtonMesh->SetStaticMesh(ButtonAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	ConnectionAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_Connection'"));
	if (ConnectionAsset.Succeeded())
	{
		ConnectionMesh->SetStaticMesh(ConnectionAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Button Mesh Asset."));
	}
}

void APowerSource::SetupMaterials()
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

// Function to animate the position of the button mesh
void APowerSource::AnimateButtonPosition()
{
    // Define animation parameters
	// Duration of animation in seconds
    FVector InitialPosition = ButtonMesh->GetRelativeLocation();
    FVector TargetPosition = InitialPosition + FVector(10.0f, 0.0f, 0.0f); // Increment X by 10 units

    // Start the animation
    GetWorldTimerManager().SetTimer(TimerHandle, [this, InitialPosition, TargetPosition]()
    {
        float ElapsedTime = 0.0f;

        // Update button position smoothly over time
        while (ElapsedTime < AnimationDuration)
        {
            // Interpolate between initial and target positions
            FVector NewPosition = FMath::Lerp(InitialPosition, TargetPosition, ElapsedTime / AnimationDuration);
            ButtonMesh->SetRelativeLocation(NewPosition);

            // Update elapsed time
            ElapsedTime += GetWorld()->GetDeltaSeconds();
        }

        // Ensure the button reaches the target position
        ButtonMesh->SetRelativeLocation(TargetPosition);

        // Reverse animation by decrementing X by 10 units
        GetWorldTimerManager().SetTimer(TimerHandle, [this, TargetPosition]()
        {
            float ElapsedTime = 0.0f;
            FVector InitialPosition = TargetPosition;
            FVector ReverseTargetPosition = InitialPosition - FVector(10.0f, 0.0f, 0.0f); // Decrement X by 10 units

            // Update button position smoothly over time
            while (ElapsedTime < AnimationDuration)
            {
                // Interpolate between initial and reverse target positions
                FVector NewPosition = FMath::Lerp(InitialPosition, ReverseTargetPosition, ElapsedTime / AnimationDuration);
                ButtonMesh->SetRelativeLocation(NewPosition);

                // Update elapsed time
                ElapsedTime += GetWorld()->GetDeltaSeconds();
            }

            // Ensure the button reaches the reverse target position
            ButtonMesh->SetRelativeLocation(ReverseTargetPosition);
        }, AnimationDuration, false);
    }, 0.0f, false);
}

UCableComponent* APowerSource::GetOutputCableX() const
{
	return OutputCableX;
}
