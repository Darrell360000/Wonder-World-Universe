using UnrealBuildTool;

public class WonderTowerTycoon : ModuleRules
{
    public WonderTowerTycoon(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "Niagara", "UMG", "Slate", "SlateCore"
        });
    }
}
