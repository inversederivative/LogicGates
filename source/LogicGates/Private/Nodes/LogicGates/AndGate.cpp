#include "Nodes/LogicGates/AndGate.h"

void AAndGate::SetInputX(AAbstractNode *input) {
    inputX = input;
    input->Attach(this);
    connectedNodes_.push_back(input);
    AAndGate::Update(GetState());
}

void AAndGate::SetInputY(AAbstractNode *input) {
    inputY = input;
    input->Attach(this);
    connectedNodes_.push_back(input);
    AAndGate::Update(GetState());
}

void AAndGate::Update(eLogicState state) {
    // Notify observers of the new output state
    outputState_ = state;
    Notify();
}

// Totally need to redo:

eLogicState AAndGate::GetState() const {

    eLogicState output;
    if (inputX && inputY)
    {
        if (inputX->GetState() == DISABLED || inputY->GetState() == DISABLED)
        {
            // Case 1 D,D
            if (inputX->GetState() == DISABLED && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
                return output;
            }
                // Case 2 D,F
            else if (inputX->GetState() == DISABLED && inputY->GetState() == OFF)
            {
                output = OFF;
                return output;
            }
                // Case 3 F,D
            else if (inputX->GetState() == OFF && inputY->GetState() == DISABLED)
            {
                output = OFF;
                return output;
            }
                // Case 4 D,T
            else if (inputX->GetState() == DISABLED && inputY->GetState() == ON)
            {
                output = DISABLED;
                return output;
            }
                // Case 5 T,D
            else if (inputX->GetState() == ON && inputY->GetState() == DISABLED)
            {
                output = DISABLED;
                return output;
            }
        }
            // Neither input was Disabled:
        else
        {
            // Case 6 F,F
            if (inputX->GetState() == OFF && inputY->GetState() == OFF)
            {
                output = OFF;
                return output;
            }
                // Case 7 F,T
            else if (inputX->GetState() == OFF && inputY->GetState() == ON)
            {
                output = OFF;
                return output;
            }
                // Case 8 T,F
            else if (inputX->GetState() == ON && inputY->GetState() == OFF)
            {
                output = OFF;
                return output;
            }
                // Case 9 T,T
            else if (inputX->GetState() == ON && inputY->GetState() == ON)
            {
                output = ON;
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