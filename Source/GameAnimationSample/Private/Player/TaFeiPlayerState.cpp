// Copyright Daisy


// ReSharper disable CppDeprecatedEntity
#include "Player/TaFeiPlayerState.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "Net/UnrealNetwork.h"

ATaFeiPlayerState::ATaFeiPlayerState()
{
	// Aura 动作游戏标准：提高网络同步频率，防止动画和伤害脱节
	// ReSharper disable once CppDeprecatedEntity
	

	//实例化 ASC
	AbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	// 联机动作游戏推荐 Mixed 模式
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 实例化 AttributeSet
	AttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");

	SetNetUpdateFrequency(100.f);
}

void ATaFeiPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 在 GetLifetimeReplicatedProps 注册同步
	DOREPLIFETIME(ATaFeiPlayerState, Level);
	DOREPLIFETIME(ATaFeiPlayerState, XP);
	DOREPLIFETIME(ATaFeiPlayerState, AttributePoints);
	DOREPLIFETIME(ATaFeiPlayerState, SpellPoints);
}

// 接口实现
int32 ATaFeiPlayerState::GetXP_Implementation() const { return XP; }

int32 ATaFeiPlayerState::FindLevelForXP_Implementation(int32 InXP) const
{
	if (LevelUpInfo) return LevelUpInfo->FindLevelForXP(InXP);
	return 1;
}

int32 ATaFeiPlayerState::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	// 安全检查：确保蓝图里配置了 LevelUpInfo，并且等级没有超出数组范围
	if (LevelUpInfo && LevelUpInfo->LevelUpInformation.IsValidIndex(InLevel))
	{
		// 直接返回自己身上 LevelUpInfo 里的数据
		return LevelUpInfo->LevelUpInformation[InLevel].AttributePointAward;
	}
	return 0;
}

int32 ATaFeiPlayerState::GetSpellPointsReward_Implementation(int32 InLevel) const
{
	// 安全检查：确保蓝图里配置了 LevelUpInfo，并且等级没有超出数组范围
	if (LevelUpInfo && LevelUpInfo->LevelUpInformation.IsValidIndex(InLevel))
	{
		// 直接返回自己身上 LevelUpInfo 里的数据
		return LevelUpInfo->LevelUpInformation[InLevel].SpellPointAward;
	}
	return 0;
}

int32 ATaFeiPlayerState::GetAttributePoints_Implementation() const
{
	return AttributePoints;
}

int32 ATaFeiPlayerState::GetSpellPoints_Implementation() const
{
	return SpellPoints;
}

void ATaFeiPlayerState::AddToXP_Implementation(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ATaFeiPlayerState::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	Level += InPlayerLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void ATaFeiPlayerState::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ATaFeiPlayerState::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void ATaFeiPlayerState::LevelUp_Implementation()
{
	// 可在这里触发升级特效，交给蓝图覆写更佳
}

void ATaFeiPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(InXP);
}

void ATaFeiPlayerState::SetToLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(InLevel);
}

// OnRep 实现
void ATaFeiPlayerState::OnRep_Level(int32 OldLevel) { OnLevelChangedDelegate.Broadcast(Level); }

void ATaFeiPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ATaFeiPlayerState::OnRep_SpellPoints(int32 OldLevel)
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void ATaFeiPlayerState::OnRep_XP(int32 OldXP) { OnXPChangedDelegate.Broadcast(XP); }

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

		//通知我们自定义的 ASC 去绑定它自己的 UI 委托！
		Cast<UTaFeiAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
		
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
	if (!AbilitySystemComponent || !CharacterData )
	{
		return;
	}

	// 制作一个 GE 的上下文 (Context)，记录是谁施加的这个 GE
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	// 【重构】：将原来的 1.f 改为 GetPlayerLevel()
	float CurrentLevel = static_cast<float>(GetPlayerLevel());
	
	// 核心改动：先从字典里查出当前角色(Player)对应的专属数据包
	FTaFeiCharacterClassDefaultInfo ClassInfo = CharacterData->GetClassDefaultInfo(CharacterClass);

    // 初始化主属性 (使用查出来的数据包里的 PrimaryAttributes)
	if (ClassInfo.PrimaryAttributes)
	{
		FGameplayEffectSpecHandle PrimarySpec = AbilitySystemComponent->MakeOutgoingSpec(ClassInfo.PrimaryAttributes, CurrentLevel, ContextHandle);
		if (PrimarySpec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*PrimarySpec.Data.Get());
		}
	}

    // 初始化副属性 (使用公共的 SecondaryAttributes)
	if (CharacterData->SecondaryAttributes)
	{
		FGameplayEffectSpecHandle SecondarySpec = AbilitySystemComponent->MakeOutgoingSpec(CharacterData->SecondaryAttributes, CurrentLevel, ContextHandle);
		if (SecondarySpec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data.Get());
		}
	}

    //  初始化核心生命法力 (使用公共的 VitalAttributes)
	if (CharacterData->VitalAttributes)
	{
		FGameplayEffectSpecHandle VitalSpec = AbilitySystemComponent->MakeOutgoingSpec(CharacterData->VitalAttributes, CurrentLevel, ContextHandle);
		if (VitalSpec.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
		}
	}
}

// 实现赋予技能逻辑
void ATaFeiPlayerState::AddStartupAbilities()
{
	// 只有服务器有资格给技能
	if (!HasAuthority()) return;

	// 获取 ASC 并发号施令，把所有的活儿丢给 ASC 去干
	if (UTaFeiAbilitySystemComponent* TaFeiASC = Cast<UTaFeiAbilitySystemComponent>(AbilitySystemComponent))
	{
		TaFeiASC->AddStartupAbilitiesFromData(CharacterData, CharacterClass, GetPlayerLevel());
	}
 //    //这里完全重构了，为了减轻PlayerState的职责，我们选择AddStartupAbilitiesFromData()都写在Asc中，干脏活累活。不然以后AI都要复制一遍PlayerState的代码
 //    if (!AbilitySystemComponent || !CharacterData)
 //    {
 //       return;
 //    }
 //
	// // characterclassinfo新增enum后：从字典里查出当前角色(Player)对应的专属数据包
	// FTaFeiCharacterClassDefaultInfo ClassInfo = CharacterData->GetClassDefaultInfo(CharacterClass);
 //
 //    // 遍历数据包里的初始技能
 //    for (const FTaFeiAbilityInfo& AbilityInfo : ClassInfo.StartupAbilities)
 //    {
 //       // 确保技能类不是空的
 //       if (AbilityInfo.AbilityClass)
 //       {
 //          // 给 ASC 赋予这个技能
 //          FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, GetPlayerLevel());
 //
 //          // 如果配置了输入标签，就把它注入到技能实例的动态标签中
 //          if (AbilityInfo.InputTag.IsValid())
 //          {
 //             AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
 //          }
 //
 //          // 正式把技能交给 ASC
 //          AbilitySystemComponent->GiveAbility(AbilitySpec);
 //       }
 //    }
 //
	// // --- 在 for 循环结束后加上这两行 (需要将 ASC 强转为 TaFeiASC 才能访问) ---
	// //有待商催  确实架构上存在问题，如果bStartupAbilitiesGiven都写在PlayerState，会导致职责过重，以后拓展AI和敌人都得复制一遍PlayerState
	// if (UTaFeiAbilitySystemComponent* TaFeiASC = Cast<UTaFeiAbilitySystemComponent>(AbilitySystemComponent))
	// {
	// 	TaFeiASC->bStartupAbilitiesGiven = true;
	// 	TaFeiASC->AbilitiesGivenDelegate.Broadcast(TaFeiASC);
	// }
	
}