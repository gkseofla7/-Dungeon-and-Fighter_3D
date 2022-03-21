// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DungeonAFighter_3D : ModuleRules
{
	public DungeonAFighter_3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
