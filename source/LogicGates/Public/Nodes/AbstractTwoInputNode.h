// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "AbstractTwoInputNode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class LOGICGATES_API AAbstractTwoInputNode : public AAbstractNode
{
	GENERATED_BODY()

public:

	AAbstractTwoInputNode();
	
	void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void SetInputX(AAbstractNode* input);
	
	UFUNCTION(BlueprintCallable)
	void SetInputY(AAbstractNode* input);

	UFUNCTION(BlueprintCallable)
	void RemoveInputX();

	UFUNCTION(BlueprintCallable)
	void RemoveInputY();
	
	UFUNCTION(BlueprintCallable)
	void Update(eLogicState state) override;

	UFUNCTION(BlueprintCallable)
	AAbstractNode* GetInputX()
	{
		return inputX;
	}
	
	UFUNCTION(BlueprintCallable)
	AAbstractNode* GetInputY()
	{
		return inputY;
	}
	
	bool GetIsNodeForOtherNodes() const
	{
		return IsNodeForOtherNodes;
	}

	void SetIsNodeForOtherNodes(bool answer)
	{
		IsNodeForOtherNodes = answer;
	}

	FString SerializeNode() override;

	AAbstractNode* DeserializeNode(FString nodeJson) override;
	
protected:

	UFUNCTION()
	void SetupMeshes();

	UFUNCTION()
	void SetupMaterials();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* NodeSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* StandMaterial;
	
	// Connection Ports
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPortX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPortY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* OutputPortX;
	
	// TODO: find a way to make these members private not protected?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* inputX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* inputY;
	
	eLogicState outputState_;
	//std::list<IObserver*> connectedNodes_;
	TMap<int, IObserver*> connectedNodesMap_;

	bool IsNodeForOtherNodes;
};
