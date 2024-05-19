// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "PowerSource.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class LOGICGATES_API APowerSource : public AAbstractNode
{
	GENERATED_BODY()
public:	
	
	// Sets default values for this actor's properties
	APowerSource();
	explicit APowerSource(eLogicState state);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Power Source")
	void ChangeState(eLogicState state);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Power Source")
	void ToggleState();
	
	eLogicState GetState() const override;

	void Update(eLogicState state) override {
		// Do nothing!
		// The switch doesn't have any objects, this is just for the interface requirement.
	}

	bool GetIsNodeForOtherNodes() const
	{
		return IsNodeForOtherNodes;
	}

	void SetIsNodeForOtherNodes(bool answer)
	{
		IsNodeForOtherNodes = answer;
	}

	virtual FString SerializeNode() override;

	UFUNCTION(BlueprintCallable)
	void ResetConnectionsArray();
	
protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void SetupMeshes();

	UFUNCTION()
	void SetupMaterials();

	// UFUNCTION()
	// void AnimateButtonPosition();

private:

	eLogicState state_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* PowerSourceSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PowerSourceMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ButtonMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ConnectionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* StandMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	float AnimationDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimerHandle;

	bool IsNodeForOtherNodes;
};
