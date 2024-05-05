#pragma once

#include "CoreMinimal.h"
#include "AbstractTwoInputNode.h"

#include "AndGate.generated.h"

UCLASS()
class LOGICGATES_API AAndGate : public AAbstractTwoInputNode
{
	GENERATED_BODY()

public:

	AAndGate();

	UFUNCTION(BlueprintCallable)
	eLogicState GetState() const override;
};
