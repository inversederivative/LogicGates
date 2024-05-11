// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/AbstractTwoInputNode.h"
#include "XorGate.generated.h"

/**
 * 
 */
UCLASS()
class LOGICGATES_API AXorGate : public AAbstractTwoInputNode
{
	GENERATED_BODY()

public: 
	
	AXorGate();
	
	eLogicState GetState() const override;
};
