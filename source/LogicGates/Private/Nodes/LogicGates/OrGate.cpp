// Fill out your copyright notice in the Description page of Project Settings.

#include "Nodes/LogicGates/OrGate.h"

#include "LogicGates/LogicGatesCharacter.h"

AOrGate::AOrGate()
{
	LogicGateSceneComponent = CreateDefaultSubobject<USceneComponent>("OrGate SceneComponent");

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
    // Compute the state of the OR gate based on its inputs
    // Note, we're checking that both inputs are NOT null!
//    if (inputX && inputY) {
//        // Here is where we check if the or condition is true or not.
//        return inputX->GetState() || inputY->GetState();
//    } else {
//        // Handle case where inputs are not set
//        return false; // Or some default value depending on your requirements
//    }

   
    eLogicState output = DISABLED;
	// If Both inputs are Disabled
    if (inputX && inputY)
    {
        if (inputX->GetState() == DISABLED && inputY->GetState() == DISABLED)
        {
        	InputPortX->SetMaterial(0, DisabledMaterial);
        	InputPortY->SetMaterial(0, DisabledMaterial);
            output = DISABLED;
        	OutputPortX->SetMaterial(0, DisabledMaterial);
        }

        // If one or the other inputs are disabled

        if (inputX->GetState() == DISABLED || inputY->GetState() == DISABLED)
        {
            if (inputX->GetState() == ON && inputY->GetState() == DISABLED)
            {
            	InputPortX->SetMaterial(0, OnMaterial);
            	InputPortY->SetMaterial(0, DisabledMaterial);
                output = ON;
            	OutputPortX->SetMaterial(0, OnMaterial);
            }
            else if (inputX->GetState() == DISABLED && inputY->GetState() == ON)
            {
            	InputPortX->SetMaterial(0, DisabledMaterial);
            	InputPortY->SetMaterial(0, OnMaterial);
                output = ON;
            	OutputPortX->SetMaterial(0, OnMaterial);
                
            }
            else if (inputX->GetState() == OFF && inputY->GetState() == DISABLED)
            {
            	InputPortX->SetMaterial(0, OffMaterial);
            	InputPortY->SetMaterial(0, DisabledMaterial);
                output = DISABLED;
            	OutputPortX->SetMaterial(0, DisabledMaterial);
            }
            else if (inputX->GetState() == DISABLED && inputY->GetState() == OFF)
            {
            	InputPortX->SetMaterial(0, DisabledMaterial);
            	InputPortY->SetMaterial(0, OffMaterial);
                output = DISABLED;
            	OutputPortX->SetMaterial(0, DisabledMaterial);
            }
        }
        else {
            // Neither input contained DISABLED...
			// TODO: we need to check what the actual state is, not just or
            if (inputX->GetState() == ON || inputY->GetState() == ON) {
            	if (inputX->GetState() == ON)
            	{
            		InputPortX->SetMaterial(0, OnMaterial);
            	}
            	else
            	{
            		InputPortX->SetMaterial(0, OffMaterial);
            	}
            	if (inputY->GetState() == ON)
            	{
            		InputPortY->SetMaterial(0, OnMaterial);
            	}
            	else
            	{
            		InputPortY->SetMaterial(0, OffMaterial);
            	}
            	
            	InputPortY->SetMaterial(0, OnMaterial);
                output = ON;
            	OutputPortX->SetMaterial(0, OnMaterial);
            } else if (inputX->GetState() == OFF && inputY->GetState() == OFF) {
            	InputPortX->SetMaterial(0, OffMaterial);
            	InputPortY->SetMaterial(0, OffMaterial);
                output = OFF;
            	OutputPortX->SetMaterial(0, OffMaterial);
            }
        }
    } else {
        std::cout<<"Both Inputs of OrGate weren't populated yet..." << std::endl;
        output = DISABLED;
    }
	return output;
}
