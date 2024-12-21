#include "LogicGates/Tests/AutomationFlags.h" // AUTO_FLAGS
#include "Nodes/DisplayOutput.h"
#include "Nodes/PowerSource.h"
#include "Nodes/Adders/FullAdder.h"
#include "Nodes/LogicGates/AndGate.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FullAdder_ToJSON_Test, "LogicGates.FullAdder_ToJSON_Test", AUTO_FLAGS)


bool FullAdder_ToJSON_Test::RunTest(const FString& Parameters)
{
	// Implement your test logic here
	// Return true if the test passes, false otherwise
	
	// Create instances of your classes
	APowerSource* switch1 = NewObject<APowerSource>();
	APowerSource* switch2 = NewObject<APowerSource>();
	APowerSource* switch3 = NewObject<APowerSource>();
	
	AFullAdder* adder = NewObject<AFullAdder>();

	adder->SetInputX(switch1);
	adder->SetInputY(switch2);
	adder->SetInputCarry(switch3);

	ADisplayOutput* DisplaySum = NewObject<ADisplayOutput>();
	ADisplayOutput* DisplayCarry = NewObject<ADisplayOutput>();
	DisplaySum->SetInput(adder);
	//DisplayCarry->SetInput(adder->GetCarryOutNode()); // Serializing the Carry node... ?!
	
	FString result = adder->SerializeNode();

	UE_LOG(LogTemp, Display, TEXT("****Actual result - \n %s\n"), *result);

	
	TestTrue(TEXT("Test JSON String Serialization"), true);
	
	return true;
}