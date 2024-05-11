// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "Nodes/LogicGates/XorGate.h"
#include "Nodes/LogicGates/AndGate.h"
#include "Nodes/LogicGates/OrGate.h"
#include "Nodes/LogicGates/BufferGate.h"
#include "Nodes/PowerSource.h"
#include "FullAdder.generated.h"

/**
 * 
 */
UCLASS()
class LOGICGATES_API AFullAdder : public AAbstractNode
{
	GENERATED_BODY()

public:

	AFullAdder();

	void BeginPlay() override;

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
	
	UFUNCTION(BlueprintCallable)
	void SetInputX(AAbstractNode* input);
	
	UFUNCTION(BlueprintCallable)
	void SetInputY(AAbstractNode* input);

	UFUNCTION(BlueprintCallable)
	void SetInputCarry(AAbstractNode* input);
	
	UFUNCTION(BlueprintCallable)
	AAbstractNode* GetCarryOutNode() const;
	
	UFUNCTION(BlueprintCallable)
	void RemoveInputX();

	UFUNCTION(BlueprintCallable)
	void RemoveInputY();

	UFUNCTION(BlueprintCallable)
	void RemoveInputCarry();
	
	// TODO: This may not have the same behavior for all two in two out classes, so we may need
	// TODO: to reimplement this differently, in different classes.

	// NOTE: Seems to be working okay. 
	UFUNCTION(BlueprintCallable)
	void Update(eLogicState state) override;

	eLogicState GetCarryOut() const;

	eLogicState GetState() const;

	bool GetFromOutputY() const
	{
		return fromOutputY;
	}
	void SetFromOutputY(bool outputY)
	{
		fromOutputY = outputY;
	}
	
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
	USceneComponent* AdderSceneComponent;

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
	UStaticMeshComponent* InputPortZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* OutputPortX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* OutputPortCarry;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* CableConnectorCarry;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AXorGate* xorGateA;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAndGate* andGateA;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AXorGate* xorGateB;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAndGate* andGateB;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AOrGate* orGate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	ABufferGate* carryOutNode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	APowerSource* defaultCarry;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* inputX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* inputY;
	/**
	 * Carry Bit
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* inputCarry; 

	std::list<IObserver*> connectedNodes_;

	
	eLogicState outputSumState_;
	eLogicState outputCarryState_;
	eLogicState inputCarryState_;


	bool fromOutputY = false;

	// TODO: refactor what we can into private section?
	
};
