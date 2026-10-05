// Copyright (c) 2026 Sidney Levin (VireliaDev)

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MDA : ModuleRules
{
	public MDA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"OnlineSubsystem",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			
		});
		
	}
}
