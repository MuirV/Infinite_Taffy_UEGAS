// Copyright Daisy

#include "TaFeiAbilityTypes.h"

UScriptStruct* FTaFeiGameplayEffectContext::GetScriptStruct() const
{
	return FTaFeiGameplayEffectContext::StaticStruct();
}

FTaFeiGameplayEffectContext* FTaFeiGameplayEffectContext::Duplicate() const
{
	FTaFeiGameplayEffectContext* NewContext = new FTaFeiGameplayEffectContext();
	*NewContext = *this;
	if (GetHitResult())
	{
		// 确保 HitResult 也是深拷贝
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	return NewContext;
}

bool FTaFeiGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// 使用位掩码（Bitmask）来优化网络带宽
	// GAS 所有跨网络的数据：
	// 必须自己序列化！
	uint32 RepBits{ 0 };
	if (Ar.IsSaving())
	{
		// 基础属性的位标记 (0-6)
		if (bReplicateInstigator && Instigator.IsValid()) { RepBits |= 1 << 0; }
		if (bReplicateEffectCauser && EffectCauser.IsValid()) { RepBits |= 1 << 1; }
		if (AbilityCDO.IsValid()) { RepBits |= 1 << 2; }
		if (bReplicateSourceObject && SourceObject.IsValid()) { RepBits |= 1 << 3; }
		if (Actors.Num() > 0) { RepBits |= 1 << 4; }
		if (HitResult.IsValid()) { RepBits |= 1 << 5; }
		if (bHasWorldOrigin) { RepBits |= 1 << 6; }

		// TaFei 自定义属性的位标记 (从 7 开始)
		if (bIsBlockedHit) { RepBits |= 1 << 7; }
		if (bIsCriticalHit) { RepBits |= 1 << 8; }
		
		// 未来你加了击退力、伤害类型等，就继续 1 << 9, 1 << 10 往下排
	}

	// 告诉网络序列化器：我们只打包前 9 位数据
	Ar.SerializeBits(&RepBits, 9);

	// ----------------- 数据解包 / 映射 -----------------
	// 基础数据映射
	if (RepBits & (1 << 0)) { Ar << Instigator; }
	if (RepBits & (1 << 1)) { Ar << EffectCauser; }
	if (RepBits & (1 << 2)) { Ar << AbilityCDO; }
	if (RepBits & (1 << 3)) { Ar << SourceObject; }
	if (RepBits & (1 << 4)) { SafeNetSerializeTArray_Default<31>(Ar, Actors); }
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading() && !HitResult.IsValid())
		{
			HitResult = TSharedPtr<FHitResult>(new FHitResult());
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}

	// TaFei 自定义数据映射
	if (RepBits & (1 << 7)) { Ar << bIsBlockedHit; }
	if (RepBits & (1 << 8)) { Ar << bIsCriticalHit; }

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); 
	}

	bOutSuccess = true;
	return true;
}