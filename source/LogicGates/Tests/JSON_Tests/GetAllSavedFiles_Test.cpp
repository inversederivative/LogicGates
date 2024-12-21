//#include "AITestsCommon.h"
#include "LogicGates/Tests/AutomationFlags.h" // AUTO_FLAGS
#include "LogicGates/LogicGatesCharacter.h"
#include "Nodes/DisplayOutput.h"
#include "Nodes/PowerSource.h"

#include "Nodes/LogicGates/AndGate.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(GetAllSavedFiles_Test, "LogicGates.GetAllSavedFiles_Test", AUTO_FLAGS)

bool GetAllSavedFiles_Test::RunTest(const FString& Parameters)
{

	FString DirectoryPath = "C:/Test";

	
	// ALogicGatesCharacter* LogicGatesCharacter = FAITestHelpers::GetWorld()->SpawnActor<ALogicGatesCharacter>();
	//
	// TArray<FString> files = LogicGatesCharacter->GetSavedFiles(DirectoryPath);
	//
	// TestEqual("Test deserialization", files[0], "Nodes22");
	//
	
	return true;
}
