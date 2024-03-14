#pragma once

#include "CoreMinimal.h"
#include "LogicGates/Public/AbstractNode.h"
#include "AndGate.generated.h"

UCLASS()
class LOGICGATES_API AAndGate : public AAbstractNode
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void SetInputX(AAbstractNode* input);

	UFUNCTION(BlueprintCallable)
	void SetInputY(AAbstractNode* input);

	UFUNCTION(BlueprintCallable)
	void Update(eLogicState state) override;

	UFUNCTION(BlueprintCallable)
	eLogicState GetState() const override;

private:

	AAbstractNode* inputX;
	AAbstractNode* inputY;
	eLogicState outputState_ = DISABLED;
	std::list<IObserver*> connectedNodes_;
};
