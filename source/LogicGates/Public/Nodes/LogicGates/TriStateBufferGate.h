// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "TriStateBufferGate.generated.h"

/**
 * 
 */
UCLASS()
class LOGICGATES_API ATriStateBufferGate : public AAbstractNode
{
	GENERATED_BODY()

	AAbstractNode* dataInput;
	AAbstractNode* enableInput;
	eLogicState outputState_ = DISABLED;
	//std::list<IObserver*> connectedNodes_;
	//TMap<int, IObserver*> connectedNodesMap_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TriStateBufferGateRoot;

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
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* StandMaterial;

	// Connection Ports
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPortData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPortEnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* OutputPortX;
	
public:

	ATriStateBufferGate();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetDataInput(AAbstractNode* input);

	UFUNCTION(BlueprintCallable)
	void SetEnableInput(AAbstractNode* input);

	UFUNCTION(BlueprintCallable)
	void Update(eLogicState state) override;

	//UFUNCTION(BlueprintCallable) parent class already has this attribute
	eLogicState GetState() const override;
	
};
