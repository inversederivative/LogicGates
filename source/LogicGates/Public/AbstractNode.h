// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LogicGates/Interfaces/NodeInterface.h"
#include "AbstractNode.generated.h"

UCLASS(Blueprintable)
class LOGICGATES_API AAbstractNode : public AActor, public ISubject, public IObserver
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	//AAbstractNode();

	virtual eLogicState GetState() const PURE_VIRTUAL(AAbstractNode::GetState, return DISABLED;);

	// IObserver interface methods
	virtual void Update(eLogicState state) override PURE_VIRTUAL(AAbstractNode::Update,);

	void Attach(IObserver *observer) override {
		observers_.push_back(observer);
	}
	void Detach(IObserver *observer) override{
		observers_.remove(observer);
	}
	void Notify() override {
		for (auto observer : observers_) {
			observer->Update(GetState());
		}
	}

	std::list<IObserver*> GetObservers()
	{
		return observers_;
	}

protected:

	std::list<IObserver*> observers_;

private:
	
	UPROPERTY
	(VisibleAnywhere, BlueprintReadOnly, Category = "Node Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* NodeMesh;
	
	eLogicState OutputState;
};
