// Copyright Daisy

#include "Actor/TaFeiEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

// Sets default values
ATaFeiEffectActor::ATaFeiEffectActor()
{
	PrimaryActorTick.bCanEverTick = false; // 场景物品通常不需要 Tick，提升性能
    
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void ATaFeiEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATaFeiEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// 阵营检测：如果碰到的是敌人，且设置为不对敌人生效，则跳过
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
    
	// 尝试获取目标的 ASC (使用 BlueprintLibrary 方式可以兼容任何带 ASC 的 Actor)
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) //PlayerState兜底检测
	{
		if (APawn* TargetPawn = Cast<APawn>(TargetActor))
		{
			if (APlayerState* PS = TargetPawn->GetPlayerState())
			{
				TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
			}
		}
	}
    
	// 如果兜底找完还是没找到，才退出
	if (TargetASC == nullptr) return;
    
	check(GameplayEffectClass);

	// 创建效果上下文 (Context)
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	// 创建效果实例 (Spec)
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	
	// 应用效果
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    
	// 检查该效果是否是“无限(Infinite)”的
	const bool bIsInfinite = EffectSpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite;
	
	// 如果是无限效果，并且设置了离开时移除，就把它存到 Map 里
	if (bIsInfinite && InfiniteEffectRemovalPolicy == ETaFeiEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	//  Bug 修复：只有当配置了允许销毁，并且该效果不是无限效果时，才销毁自身
	// (无限效果不能销毁自身，否则玩家离开时找不到这个 Actor 去触发 EndOverlap 来移除效果)
	if (bDestroyOnEffectApplication && !bIsInfinite)
	{ 
		Destroy();
	}
}

void ATaFeiEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
    
	// 检查配置，如果策略是 ApplyOnOverlap，则应用对应的效果
	if (InstantEffectApplicationPolicy == ETaFeiEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == ETaFeiEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == ETaFeiEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void ATaFeiEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
    
	// 检查配置，如果策略是 ApplyOnEndOverlap，则应用对应的效果
	if (InstantEffectApplicationPolicy == ETaFeiEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == ETaFeiEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == ETaFeiEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	// 核心逻辑：如果玩家离开了区域，移除那些需要被移除的无限效果
	if (InfiniteEffectRemovalPolicy == ETaFeiEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) //PlayerState兜底检查
		{
			if (APawn* TargetPawn = Cast<APawn>(TargetActor))
			{
				if (APlayerState* PS = TargetPawn->GetPlayerState())
				{
					TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
				}
			}
		}
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		
		// 遍历我们存下来的句柄字典，找到属于这个离开的 TargetASC 的所有效果
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				// 移除效果 (参数 1 代表移除 1 层，如果是无限叠加的这里可以改)
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		// 从字典中清理掉已经失效的句柄
		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}