// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;    // Path.Combine
using UnrealBuildTool;

public class Project_S : ModuleRules
{
	public Project_S(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "NavigationSystem", "AIModule", "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "Steamworks" });

        //PrivateDependencyModuleNames.AddRange(new string[] {  });

        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystemSteam" });

        PrivateIncludePaths.Add(
            Path.Combine(
                EngineDirectory,
                "Plugins/Online/OnlineSubsystemSteam/Source/Private"
            )
        );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        //Uncomment if you are using online features
        //PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
