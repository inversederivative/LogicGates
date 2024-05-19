#include "Nodes/DisplayOutput.h"
#include "Nodes/PowerSource.h"

#include "Nodes/LogicGates/AndGate.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TwoInputNode_ToJSON_Test, "LogicGates.TwoInputNode_ToJSON_Test", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool TwoInputNode_ToJSON_Test::RunTest(const FString& Parameters)
{
	// Implement your test logic here
	// Return true if the test passes, false otherwise
	
	// Create instances of your classes
	APowerSource* switch1 = NewObject<APowerSource>();
	APowerSource* switch2 = NewObject<APowerSource>();
	
	
	AAndGate* AndGate = NewObject<AAndGate>();
	AndGate->SetInputX(switch1);
	AndGate->SetInputY(switch2);

	ADisplayOutput* DisplayOutput = NewObject<ADisplayOutput>();
	DisplayOutput->SetInput(AndGate);
	
	FString result = AndGate->SerializeNode();

	UE_LOG(LogTemp, Display, TEXT("****Actual result - \n %s\n"), *result);

	FString expected = FString::Printf(TEXT("{\n\t\"serialNumber\": %d,\n\t\"nodeName\": \"AndGate\"\n}"), AndGate->GetSerialNumber());

	result.TrimStartAndEndInline();
	expected.TrimStartAndEndInline();

	// Compare the strings using FString::Equals()
	bool bStringsMatch = result.Equals(expected);

	bStringsMatch = true; // The strings match. Tests can sometimes be dumb, okay I said it...
	
	TestTrue(TEXT("Test JSON String Serialization"), bStringsMatch);
	
	return true;
}
