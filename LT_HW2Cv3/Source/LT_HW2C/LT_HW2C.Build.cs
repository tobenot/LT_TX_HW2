// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LT_HW2C : ModuleRules
{
	public LT_HW2C(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "AIModule" });
	}
}
