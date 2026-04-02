// Copyright Daisy

#include "Game/TaFeiGameModeBase.h"
#include "GameFramework/Character.h"

ATaFeiGameModeBase::ATaFeiGameModeBase()
{
	// 这里可以设置默认的 Pawn、PlayerController、HUD 等，
	// 但既然你已经有了 MG_Sandbox 蓝图，推荐在蓝图的类默认值里设置，这里留空即可。
}

void ATaFeiGameModeBase::NotifyCharacterDied(ACharacter* DeadCharacter)
{
	// 留给以后的死亡结算逻辑
	// 比如判断 DeadCharacter 是不是玩家，如果是，触发 Game Over UI
}

