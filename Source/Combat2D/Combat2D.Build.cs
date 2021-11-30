// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Combat2D : ModuleRules
{
	public Combat2D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Paper2D",
                "Engine",
                "RenderCore",
                "RHI"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Renderer"
			}
		);
	}
}
