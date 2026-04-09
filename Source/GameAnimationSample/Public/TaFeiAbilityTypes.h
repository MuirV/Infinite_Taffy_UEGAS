// Copyright Daisy

#pragma once

#include "GameplayEffectTypes.h"
#include "TaFeiAbilityTypes.generated.h"

/**
 * 自定义 GameplayEffect 上下文
 * 就像一个扩展的包裹，可以在这里加入任何未来需要的战斗数据传递给属性集或计算类
 */
USTRUCT(BlueprintType)
struct FTaFeiGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	// 预留的未来战斗属性：是否暴击，是否被格挡（以后可以随意添加，比如添加击退力 FVector KnockbackForce）
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	// 拓展: 完美闪避
	bool IsPerfectDodge() const { return bIsPerfectDodge; }
	
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	/** 必须重写以下三个函数才能让 GAS 认识并能在网络间同步我们自定义的结构体 */ //有点像编程子系统的味道了 hhh
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FTaFeiGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsPerfectDodge = false;
};

// 结构体需要网络序列化和复制
template<>
struct TStructOpsTypeTraits<FTaFeiGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FTaFeiGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};