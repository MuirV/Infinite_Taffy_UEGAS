// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "TaFeiAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API UTaFeiAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UTaFeiAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;	
};
