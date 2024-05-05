#include "Nodes/LogicGates/AndGate.h"



AAndGate::AAndGate()
{
	LogicGateSceneComponent = CreateDefaultSubobject<USceneComponent>("AndGate SceneComponent");

	// Set Mesh for AndGate
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	AndGateAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_AndGate'"));
	if (AndGateAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(AndGateAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned cube mesh asset to Cube!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Power Source Asset."));
	}
}

// TODO: Totally need to redo:

eLogicState AAndGate::GetState() const {

    eLogicState output;
	
    if (inputX && inputY)
    {
        if (inputX->GetState() == DISABLED || inputY->GetState() == DISABLED)
        {
            // Case 1 D,D
            if (inputX->GetState() == DISABLED && inputY->GetState() == DISABLED)
            {
            	InputPortX->SetMaterial(0, DisabledMaterial);
            	InputPortY->SetMaterial(0, DisabledMaterial);
                output = DISABLED;
            	OutputPortX->SetMaterial(0, DisabledMaterial);
                return output;
            }
                // Case 2 D,F
            else if (inputX->GetState() == DISABLED && inputY->GetState() == OFF)
            {
            	InputPortX->SetMaterial(0, DisabledMaterial);
            	InputPortY->SetMaterial(0, OffMaterial);
                output = OFF;
            	OutputPortX->SetMaterial(0, OffMaterial);
                return output;
            }
                // Case 3 F,D
            else if (inputX->GetState() == OFF && inputY->GetState() == DISABLED)
            {
            	InputPortX->SetMaterial(0, OffMaterial);
            	InputPortY->SetMaterial(0, DisabledMaterial);
                output = OFF;
            	OutputPortX->SetMaterial(0, OffMaterial);
                return output;
            }
                // Case 4 D,T
            else if (inputX->GetState() == DISABLED && inputY->GetState() == ON)
            {
            	InputPortX->SetMaterial(0, DisabledMaterial);
            	InputPortY->SetMaterial(0, OnMaterial);
                output = DISABLED;
            	OutputPortX->SetMaterial(0, DisabledMaterial);
                return output;
            }
                // Case 5 T,D
            else if (inputX->GetState() == ON && inputY->GetState() == DISABLED)
            {
            	InputPortX->SetMaterial(0, OnMaterial);
            	InputPortY->SetMaterial(0, DisabledMaterial);
                output = DISABLED;
            	OutputPortX->SetMaterial(0, DisabledMaterial);
                return output;
            }
        }
            // Neither input was Disabled:
        else
        {
            // Case 6 F,F
            if (inputX->GetState() == OFF && inputY->GetState() == OFF)
            {
            	InputPortX->SetMaterial(0, OffMaterial);
            	InputPortY->SetMaterial(0, OffMaterial);
                output = OFF;
            	OutputPortX->SetMaterial(0, OffMaterial);
                return output;
            }
                // Case 7 F,T
            else if (inputX->GetState() == OFF && inputY->GetState() == ON)
            {
            	InputPortX->SetMaterial(0, OffMaterial);
            	InputPortY->SetMaterial(0, OnMaterial);
                output = OFF;
            	OutputPortX->SetMaterial(0, OffMaterial);
                return output;
            }
                // Case 8 T,F
            else if (inputX->GetState() == ON && inputY->GetState() == OFF)
            {
            	InputPortX->SetMaterial(0, OnMaterial);
            	InputPortY->SetMaterial(0, OffMaterial);
                output = OFF;
            	OutputPortX->SetMaterial(0, OffMaterial);
                return output;
            }
                // Case 9 T,T
            else if (inputX->GetState() == ON && inputY->GetState() == ON)
            {
            	InputPortX->SetMaterial(0, OnMaterial);
            	InputPortY->SetMaterial(0, OnMaterial);
                output = ON;
            	OutputPortX->SetMaterial(0, OnMaterial);
                return output;
            }
        }
    }
    else {
        // For debugging from original LogicGatesAndEvents project. Not necessary for Release Build
        // TODO: Remove for release build
        std::cout<< "Both Inputs of AndGate weren't populated yet..." << std::endl;
        return DISABLED;
    }
    return DISABLED;
}


