using UnrealBuildTool;
using System.Collections.Generic;

public class UE5_ComputeShadersTarget : TargetRules
{
    public UE5_ComputeShadersTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange(new string[] { "UE5_ComputeShaders" });
    }
}