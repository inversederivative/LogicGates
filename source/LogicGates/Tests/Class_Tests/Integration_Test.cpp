#include "Nodes/PowerSource.h"
#include "Nodes/DisplayOutput.h"
#include "Nodes/LogicGates/AndGate.h"

#define AUTO_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(Integration_Test, "LogicGates.Integration_Test", AUTO_FLAGS)


bool Integration_Test::RunTest(const FString& Parameters)
{
	// Implement your test logic here
	// Return true if the test passes, false otherwise
	
	// Create instances of your classes
	APowerSource* switchX = NewObject<APowerSource>();
	APowerSource* switchY = NewObject<APowerSource>();
	AAndGate* AndGate = NewObject<AAndGate>();
	ADisplayOutput* Display = NewObject<ADisplayOutput>();
	
	AndGate->SetInputX(switchX);
	AndGate->SetInputY(switchY);
	Display->SetInput(AndGate);
	
	switchX->ChangeState(ON);
	switchY->ChangeState(ON);
	
	TestEqual("Delegates are Working!", Display->GetState(), ON);
	
	return true;
}
