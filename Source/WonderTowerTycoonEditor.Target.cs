using UnrealBuildTool;
using System.Collections.Generic;

public class WonderTowerTycoonEditorTarget : TargetRules
{
	public WonderTowerTycoonEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.Add("WonderTowerTycoon");
	}
}
