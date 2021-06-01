// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Combat2DEditor : ModuleRules
{
    public Combat2DEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                "Combat2DEditor/Public"
				// ... add public include paths required here ...
			}
        );


        PrivateIncludePaths.AddRange(
            new string[]
            {
                "Combat2DEditor/Private",
                "Combat2DEditor/Classes"
				// ... add other private include paths required here ...
			}
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Paper2D"
				// ... add other public dependencies that you statically link with here ...
			}
        );
        

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Json",
                "Slate",
                "SlateCore",
                "Engine",
                "InputCore",
                "UnrealEd", // for FAssetEditorManager
                "KismetWidgets",
                "Kismet",  // for FWorkflowCentricApplication
                "PropertyEditor",
                "RenderCore",
                "ContentBrowser",
                "WorkspaceMenuStructure",
                "EditorStyle",
                "MeshPaint",
                "EditorWidgets",
                "Projects",
                "AssetRegistry"

                // ... add private dependencies that you statically link with here ...	
	        }
        );


        PrivateIncludePathModuleNames.AddRange(
           new string[]
           {
                    "Settings",
                    "IntroTutorials",
                    "AssetTools",
                    "LevelEditor",
                    "ProjectMansion"
           }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                "AssetTools"
            }
        );
    }
}
