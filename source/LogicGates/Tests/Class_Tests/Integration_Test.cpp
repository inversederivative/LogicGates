#include "Nodes/PowerSource.h"
#include "Nodes/DisplayOutput.h"
#include "Nodes/LogicGates/AndGate.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(Integration_Test, "LogicGates.Integration_Test", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)


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
