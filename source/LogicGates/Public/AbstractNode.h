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

	AAbstractNode()
	{
		// We only want to increment the current ID
		SerialNumber = NextSerialNumber++;
	}

	

	int GetSerialNumber() const {
		return SerialNumber;
	}
	
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
		//observers_.push_back(observer);
		observersMap_.Add(Cast<AAbstractNode>(observer)->GetSerialNumber(), observer);
		
	}

	/**
	 * The Detach function is responsible for Removing the observers to
	 * the publishers. 
	 * @param observer 
	 */
	void Detach(IObserver *observer) override{
		//observers_.remove(observer);
		observersMap_.Remove(Cast<AAbstractNode>(observer)->GetSerialNumber());
	}

	/**
	 * The Notify function notifies all connected observers
	 * of the state change.
	 */
	void Notify() override {
		// for (auto observer : observers_) {
		// 	observer->Update(GetState());
		// }
		for (auto observer : observersMap_)
		{
			observer.Value->Update(GetState());
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
	
	// std::list<IObserver*> GetObservers()
	// {
	// 	return observers_;
	// }

	TMap<int, IObserver*> GetObserversMap()
	{
		return observersMap_;
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

	UFUNCTION(BlueprintCallable)
	void Placed()
	{
		// Results in an access violation. :(
		//GetNodeMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//NodeMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		std::cout << "Placed function called!" << std::endl;
	}

	UFUNCTION(BlueprintCallable)
	FString GetNodeName()
	{
		return NodeName;
	}

	UFUNCTION(BlueprintCallable)
	void SetNodeName(FString name)
	{
		NodeName = name;
	}

	UFUNCTION(BlueprintCallable)
	virtual FString SerializeNode()  PURE_VIRTUAL(AAbstractNode::SerializeNode, return "";);

	UFUNCTION(BlueprintCallable)
	virtual AAbstractNode* DeserializeNode(FString nodeJson)  PURE_VIRTUAL(AAbstractNode::DeserializeNode, return nullptr;);


	FString ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage)
	{
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
		{
			bOutSuccess = false;
			OutInfoMessage = FString::Printf(TEXT("Read String From File Failed - File doesn't exist - '%s'"), *FilePath);
			return "";
		}

		FString RetString = "";

		// Try to read the file. Output goes into RetString

		if (!FFileHelper::LoadFileToString(RetString, *FilePath))
		{
			bOutSuccess = false;
			OutInfoMessage = FString::Printf(TEXT("Read String From File Faild - Was not able to read file. Is this a text file? - '%s'"), *FilePath);
			return "";
		}

		bOutSuccess = true;
		OutInfoMessage = FString::Printf(TEXT("Read String From File Succeeded - '%s'"), *FilePath);
		return RetString;
	}

	void WriteSTringToFile(FString FilePath, FString String, bool& bOutSuccess, FString& OutInfoMessage)
	{
		if (!FFileHelper::SaveStringToFile(String, *FilePath))
		{
			bOutSuccess = false;
			OutInfoMessage = FString::Printf(TEXT("Write String To File Failed - Was not able to write to file. Is your file read only? Is the path valid? - '%s'"), *FilePath);
			return; // Not necessary?
		}

		bOutSuccess = true;
		OutInfoMessage = FString::Printf(TEXT("Write String To File Succeeded - '%s'"), *FilePath);
	}
	
protected:

	//std::list<IObserver*> observers_;
	TMap<int, IObserver*> observersMap_;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Serialization", meta = (AllowPrivateAccess = "true"))
	FString NodeName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Serialization", meta = (AllowPrivateAccess = "true"))
	int SerialNumber;
	
private:
	
	bool HasOutputNode = false;

	// TODO: Remove, not used.
	int NumOfConnectedOutputNodes = 0;
	
	eLogicState OutputState;

	

	static int NextSerialNumber;
};

int AAbstractNode::NextSerialNumber = 0;