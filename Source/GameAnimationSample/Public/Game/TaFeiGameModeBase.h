// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TaFeiGameModeBase.generated.h"

class UCharacterClassInfo;
/**
 * TaFei 项目的基础 GameMode
 * 目前作为 GM_Sandbox 的 C++ 父类。
 * 以后如果有全局的战斗结算、玩家死亡重生逻辑、怪物刷波次逻辑，都可以写在这里。
 */
UCLASS()
class GAMEANIMATIONSAMPLE_API ATaFeiGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATaFeiGameModeBase();

	// 预留一个接口：当主角或敌人死亡时，可以通过 Controller 通知 GameMode 进行后续处理
	// 例如：如果是玩家死了，等几秒后重新生成；如果是敌人死了，加分等。
	virtual void NotifyCharacterDied(ACharacter* DeadCharacter);

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Info")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
};