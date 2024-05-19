// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CableComponent.h"

#include "Components/SphereComponent.h"
#include "LogicGates/Interfaces/NodeInterface.h"
#include "AbstractNode.generated.h"


USTRUCT(BlueprintType)
struct FKeyValuePair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyValue")
	int32 Key;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyValue")
	FString Value;
};

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
		observers_.push_back(observer);
		//observersMap_.Add(Cast<AAbstractNode>(observer)->GetSerialNumber(), observer);
		
	}

	/**
	 * The Detach function is responsible for Removing the observers to
	 * the publishers. 
	 * @param observer 
	 */
	void Detach(IObserver *observer) override{
		observers_.remove(observer);
		//observersMap_.Remove(Cast<AAbstractNode>(observer)->GetSerialNumber());
	}

	/**
	 * The Notify function notifies all connected observers
	 * of the state change.
	 */
	void Notify() override {
		// for (auto observer : observers_) {
		// 	observer->Update(GetState());
		// }
		for (auto observer : observers_) //observersMap_)
		{
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

	// TMap<int, IObserver*> GetObserversMap()
	// {
	//	return observersMap_;
	// }

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

	UFUNCTION(BlueprintCallable)
	bool GetHasOutputNode() const
	{
		return HasOutputNode;
	}

	void SetHasOutputNode(bool outputNode)
	{
		HasOutputNode = outputNode;
	}

	bool GetHasOutputY() const
	{
		return HasOutputY;
	}

	void SetHasOutputY(bool outputNode)
	{
		HasOutputY = outputNode;
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
	virtual FString SerializeNode() PURE_VIRTUAL(AAbstractNode::SerializeNode, return "";);
	// {
	// 	// Create a JSON array to store node entries
	// 	TArray<TSharedPtr<FJsonValue>> connectedNodesArray;
	// 	TArray<TSharedPtr<FJsonValue>> observersArray;
	// 	
	//
	// 	// Iterate through each item in the map
	// 	for (const auto& Pair : connectedNodesMap_)
	// 	{
	// 		// Create a JSON object for each node
	// 		TSharedPtr<FJsonObject> NodeObject = MakeShareable(new FJsonObject);
	// 		NodeObject->SetNumberField(TEXT("serialNumber"), Pair.Key);
	// 		NodeObject->SetStringField(TEXT("nodeName"), Cast<AAbstractNode>(Pair.Value)->GetNodeName());
	//
	// 		// Add the node object to the array
	// 		TSharedPtr<FJsonValueObject> NodeJsonValue = MakeShareable(new FJsonValueObject(NodeObject));
	// 		connectedNodesArray.Add(NodeJsonValue);
	// 	}
	//
	// 	for (const auto& Pair : observersMap_)
	// 	{
	// 		// Create a JSON object for each node
	// 		TSharedPtr<FJsonObject> NodeObject = MakeShareable(new FJsonObject);
	// 		NodeObject->SetNumberField(TEXT("serialNumber"), Pair.Key);
	// 		NodeObject->SetStringField(TEXT("nodeName"), Cast<AAbstractNode>(Pair.Value)->GetNodeName());
	//
	// 		// Add the node object to the array
	// 		TSharedPtr<FJsonValueObject> NodeJsonValue = MakeShareable(new FJsonValueObject(NodeObject));
	// 		observersArray.Add(NodeJsonValue);
	// 	}
	//
	// 	// Report Transform information
	// 	FTransform NodeTransform = GetActorTransform();
	// 	FVector NodePosition = NodeTransform. GetTranslation();
	// 	FRotator NodeRotation = NodeTransform.Rotator();
	// 	
	// 	// Create a JSON object to hold the array of nodes
	// 	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	// 	RootObject->SetStringField(TEXT("nodeName"), GetNodeName());
	// 	RootObject->SetNumberField(TEXT("serialNumber"), GetSerialNumber());
	//
	// 	if (auto twoInputNode = Cast<AAbstractTwoInputNode>(this))
	// 	{
	// 		if (twoInputNode->GetInputX())
	// 		{
	// 			RootObject->SetNumberField(TEXT("inputX"),
	// 			twoInputNode->GetInputX()->GetSerialNumber());
	// 		}
	// 		if (twoInputNode->GetInputY())
	// 		{
	// 			RootObject->SetNumberField(TEXT("inputY"),
	// 			twoInputNode->GetInputY()->GetSerialNumber());
	// 		}
	// 		
	// 		
	// 	}
	// 	
	// 	TSharedPtr<FJsonObject> PositionObject = MakeShareable(new FJsonObject);
	// 	PositionObject->SetNumberField(TEXT("x"), NodePosition.X);
	// 	PositionObject->SetNumberField(TEXT("y"), NodePosition.Y);
	// 	PositionObject->SetNumberField(TEXT("z"), NodePosition.Z);
	//
	// 	TSharedPtr<FJsonObject> RotationObject = MakeShareable(new FJsonObject);
	// 	RotationObject->SetNumberField(TEXT("pitch"), NodeRotation.Pitch);
	// 	RotationObject->SetNumberField(TEXT("yaw"), NodeRotation.Yaw);
	// 	RotationObject->SetNumberField(TEXT("roll"), NodeRotation.Roll);
	//
	// 	RootObject->SetObjectField(TEXT("position"), PositionObject);
	// 	RootObject->SetObjectField(TEXT("rotation"), RotationObject);
	// 	RootObject->SetArrayField(TEXT("connectedNodes"), connectedNodesArray);
	// 	RootObject->SetArrayField(TEXT("observers"), observersArray);
	// 	
	// 	// Create a writer and write JSON to string
	// 	FString OutputString;
	// 	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&OutputString);
	// 	FJsonSerializer::Serialize(RootObject.ToSharedRef(), JsonWriter);
	//
	// 	return OutputString;
	// }

	UFUNCTION(BlueprintCallable)
	virtual AAbstractNode* DeserializeNode(FString nodeJson)  PURE_VIRTUAL(AAbstractNode::DeserializeNode, return nullptr;);
	
	UFUNCTION(BlueprintCallable)
	int GetCableConnectNumber() const
	{
		return CableConnectNumber;
	}
	
	void SetCableConnectNumber(int number)
	{
		CableConnectNumber = number;
	}

	UFUNCTION(BlueprintCallable)
	FString GetCableConnectString() const
	{
		return CableConnectString;
	}
	
	void SetCableConnectString(FString cableConnect)
	{
		CableConnectString = cableConnect;
	}

	UFUNCTION(BlueprintCallable)
	FString GetCableConnectFrom() const
	{
		return CableConnectFrom;
	}

	void SetCableConnectFrom(FString xOrY)
	{
		CableConnectFrom = xOrY;
	}

	UFUNCTION(BlueprintCallable)
	int GetDeserializationNumber() const
	{
		return DeserializationNumber;
	}

	UFUNCTION(BlueprintCallable)
	void SetDeserializationNumber(int number) 
	{
		DeserializationNumber = number;
	}

	UFUNCTION(BlueprintCallable)
	TArray<FKeyValuePair> GetConnectionSerializationArray() const
	{
		return ConnectionSerializationArray;
	}

	UFUNCTION(BlueprintCallable)
	void SetConnectionSerializationArray(TArray<FKeyValuePair> connectionsArray)
	{
		ConnectionSerializationArray = connectionsArray;
	}

	UFUNCTION(BlueprintCallable)
	void AddNodeToConnectionSerializationArray(int indexKey, FString xOrY)
	{
		FKeyValuePair pair;
		pair.Key = indexKey;
		pair.Value = xOrY;
		ConnectionSerializationArray.Add(pair);
	}

	UFUNCTION(BlueprintCallable)
	void AddNodeToConnectionSerializationArrayKV(FKeyValuePair kvPair)
	{
		ConnectionSerializationArray.Add(kvPair);
	}
	
	FString GetToInputXorY() const
	{
		return ToInputXorY;
	}

	void SetToInputXorY(FString xOrY)
	{
		ToInputXorY = xOrY;
	}
	
protected:

	std::list<IObserver*> observers_;
	std::list<IObserver*> connectedNodes_;

	//TMap<int, IObserver*> observersMap_;
	//TMap<int, IObserver*> connectedNodesMap_;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Serialization", meta = (AllowPrivateAccess = "true"))
	int DeserializationNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Serialization", meta = (AllowPrivateAccess = "true"))
	TArray<FKeyValuePair> ConnectionSerializationArray;
	
private:
	
	bool HasOutputNode = false;

	bool HasOutputY = false;

	// TODO: Remove, not used.
	int NumOfConnectedOutputNodes = 0;
	
	eLogicState OutputState;

	int CableConnectNumber;

	FString CableConnectString;

	FString CableConnectFrom;

	FString ToInputXorY;

	//TArray<TPair<int, FString>> ConnectionSerializationArray;
	
	static int NextSerialNumber;
};

int AAbstractNode::NextSerialNumber = 0;