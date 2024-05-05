// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "Components/SphereComponent.h"
#include "AbstractTwoInputNode.generated.h"

/**
 * 
 */
UCLASS()
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
	void Update(eLogicState state) override;
	
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
	USceneComponent* LogicGateSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DisplayMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DisabledMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OffMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OnMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* StandMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* CableConnector;
	
	// Connection Ports
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPortX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPortY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Ports", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* OutputPortX;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;
	
	// TODO: Get rid of the private members in AndGate, and setup abstract getters and setters here.
	// TODO: find a way to make these members private not protected?
	AAbstractNode* inputX;
	AAbstractNode* inputY;
	eLogicState outputState_;
	std::list<IObserver*> connectedNodes_;

	
};
