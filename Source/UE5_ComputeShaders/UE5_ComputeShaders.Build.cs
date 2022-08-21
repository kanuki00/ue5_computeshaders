using UnrealBuildTool;

public class UE5_ComputeShaders : ModuleRules
{
    public UE5_ComputeShaders(ReadOnlyTargetRules Target) : base(Target)
    {
        // Precompiled header usage.
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTasks", "AIModule", "MyShaders"});

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[] { "/Users/kanuki/Documents/Unreal Projects/UE5_ComputeShaders/Plugins/ComputeShader/Source/MyShaders/Public" });
    }
}