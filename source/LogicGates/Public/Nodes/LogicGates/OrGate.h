// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/AbstractTwoInputNode.h"
#include "OrGate.generated.h"

/**
 * 
 */
UCLASS()
class LOGICGATES_API AOrGate : public AAbstractTwoInputNode
{
	GENERATED_BODY()

public:
	AOrGate();
	
	eLogicState GetState() const override;
};
