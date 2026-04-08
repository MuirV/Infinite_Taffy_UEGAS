// Copyright Daisy

#include "AbilitySystem/TaFeiAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// 引入你的标签和接口，请确保包含路径正确
#include "TaFeiAbilityTypes.h"
#include "TaFeiGameplayTags.h" 
#include "AbilitySystem/TaFeiAbilitySystemLibrary.h"
#include "Interaction/TaFeiCombatInterface.h"
#include "Interaction/TaFeiPlayerInterface.h"


#include "Player/TaFeiPlayerController.h"
#include "Player/TaFeiPlayerState.h"

UTaFeiAttributeSet::UTaFeiAttributeSet()
{
	// 给个默认初始值，方便测试
	// InitHealth(100.f);
	// InitMaxHealth(100.f);
	// InitMana(200.f);
	// InitMaxMana(200.f);
	// InitUltimateEnergy(0.f);
	// InitMaxUltimateEnergy(100.f);
	// InitDamageMultiplier(1.0f);
	// InitDamageReduction(0.0f); 
	// InitIncomingDamage(0.0f);

	const FTaFeiGameplayTags& GameplayTags = FTaFeiGameplayTags::Get();
	
	//最cpp高手一集在这里发挥用处，初始化Attributes反映到AttributeMenu UI上面，通过之前写的TMap(TagsToAttributes)
	
	//Primary Attributes
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	//Secondary Attributes
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);

	
}

void UTaFeiAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册需要网络同步的属性
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, UltimateEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxUltimateEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, DamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	//Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	//Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	
}

void UTaFeiAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 钳制上限值，防止出现最大血量变负数等 Bug
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetMaxUltimateEnergyAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

// 核心解包函数（照搬 Aura 架构，用于提取攻击者和受击者的 Controller、Character 等信息）
void UTaFeiAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	if(IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if(Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if(APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if(Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	
	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{	
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}		
}

void UTaFeiAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 使用结构体收集本次伤害事件的全部上下文
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	// 钳制当前值，防止溢出或变负
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	if (Data.EvaluatedData.Attribute == GetUltimateEnergyAttribute())
	{
		SetUltimateEnergy(FMath::Clamp(GetUltimateEnergy(), 0.f, GetMaxUltimateEnergy()));
	}

	//  扣血逻辑 (IncomingDamage)
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f); // 用完清零

		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			// 检查是否死亡
			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				// 调用你的战斗接口里的 Die() 逻辑
				if (ITaFeiCombatInterface* CombatInterface = Cast<ITaFeiCombatInterface>(Props.TargetAvatarActor))
				{
					// 死亡事件发送
					CombatInterface->Die(); 
				}
				// 确保这个 Log 放在 SendXPEvent(Props) 附近
				UE_LOG(LogTemp, Warning, TEXT("=== [XP_Log_1] 敌人死亡！准备发送 XP Event === 发送给目标: %s"), *Props.SourceCharacter->GetName());
				SendXPEvent(Props);
			}
			else
			{
				//这里用载荷，SendGameplayEventTOActor，为以后拓展HitEffect，比如击晕等等
				FGameplayEventData Payload;
				Payload.EventTag = FTaFeiGameplayTags::Get().Effects_HitReact;
				Payload.Instigator = Props.SourceAvatarActor;
				Payload.Target = Props.TargetAvatarActor;

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					Props.TargetAvatarActor,
					Payload.EventTag,
					Payload
				);
			}
			
			const bool bBlock = UTaFeiAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bCriticalHit = UTaFeiAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			// 确保在 if (GetOwningActor()->HasAuthority()) 里面打这个 Log
			
			
			ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		}
	}
	
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
	    const float LocalIncomingXP = GetIncomingXP();
	    SetIncomingXP(0.f);

	    UE_LOG(LogTemp, Warning, TEXT("=== [XP_Log_3] 玩家 AttributeSet 成功接收 IncomingXP === 准备分配经验值: %f"), LocalIncomingXP);

	    // 精准获取施法者的专属 TaFeiPlayerState
	    ATaFeiPlayerState* TaFeiPS = nullptr;
	    if (Props.SourceController)
	    {
	        // 从 Controller 身上拿并强转
	        TaFeiPS = Cast<ATaFeiPlayerState>(Props.SourceController->PlayerState);
	    }
	    if (!TaFeiPS && Props.SourceASC)
	    {
	        // 兜底：从 ASC 的 Owner 身上拿并强转
	        TaFeiPS = Cast<ATaFeiPlayerState>(Props.SourceASC->GetOwnerActor());
	    }

	    //  只有当确实是 TaFeiPlayerState 时，才进行所有的接口调用！
	    if (TaFeiPS && TaFeiPS->Implements<UTaFeiPlayerInterface>() && TaFeiPS->Implements<UTaFeiCombatInterface>())
	    {
	        // 这里的调用将毫无疑问地走入 TaFeiPlayerState.cpp
	        const int32 CurrentLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(TaFeiPS);
	        const int32 CurrentXP = ITaFeiPlayerInterface::Execute_GetXP(TaFeiPS);

	        const int32 NewLevel = ITaFeiPlayerInterface::Execute_FindLevelForXP(TaFeiPS, CurrentXP + LocalIncomingXP);
	        const int32 NumLevelUps = NewLevel - CurrentLevel;
	        
	        if (NumLevelUps > 0)
	        {
	            const int32 AttributePointsReward = ITaFeiPlayerInterface::Execute_GetAttributePointsReward(TaFeiPS, CurrentLevel);
	            const int32 SpellPointsReward = ITaFeiPlayerInterface::Execute_GetSpellPointsReward(TaFeiPS, CurrentLevel);

	            ITaFeiPlayerInterface::Execute_AddToPlayerLevel(TaFeiPS, NumLevelUps);
	            ITaFeiPlayerInterface::Execute_AddToAttributePoints(TaFeiPS, AttributePointsReward);
	            ITaFeiPlayerInterface::Execute_AddToSpellPoints(TaFeiPS, SpellPointsReward);

	            // 升级回满状态
	            SetHealth(GetMaxHealth());
	            SetMana(GetMaxMana());
	            SetUltimateEnergy(GetMaxUltimateEnergy());

	            // 告诉 TaFeiPlayerState 触发升级
	            ITaFeiPlayerInterface::Execute_LevelUp(TaFeiPS);
	        }
	        
	        // 增加经验值
	        ITaFeiPlayerInterface::Execute_AddToXP(TaFeiPS, LocalIncomingXP);
	    }
	}
		
}


void UTaFeiAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit,
	bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (ATaFeiPlayerController* PC = Cast<ATaFeiPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		if (ATaFeiPlayerController* PC = Cast<ATaFeiPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);					
		}		
	}
}

void UTaFeiAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetCharacter->Implements<UTaFeiCombatInterface>())
	{
		const int32 TargetLevel = ITaFeiCombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ETaFeiCharacterClass TargetClass = ITaFeiCombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const int32 XPReward = UTaFeiAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

		const FTaFeiGameplayTags& GameplayTags = FTaFeiGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		
		/// 换成下面这种直接调用 ASC 的写法：
		if (Props.SourceASC)
		{
			// 直接让攻击者的 ASC 处理这个事件，百分之百不会丢失！
			Props.SourceASC->HandleGameplayEvent(Payload.EventTag, &Payload);
    
			UE_LOG(LogTemp, Warning, TEXT("=== [XP_Log_1.5] 已直接向 ASC 塞入 Event ==="));
		}
	}
}

// -------------------- OnRep 复制函数实现 --------------------
void UTaFeiAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Health, OldHealth);
}
void UTaFeiAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, MaxHealth, OldMaxHealth);
}
void UTaFeiAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Mana, OldMana);
}
void UTaFeiAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, MaxMana, OldMaxMana);
}
void UTaFeiAttributeSet::OnRep_UltimateEnergy(const FGameplayAttributeData& OldEnergy) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, UltimateEnergy, OldEnergy);
}
void UTaFeiAttributeSet::OnRep_MaxUltimateEnergy(const FGameplayAttributeData& OldMaxEnergy) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, MaxUltimateEnergy, OldMaxEnergy);
}
void UTaFeiAttributeSet::OnRep_DamageMultiplier(const FGameplayAttributeData& OldMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, DamageMultiplier, OldMultiplier);
}
void UTaFeiAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldReduction) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, DamageReduction, OldReduction);
}
void UTaFeiAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Strength, OldStrength);
}

void UTaFeiAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Intelligence, OldIntelligence);
}

void UTaFeiAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Resilience, OldResilience);
}

void UTaFeiAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Vigor, OldVigor);
}

void UTaFeiAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, Armor, OldArmor);
}

void UTaFeiAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UTaFeiAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, BlockChance, OldBlockChance);
}

void UTaFeiAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UTaFeiAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UTaFeiAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UTaFeiAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UTaFeiAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTaFeiAttributeSet, ManaRegeneration, OldManaRegeneration);
}
