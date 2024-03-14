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

	UFUNCTION(BlueprintCallable, Category = "Power")
	void ChangeState(eLogicState state);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Power")
	void ToggleState();
	
	UFUNCTION(BlueprintPure)
	eLogicState GetState() const override;

	void Update(eLogicState state) override {
		// Do nothing!
		// The switch doesn't have any objects, this is just for the interface requirement.
	}
	
protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DisplayMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* DisabledMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OffMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* OnMaterial;
	
	eLogicState state_;
};
