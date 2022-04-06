// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonAFighter_3DTarget : TargetRules
{
	public DungeonAFighter_3DTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[]{"DungeonAFighter_3D",
			"ArenaBattleSetting" });
	}
}
