// Copyright Daisy

#include "AbilitySystem/TaFeiAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

// 引入你的标签和接口，请确保包含路径正确
#include "TaFeiGameplayTags.h" 
#include "Interaction/TaFeiCombatInterface.h"

UTaFeiAttributeSet::UTaFeiAttributeSet()
{
	// 给个默认初始值，方便测试
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitMana(200.f);
	InitMaxMana(200.f);
	InitUltimateEnergy(0.f);
	InitMaxUltimateEnergy(100.f);
	InitDamageMultiplier(1.0f);
	InitDamageReduction(0.0f); 
	InitIncomingDamage(0.0f);

	// ★ UI 数据映射绑定
	const FTaFeiGameplayTags& GameplayTags = FTaFeiGameplayTags::Get();
	
	// 注意：如果你在 TaFeiGameplayTags 中还没添加下面这些标签，请记得去添加！
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_UltimateEnergy, GetUltimateEnergyAttribute);
	
	// 你还可以映射 MaxHealth, Mana 等，根据你 UI 面板要显示什么来决定
}

void UTaFeiAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册需要网络同步的属性
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, UltimateEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, MaxUltimateEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, DamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTaFeiAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);
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

	// ★ 钳制当前值，防止溢出或变负
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

	// ★ 扣血逻辑 (IncomingDamage)
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
					// 注意：这里后续如果有死亡事件发送的需求可以补上
					// CombatInterface->Die(); 
				}
			}
			else
			{
				// 没死则播放受击动画 (如果需要的话，后续可在这里加发送 HitReact Event)
			}
			
			// 以后你如果做飘字功能 (Damage Text)，可以在这里调用
			// ShowFloatingText(Props, LocalIncomingDamage, ...);
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