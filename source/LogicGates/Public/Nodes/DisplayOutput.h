// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "GameFramework/Actor.h"
#include "DisplayOutput.generated.h"

class UStaticMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOGICGATES_API ADisplayOutput : public AAbstractNode
{
	GENERATED_BODY()
	
public:	

	UFUNCTION(BlueprintCallable, Category = "Power")
	void SetInput(AAbstractNode* input);

	eLogicState GetState() const override;

	void Update(eLogicState state) override;
	
private:
	std::list<IObserver*> connectedNodes_;
	AAbstractNode* input_;
	eLogicState state_ = DISABLED;
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
	bool bIsLit;

	

};
