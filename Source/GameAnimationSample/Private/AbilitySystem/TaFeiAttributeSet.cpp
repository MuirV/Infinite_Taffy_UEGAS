// Copyright Daisy


#include "AbilitySystem/TaFeiAttributeSet.h"
#include "GameplayEffectExtension.h"

UTaFeiAttributeSet::UTaFeiAttributeSet()
{
	// 给个默认初始值，方便测试
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitMana(200.f);
	InitMaxMana(200.f);
	InitUltimateEnergy(0.f);
	InitMaxUltimateEnergy(100.f);

	// 战斗属性
	InitDamageMultiplier(1.0f); // 默认 1 倍伤害
	InitDamageReduction(0.0f);  // 默认 0 减伤
    
	InitIncomingDamage(0.0f);
	
}

void UTaFeiAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 限制血量在 0 到 MaxHealth 之间
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
    {
       // 修复：之前这里钳制成了 GetMaxMana()
       NewValue = FMath::Max(NewValue, 1.f); 
    }
	// 限制能量在 0 到 MaxUltimateEnergy 之间
	else if (Attribute == GetUltimateEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxUltimateEnergy());
	}
	
}

void UTaFeiAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 当有 GameplayEffect 修改了我们的属性后触发
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		//获取传入的伤害值
		const float LocalIncomingDamage = GetIncomingDamage();
        
		//清空 IncomingDamage，防止下次计算累加
		SetIncomingDamage(0.f);

		if (LocalIncomingDamage > 0.f)
		{
			//实际扣血逻辑：最终血量 = 当前血量 - 传入的伤害
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			// TODO: 未来可以在这里触发受击动画 (HitReact) 或死亡逻辑
		}
	}
}
