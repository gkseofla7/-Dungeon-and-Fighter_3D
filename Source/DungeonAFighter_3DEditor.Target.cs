// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class DungeonAFighter_3DEditorTarget : TargetRules
{
	public DungeonAFighter_3DEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[]{"DungeonAFighter_3D",
			"ArenaBattleSetting" });
	}
}
