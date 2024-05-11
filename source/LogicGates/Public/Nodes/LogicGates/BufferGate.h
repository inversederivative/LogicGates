// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "BufferGate.generated.h"

/**
 * 
 */
UCLASS()
class LOGICGATES_API ABufferGate : public AAbstractNode
{
	GENERATED_BODY()

public:
	void SetInput(AAbstractNode* in);

	void Update(eLogicState state) override;

	eLogicState GetState() const override;


private:

	AAbstractNode* input;

	eLogicState outputState_ = DISABLED;
	std::list<IObserver*> connectedNodes_;
	
};
