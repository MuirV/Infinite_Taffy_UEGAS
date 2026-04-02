// Copyright Daisy

#include "AbilitySystem/TaFeiAbilitySystemLibrary.h"

#include "AbilitySystem/TaFeiAttributeSet.h"
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
				// if (!ITaFeiCombatInterface::Execute_IsDead(Overlap.GetActor()))
				// {
					OutOverlappingActors.AddUnique(Overlap.GetActor());
				// }
			}
		}
	}
}