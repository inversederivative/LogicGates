// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractNode.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "DisplayOutput.generated.h"

class UStaticMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LOGICGATES_API ADisplayOutput : public AAbstractNode
{
	GENERATED_BODY()
	
public:

	ADisplayOutput();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Power")
	void SetInput(AAbstractNode* input);

	eLogicState GetState() const override;

	void Update(eLogicState state) override;
	
protected:

	UFUNCTION()
	void SetupMeshes();

	UFUNCTION()
	void SetupMaterials();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	std::list<IObserver*> connectedNodes_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* input_;
	
	eLogicState state_ = DISABLED;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Display", meta = (AllowPrivateAccess = "true"))
	bool bIsLit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* DisplaySceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScreenMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* StandMaterial;
	
	// Connector
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power Source", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* InputPort;
};
