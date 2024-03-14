// NodeInterface.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <iostream>
#include <list>
#include "NodeInterface.generated.h"


UENUM(BlueprintType)
enum eLogicState
{
	DISABLED = 0 UMETA(DisplayName = "DISABLED"),
	OFF = 1 UMETA(DisplayName = "OFF"),
	ON = 2 UMETA(DisplayName = "ON")
};

UINTERFACE(Blueprintable)
class USubject : public UInterface
{
	GENERATED_BODY()
};

UINTERFACE(Blueprintable)
class UObserver : public UInterface
{
	GENERATED_BODY()
};

class LOGICGATES_API IObserver
{
	GENERATED_BODY()
public:
	//virtual ~IObserver()= default;
	virtual void Update(eLogicState state) PURE_VIRTUAL(IObserver::Update,);
};

class LOGICGATES_API ISubject
{
	GENERATED_BODY()
public:
	//virtual ~ISubject()= default;
	virtual void Attach(IObserver *observer) PURE_VIRTUAL(IObserver::Attach,);
	virtual void Detach(IObserver *observer) PURE_VIRTUAL(IObserver::Detach,);
	virtual void Notify() PURE_VIRTUAL(IObserver::Notify,);
};
