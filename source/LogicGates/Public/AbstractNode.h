// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"

#include "Components/SphereComponent.h"
#include "LogicGates/Interfaces/NodeInterface.h"
#include "AbstractNode.generated.h"

UCLASS(Blueprintable)
class LOGICGATES_API AAbstractNode : public AActor, public ISubject, public IObserver
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	//AAbstractNode();
	UFUNCTION(BlueprintCallable)
	virtual eLogicState GetState() const PURE_VIRTUAL(AAbstractNode::GetState, return DISABLED;);

	// IObserver interface methods
	virtual void Update(eLogicState state) override PURE_VIRTUAL(AAbstractNode::Update,);

	/**
	 * The Attach function is responsible for adding the observers to
	 * the publishers. A PowerSource connects to an AndGate, and the
	 * AndGate is attached to the PowerSource as an observer.
	 * @param observer 
	 */
	void Attach(IObserver *observer) override {
		observers_.push_back(observer);
	}

	/**
	 * The Detach function is responsible for Removing the observers to
	 * the publishers. 
	 * @param observer 
	 */
	void Detach(IObserver *observer) override{
		observers_.remove(observer);
	}

	/**
	 * The Notify function notifies all connected observers
	 * of the state change.
	 */
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

	virtual void SetOutputCableX(UCableComponent* cable)
	{
		OutputCableX = cable;
	}

	virtual void SetOutputCableY(UCableComponent* cable)
	{
		OutputCableY = cable;
	}
	
	std::list<IObserver*> GetObservers()
	{
		return observers_;
	}

	int GetNumOfConnectedOutputNodes() const
	{
		return NumOfConnectedOutputNodes;
	}

	void IncrementAttachedOutputs()
	{
		NumOfConnectedOutputNodes++;
	}

	void DecrementAttachedOutputs()
	{
		NumOfConnectedOutputNodes--;
	}

	bool GetHasOutputNode() const
	{
		return HasOutputNode;
	}

	void SetHasOutputNode(bool outputNode)
	{
		HasOutputNode = outputNode;
	}

	UStaticMeshComponent* GetNodeMesh() const
	{
		return NodeMesh;
	}

	void SetNodeMesh(UStaticMeshComponent* nodeMesh)
	{
		NodeMesh = nodeMesh;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* CableConnector;
	
	virtual void SetVisibility(bool visibility) const PURE_VIRTUAL(AAbstractNode::SetVisibility,);
	
	
protected:

	std::list<IObserver*> observers_;

	// TODO: Make these private, and create abstract Getters and Setters
	// TODO: Find a way to make them private? I set them in a child class, so maybe they need to be protected.

	UPROPERTY
	(VisibleAnywhere, BlueprintReadOnly, Category = "Node Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* NodeMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cable Components", meta = (AllowPrivateAccess = "true"))
	UCableComponent* OutputCableX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cable Components", meta = (AllowPrivateAccess = "true"))
	UCableComponent* OutputCableY;
	
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(AAbstractNode::OnOverlapBegin,);

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) PURE_VIRTUAL(AAbstractNode::OnOverlapEnd,);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DisabledMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OffMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OnMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;
	
private:
	
	bool HasOutputNode = false;	
	
	int NumOfConnectedOutputNodes = 0;
	
	eLogicState OutputState;
};
