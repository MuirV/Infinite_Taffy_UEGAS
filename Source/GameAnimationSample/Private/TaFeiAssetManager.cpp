// Copyright Daisy


#include "TaFeiAssetManager.h"

#include "TaFeiGameplayTags.h"

UTaFeiAssetManager& UTaFeiAssetManager::Get()
{
	check(GEngine);
	UTaFeiAssetManager* TaFeiAssetManager = Cast<UTaFeiAssetManager>(GEngine->AssetManager);
	return *TaFeiAssetManager;
}

void UTaFeiAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// Core! 在引擎加载初期初始化我们写的原生 Tags
	FTaFeiGameplayTags::InitializeNativeGameplayTags();
	
	
	//5.3以下版本需要手动调用InitGlobalData来初始化全局数据，5.3版本之后会自动调用
	//UAbilitySystemGlobals::Get().InitGlobalData();
}