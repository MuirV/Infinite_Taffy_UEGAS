// Copyright Daisy

#include "Game/TaFeiGameModeBase.h"
#include "GameFramework/Character.h"

ATaFeiGameModeBase::ATaFeiGameModeBase()
{
	
}

void ATaFeiGameModeBase::NotifyCharacterDied(ACharacter* DeadCharacter)
{
	// 留给以后的死亡结算逻辑
	// 比如判断 DeadCharacter 是不是玩家，如果是，触发 Game Over UI
}

