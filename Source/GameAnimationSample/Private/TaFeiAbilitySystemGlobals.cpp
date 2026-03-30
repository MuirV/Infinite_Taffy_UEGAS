// Copyright Daisy


#include "TaFeiAbilitySystemGlobals.h"
#include "TaFeiAbilityTypes.h" // 引入我们刚才写的自定义 Context

FGameplayEffectContext* UTaFeiAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	// 返回 Custom Context
	return new FTaFeiGameplayEffectContext();
}
