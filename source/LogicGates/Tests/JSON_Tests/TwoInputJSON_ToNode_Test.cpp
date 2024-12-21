//#include "AITestsCommon.h"
#include "LogicGates/Tests/AutomationFlags.h" // AUTO_FLAGS
#include "LogicGates/LogicGatesCharacter.h"
#include "Nodes/DisplayOutput.h"
#include "Nodes/PowerSource.h"

#include "Nodes/LogicGates/AndGate.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TwoInputJSON_ToNode_Test, "LogicGates.TwoInputJSON_ToNode_Tests", AUTO_FLAGS)

bool TwoInputJSON_ToNode_Test::RunTest(const FString& Parameters)
{

	FString AndGateJSON = R"({
			"nodeName": "AndGate",
			"serialNumber": 35,
			"position":
			{
				"x": 900,
				"y": 100,
				"z": 0
			},
			"rotation":
			{
				"pitch": 0,
				"yaw": 4.2249927520751998,
				"roll": 0
			},
			"connectedNodes": [
				{
					"serialNumber": 33,
					"nodeName": "PowerSource"
				},
				{
					"serialNumber": 34,
					"nodeName": "PowerSource"
				}
			],
			"observers": [
				{
					"serialNumber": 36,
					"nodeName": "DisplayOutput"
				}
			]
		})";

	//ALogicGatesCharacter* LogicGatesCharacter = GetWorld()->SpawnActor<ALogicGatesCharacter>();


	
	// ALogicGatesCharacter* LogicGatesCharacter = FAITestHelpers::GetWorld()->SpawnActor<ALogicGatesCharacter>();
	//
	// AAbstractNode* node = LogicGatesCharacter->DeserializeNode(AndGateJSON);
	//
	// AAndGate* andGate = Cast<AAndGate>(node);
	//
	// TestEqual("Test deserialization", andGate->GetNodeName(), "AndGate");
	//
	
	return true;
}
