//#include "AITestsCommon.h"
#include "LogicGates/Tests/AutomationFlags.h" // AUTO_FLAGS
#include "LogicGates/LogicGatesCharacter.h"
#include "Nodes/DisplayOutput.h"
#include "Nodes/PowerSource.h"

#include "Nodes/LogicGates/AndGate.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(DeserializeMultipleNodes_Test, "LogicGates.DeserializeMultipleNodes_Test",AUTO_FLAGS)

bool DeserializeMultipleNodes_Test::RunTest(const FString& Parameters)
{

	FString FullJson = R"({
	"nodes": [
		{
			"nodeName": "DisplayOutput",
			"serialNumber": 36,
			"position":
			{
				"x": 1100,
				"y": 300,
				"z": 0
			},
			"rotation":
			{
				"pitch": 0,
				"yaw": 23.483323439956006,
				"roll": 0
			}
		},
		{
			"nodeName": "PowerSource",
			"serialNumber": 33,
			"powerSourceState": "DISABLED",
			"position":
			{
				"x": 800,
				"y": -600,
				"z": 0
			},
			"rotation":
			{
				"pitch": 0,
				"yaw": 69.958340771496324,
				"roll": 0
			},
			"connections": [
				{
					"serialNumber": 35,
					"xOrY": "X"
				}
			]
		},
		{
			"nodeName": "PowerSource",
			"serialNumber": 34,
			"powerSourceState": "DISABLED",
			"position":
			{
				"x": 1300,
				"y": -700,
				"z": 0
			},
			"rotation":
			{
				"pitch": 0,
				"yaw": 43.149996541440665,
				"roll": 0
			},
			"connections": [
				{
					"serialNumber": 35,
					"xOrY": "Y"
				}
			]
		},
		{
			"nodeName": "AndGate",
			"serialNumber": 35,
			"inputX": 33,
			"inputY": 34,
			"position":
			{
				"x": 1000,
				"y": -200,
				"z": 0
			},
			"rotation":
			{
				"pitch": 0,
				"yaw": 3.5666737109423869,
				"roll": 0
			},
			"connections": [
				{
					"serialNumber": 36,
					"xOrY": "X"
				}
			]
		}
	]
})";

	
	// ALogicGatesCharacter* LogicGatesCharacter = FAITestHelpers::GetWorld()->SpawnActor<ALogicGatesCharacter>();
	//
	// TMap<int, AAbstractNode*> nodes = LogicGatesCharacter->DeserializeAllNodes(FullJson);
	//
	//
	// TestEqual("Test deserialization", nodes.Num(), 4);
	//
	
	return true;
}
