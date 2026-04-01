// Copyright Daisy


#include "Player/TaFeiPlayerState.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassData.h"
#include "AbilitySystem/TaFeiAttributeSet.h"

ATaFeiPlayerState::ATaFeiPlayerState()
{
	// Aura 动作游戏标准：提高网络同步频率，防止动画和伤害脱节
	// ReSharper disable once CppDeprecatedEntity
	NetUpdateFrequency = 100.f;

	//实例化 ASC
	AbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// 联机动作游戏推荐 Mixed 模式
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 实例化 AttributeSet
	AttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ATaFeiPlayerState::GetAbilitySystemComponent() const
{

	return AbilitySystemComponent;
}

void ATaFeiPlayerState::InitializeGASForPawn(APawn* AvatarPawn)
{

	// 如果已经初始化过，或者组件/Pawn无效，直接退出
	if (bGASInitialized || !AbilitySystemComponent || !AvatarPawn) return;
	
	if (AbilitySystemComponent && AvatarPawn)
	{
		// GAS 最重要  绑定灵魂(Owner)和肉体(Avatar)
		// Owner 是 PlayerState(this)，Avatar 是你的蓝图角色(AvatarPawn)
		AbilitySystemComponent->InitAbilityActorInfo(this, AvatarPawn);

		// 锁上，确保这一段肉体只绑定一次
		bGASInitialized = true;
		
	}
	//只有服务器（Authority）才有资格赋予技能和初始属性 
	if (HasAuthority())
	{
		InitializeAttributes();
		AddStartupAbilities();
	}

	UE_LOG(LogTemp, Warning, TEXT("GAS 灵魂注入成功！Owner: %s, Avatar: %s"), *GetName(), *AvatarPawn->GetName());

	
}

// 实现初始化属性逻辑
void ATaFeiPlayerState::InitializeAttributes()
{
	// 如果没有配置 DataAsset，或者没有配置 PrimaryAttributes(GE)，则跳过
	if (!AbilitySystemComponent || !CharacterData || !CharacterData->PrimaryAttributes)
	{
		return;
	}

	// 制作一个 GE 的上下文 (Context)，记录是谁施加的这个 GE
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	// 【重构】：将原来的 1.f 改为 GetPlayerLevel()
	float CurrentLevel = static_cast<float>(GetPlayerLevel());
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		CharacterData->PrimaryAttributes, CurrentLevel, ContextHandle);
	
	// 将 GE 应用给自己
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

// 实现赋予技能逻辑
void ATaFeiPlayerState::AddStartupAbilities()
{
	// 如果没有配置 DataAsset，跳过
	if (!AbilitySystemComponent || !CharacterData)
	{
		return;
	}

	// 遍历我们在 DataAsset 里配置的所有初始技能
	for (const FTaFeiAbilityInfo& AbilityInfo : CharacterData->StartupAbilities)
	{
		// 确保技能类不是空的
		if (AbilityInfo.AbilityClass)
		{
			// 给 ASC 赋予这个技能，1 代表技能等级是 1 级
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, GetPlayerLevel());

			// ★ 核心操作：如果配置了输入标签，就把它注入到技能实例的动态标签中
			if (AbilityInfo.InputTag.IsValid())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
			}

			// 正式把技能交给 ASC
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}
}