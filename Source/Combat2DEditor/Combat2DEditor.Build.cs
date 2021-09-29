// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Combat2DEditor : ModuleRules
{
	public Combat2DEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "Paper2D",
                "Combat2D",
                "InputCore",
                "UnrealEd", // for FAssetEditorManager and Factory classes!!!!
                "KismetWidgets",
                "Kismet",  // for FWorkflowCentricApplication
				"Slate",
                "SlateCore",
                "RenderCore",
                "ContentBrowser",
                "EditorStyle",
                "EditorWidgets",
                "PropertyEditor",
                "MeshPaint",
                "Projects",
                "AssetRegistry",
            }
        );
	}
}
