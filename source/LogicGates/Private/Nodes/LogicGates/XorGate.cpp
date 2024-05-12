// Fill out your copyright notice in the Description page of Project Settings.


#include "Nodes/LogicGates/XorGate.h"

AXorGate::AXorGate()
{
    SetNodeName("XorGate");
    
	// Set Mesh for AndGate
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	AndGateAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_XorGate'"));
	if (AndGateAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(AndGateAsset.Object);
		UE_LOG(LogTemp, Display, TEXT("****Successfully assigned mesh asset to XorGate!."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find XorGate Asset."));
	}
}

eLogicState AXorGate::GetState() const
{
	eLogicState output = DISABLED;
    
    if (inputX && inputY)
    {
        if (inputX->GetState() == DISABLED || inputY->GetState() == DISABLED)
        {
            // Case 1 D,D
            if (inputX->GetState() == DISABLED && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
            }
                // Case 2 D,F
            else if (inputX->GetState() == DISABLED && inputY->GetState() == OFF)
            {
                output = DISABLED;
            }
                // Case 3 F,D
            else if (inputX->GetState() == OFF && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
            }
                // Case 4 D,T
            else if (inputX->GetState() == DISABLED && inputY->GetState() == ON)
            {
                output = DISABLED;
            }
                // Case 5 T,D
            else if (inputX->GetState() == ON && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
            }
        }
            // Neither input was Disabled:
        else
        {
            // Case 6 F,F
            if (inputX->GetState() == OFF && inputY->GetState() == OFF)
            {
                output = OFF;
            }
                // Case 7 F,T
            else if (inputX->GetState() == OFF && inputY->GetState() == ON)
            {
                output = ON;
            }
                // Case 8 T,F
            else if (inputX->GetState() == ON && inputY->GetState() == OFF)
            {
                output = ON;
            }
                // Case 9 T,T
            else if (inputX->GetState() == ON && inputY->GetState() == ON)
            {
                output = OFF;
            }
        }
    }
    else {
        std::cout<< "Both Inputs of XorGate weren't populated yet..." << std::endl;
        output = DISABLED;
    }
	return output;
}
