// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/LogicGates/OrGate.h"

#include "LogicGates/LogicGatesCharacter.h"

AOrGate::AOrGate()
{
    SetNodeName("OrGate");
    
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	AndGateAsset(TEXT("StaticMesh'/Game/LogicGates/LogicGates/Mesh_OrGate'"));
	if (AndGateAsset.Succeeded())
	{
		DisplayMesh->SetStaticMesh(AndGateAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("****Failed to find Or Gate Asset."));
	}
}

// TODO: This class needs an overhaul bad!
eLogicState AOrGate::GetState() const {
    
    eLogicState output = DISABLED;
	
    if (inputX && inputY)
    {
        if (inputX->GetState() == DISABLED && inputY->GetState() == DISABLED)
        {
            output = DISABLED;
        }

        // If one or the other inputs are disabled
        if (inputX->GetState() == DISABLED || inputY->GetState() == DISABLED)
        {
            if (inputX->GetState() == ON && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
            }
            else if (inputX->GetState() == DISABLED && inputY->GetState() == ON)
            {
                output = DISABLED;
            }
            else if (inputX->GetState() == OFF && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
            }
            else if (inputX->GetState() == DISABLED && inputY->GetState() == OFF)
            {
                output = DISABLED;
            }
        }
        else {
            // Neither input contained DISABLED...
            if (inputX->GetState() == ON || inputY->GetState() == ON) {
                output = ON;
            } else if (inputX->GetState() == OFF && inputY->GetState() == OFF) {
                output = OFF;
            }
        }
    } else {
        std::cout<<"Both Inputs of OrGate weren't populated yet..." << std::endl;
        output = DISABLED;
    }
	return output;
}