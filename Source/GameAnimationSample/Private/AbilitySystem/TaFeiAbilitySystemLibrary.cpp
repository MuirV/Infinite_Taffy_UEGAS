// Copyright Daisy

#include "AbilitySystem/TaFeiAbilitySystemLibrary.h"

#include "TaFeiAbilityTypes.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "Game/TaFeiGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TaFeiPlayerState.h"
#include "UI/HUD/TaFeiHUD.h"
#include "Interaction/TaFeiCombatInterface.h"
#include "UI/WidgetController/TaFeiWidgetController.h"

// 获取主界面控制器
UTaFeiOverlayWidgetController* UTaFeiAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (ATaFeiHUD* TaFeiHUD = Cast<ATaFeiHUD>(PC->GetHUD()))
		{
			ATaFeiPlayerState* PS = PC->GetPlayerState<ATaFeiPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return TaFeiHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

// 获取属性面板控制器
UTaFeiAttributeMenuWidgetController* UTaFeiAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (ATaFeiHUD* TaFeiHUD = Cast<ATaFeiHUD>(PC->GetHUD()))
		{
			ATaFeiPlayerState* PS = PC->GetPlayerState<ATaFeiPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return TaFeiHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}


void UTaFeiAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ETaFeiCharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	FTaFeiCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	
	
}
void UTaFeiAbilitySystemLibrary::GiveStartupGameplayAbilities(
	const UObject* WorldContextObject, 
	UAbilitySystemComponent* ASC, 
	ETaFeiCharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	// ================================
	//  先给 CommonAbilities（所有角色都有）
	// ================================
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		if (!AbilityClass) continue;

		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	// ================================
	//  再给职业专属 StartupAbilities（你自己的结构）
	// ================================
	const FTaFeiCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	for (const FTaFeiAbilityInfo& AbilityInfo : DefaultInfo.StartupAbilities)
	{
		if (!AbilityInfo.AbilityClass) continue;

		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityInfo.AbilityClass, 1);

		// // 注入 InputTag（TaFei核心改造点） 改到ASC里面写了，此函数给敌人用。敌人获取这个函数，不需要InputTag功能
		// if (AbilityInfo.InputTag.IsValid())
		// {
		// 	AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
		// }

		ASC->GiveAbility(AbilitySpec);
	}
}


int32 UTaFeiAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ETaFeiCharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FTaFeiCharacterClassDefaultInfo Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}


UCharacterClassInfo* UTaFeiAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	// 从 World 中获取 GameMode 并转换
	if (ATaFeiGameModeBase* TaFeiGameMode = Cast<ATaFeiGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return TaFeiGameMode->CharacterClassInfo;
	}
	return nullptr;
	
}

bool UTaFeiAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FTaFeiGameplayEffectContext* TaFeiGameplayEffectContext = static_cast<const FTaFeiGameplayEffectContext*>(EffectContextHandle.Get()))
	{
			return TaFeiGameplayEffectContext->IsBlockedHit();
	}
	
	return false;
}

bool UTaFeiAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FTaFeiGameplayEffectContext* TaFeiGameplayEffectContext = static_cast<const FTaFeiGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return TaFeiGameplayEffectContext->IsCriticalHit();
	}
	
	return false;
}

void UTaFeiAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FTaFeiGameplayEffectContext* TaFeiGameplayEffectContext = static_cast<FTaFeiGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TaFeiGameplayEffectContext->SetIsBlockedHit(bInIsBlockedHit);	
	}
}

void UTaFeiAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if (FTaFeiGameplayEffectContext* TaFeiGameplayEffectContext = static_cast<FTaFeiGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TaFeiGameplayEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

bool UTaFeiAbilitySystemLibrary::IsPerfectDodge(const FGameplayEffectContextHandle& EffectContextHandle)
{
	// 获取基础 Context
	const FGameplayEffectContext* BaseContext = EffectContextHandle.Get();

	// 只有当指针有效，且类型完全匹配时才进行逻辑判断
	if (BaseContext && BaseContext->GetScriptStruct() == FTaFeiGameplayEffectContext::StaticStruct())
	{
		// 既然已经验证了结构体类型，这里的 static_cast 就是绝对安全的
		const FTaFeiGameplayEffectContext* TaFeiContext = static_cast<const FTaFeiGameplayEffectContext*>(BaseContext);
		return TaFeiContext->IsPerfectDodge();
	}

	//默认不触发完美闪避
	return false;
}

void UTaFeiAbilitySystemLibrary::SetIsPerfectDodge(
	FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsPerfectDodge)
{
	if (FTaFeiGameplayEffectContext* TaFeiGameplayEffectContext =
		static_cast<FTaFeiGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		TaFeiGameplayEffectContext->SetIsPerfectDodge(bInIsPerfectDodge);
	}
}

// 阵营判断：依赖 GameplayTags 甚至普通的 ActorTags (这里使用标准的 ActorTags 示例)
bool UTaFeiAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	if (!FirstActor || !SecondActor) return false;

	// 只要给玩家的蓝图里添加 "Player" 的 Tag，敌人的蓝图里添加 "Enemy" 的 Tag 即可
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;

	return !bFriends; // 不是朋友就是敌人
}

// AOE 圆柱体/球体检测
void UTaFeiAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		// 检测所有动态物体
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		
		for (FOverlapResult& Overlap : Overlaps)
		{
			// 过滤条件：1. 实现了战斗接口 2. 没死 (使用你的接口写法，比如 ITaFeiCombatInterface)
			if (Overlap.GetActor()->Implements<UTaFeiCombatInterface>())
			{
				// 这里假设你在接口里写了 IsDead 函数。如果你还没写，这行逻辑可以先注释掉，只收集 Actor
				 if (!ITaFeiCombatInterface::Execute_IsDead(Overlap.GetActor()))
				 {
					OutOverlappingActors.AddUnique(Overlap.GetActor());
				 }
			}
		}
	}
}