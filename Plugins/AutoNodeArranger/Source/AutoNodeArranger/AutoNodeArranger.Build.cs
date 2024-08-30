// Copyright 2022 bstt, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AutoNodeArranger : ModuleRules
{
	public AutoNodeArranger(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...
				"BlueprintGraph",
				"GraphEditor",
				"InputCore",
				"UnrealEd",
				"MainFrame",
                "Json",
                "Projects",
                "JsonUtilities"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
