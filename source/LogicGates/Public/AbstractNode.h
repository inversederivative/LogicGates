// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"

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

	// Getters and Setters for Output X and Y

	virtual UCableComponent* GetOutputCableX() const
	{
		return OutputCableX;
	}
	
	virtual UCableComponent* GetOutputCableY() const
	{
		return OutputCableY;
	}
	
	std::list<IObserver*> GetObservers()
	{
		return observers_;
	}

protected:

	std::list<IObserver*> observers_;

	// TODO: Make these private, and create abstract Getters and Setters
	// TODO: Find a way to make them private? I set them in a child class, so maybe they need to be protected.
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cable Components", meta = (AllowPrivateAccess = "true"))
	UCableComponent* OutputCableX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cable Components", meta = (AllowPrivateAccess = "true"))
	UCableComponent* OutputCableY;
	
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(AAbstractNode::OnOverlapBegin,);

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) PURE_VIRTUAL(AAbstractNode::OnOverlapEnd,);
	

private:
	
	UPROPERTY
	(VisibleAnywhere, BlueprintReadOnly, Category = "Node Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* NodeMesh;



	
	eLogicState OutputState;
};
