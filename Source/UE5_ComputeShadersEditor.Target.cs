using UnrealBuildTool;
using System.Collections.Generic;

public class UE5_ComputeShadersEditorTarget : TargetRules
{
    public UE5_ComputeShadersEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange(new string[] { "UE5_ComputeShaders" });
    }
}