// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class HeartWeaver : ModuleRules
{
	private string PluginsPath
	{
		get { return Path.GetFullPath( Path.Combine( ModuleDirectory, "../../Plugins/" ) ); }
	}
	
	protected void AddSPUD() {
		// Linker
		PrivateDependencyModuleNames.AddRange(new string[] { "SPUD" });
		// Headers
		PublicIncludePaths.Add(Path.Combine( PluginsPath, "SPUD", "Source", "SPUD", "Public"));
	}
	
	protected void AddSUDS() {
		// Linker
		PrivateDependencyModuleNames.AddRange(new string[] { "SUDS" });
		// Headers
		PublicIncludePaths.Add(Path.Combine( PluginsPath, "SUDS", "Source", "SUDS", "Public"));
	}
	
	public HeartWeaver(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "AIModule", "SPUD" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTasks", "AnimGraphRuntime" });
		
		AddSPUD();
		AddSUDS();
	}
}
