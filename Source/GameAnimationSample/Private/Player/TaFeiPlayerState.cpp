// Copyright Daisy


#include "Player/TaFeiPlayerState.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"

ATaFeiPlayerState::ATaFeiPlayerState()
{
	// Aura 动作游戏标准：提高网络同步频率，防止动画和伤害脱节
	NetUpdateFrequency = 100.f;

	// 1. 实例化 ASC
	AbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// 联机动作游戏推荐 Mixed 模式
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 2. 实例化 AttributeSet
	AttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ATaFeiPlayerState::GetAbilitySystemComponent() const
{

	return AbilitySystemComponent;
}

void ATaFeiPlayerState::InitializeGASForPawn(APawn* AvatarPawn)
{

	// 如果已经初始化过，或者组件/Pawn无效，直接退出
	if (bGASInitialized || !AbilitySystemComponent || !AvatarPawn)
	{
		return;
	}
	
	if (AbilitySystemComponent && AvatarPawn)
	{
		// GAS 最重要的一步：绑定灵魂(Owner)和肉体(Avatar)
		// Owner 是 PlayerState(this)，Avatar 是你的蓝图角色(AvatarPawn)
		AbilitySystemComponent->InitAbilityActorInfo(this, AvatarPawn);

		// 锁上，确保这一段肉体只绑定一次
		bGASInitialized = true;
		
		// 打印日志，方便我们在编辑器里确认绑定成功
		UE_LOG(LogTemp, Warning, TEXT("GAS 灵魂注入成功！Owner: %s, Avatar: %s"), *GetName(), *AvatarPawn->GetName());
	}
	
}
