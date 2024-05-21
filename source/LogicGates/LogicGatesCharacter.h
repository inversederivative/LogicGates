// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LogicGates/Public/AbstractNode.h"
#include "Logging/LogMacros.h"
#include "LogicGatesCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ALogicGatesCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Enhanced Input Component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UEnhancedInputComponent* EnhancedInputComponent;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenBuildMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PlaceObjectAction;
	
	/** For Toggling Power Sources*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PowerToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ConnectFromOutputXAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ConnectFromOutputYAction;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ConnectToInputXAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ConnectToInputYAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ConnectToInputZAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SerializeAllAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DeSerializeAllAction;
	
	/** Reference to the current node in radius*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* CurrentNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Power", meta = (AllowPrivateAccess = "true"))
	AAbstractNode* LastOutputNode;

	bool ConnectedToHand;

	bool IsFromOutputY;

public:
	ALogicGatesCharacter();

	void SetCurrentNode(AAbstractNode* currentNode);

	void SetLastOutputNode(AAbstractNode* lastOutputNode);

	UFUNCTION(BlueprintCallable)
	FString SerializeAllNodes(const TArray<AAbstractNode*>& AllNodes);

	UFUNCTION(BlueprintCallable)
	FString ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage);
	
	UFUNCTION(BlueprintCallable)
	void WriteStringToFile(FString FilePath, FString String, bool& bOutSuccess, FString& OutInfoMessage);
	
	UFUNCTION(BlueprintCallable)
	FString GetUniqueFilePath(const FString& BasePath);

	UFUNCTION(BlueprintCallable)
	AAbstractNode* DeserializeNode(FString nodeJson);

	// Not implemented
	AAbstractNode* DeserializeNodeFromJsonObject(const TSharedPtr<FJsonObject>& NodeJsonObject);

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetSavedFiles(const FString& directoryPath);
	
	UFUNCTION(BlueprintCallable)
	TMap<int, AAbstractNode*> DeserializeAllNodes(FString fullJson);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void OpenBuildMenu();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void PlaceObject();
	
	UFUNCTION(BlueprintCallable, Category = "Power")
	void ToggleCurrentPowerSource();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void ConnectFromOutputX();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void ConnectFromOutputY();
	
	UFUNCTION(BlueprintCallable, Category = "Power")
	void ConnectToInputX();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void ConnectToInputY();

	UFUNCTION(BlueprintCallable, Category = "Power")
	void ConnectToInputZ();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns the CurrentNode */
	FORCEINLINE AAbstractNode* GetCurrentNode() const {return CurrentNode;}

	FORCEINLINE AAbstractNode* GetLastOutputNode() const {return LastOutputNode;}
};

