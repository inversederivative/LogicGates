#pragma once

#include "CoreMinimal.h"
#include "Nodes/AbstractTwoInputNode.h"

#include "AndGate.generated.h"

UCLASS()
class LOGICGATES_API AAndGate : public AAbstractTwoInputNode
{
	GENERATED_BODY()

public:

	AAndGate();

	eLogicState GetState() const override;
};
