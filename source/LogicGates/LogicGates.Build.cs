// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LogicGates : ModuleRules
{
	public LogicGates(ReadOnlyTargetRules Target) : base(Target)
	{
		// PrivateDependencyModuleNames.AddRange(new string[] { "AITestSuite" });
		// PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"CableComponent",
			"Json"
		});
	}
}
