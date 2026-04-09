// Copyright Daisy

#include "Actor/TaFeiDamageActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "TaFeiAbilityTypes.h"
#include "GameFramework/PlayerState.h"

ATaFeiDamageActor::ATaFeiDamageActor()
{
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void ATaFeiDamageActor::OnOverlap(AActor* TargetActor)
{
    ApplyDamageToTarget(TargetActor);
}

void ATaFeiDamageActor::ApplyDamageToTarget(AActor* TargetActor)
{
    // 安全检查
    if (!TargetActor || !DamageEffectClass || !DamageTypeTag.IsValid()) return;
    
    // 阵营检测
    if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyDamageToEnemies) return;

    // 尝试获取目标的 ASC (保留了你的兜底逻辑)
    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (!TargetASC) 
    {
        if (APawn* TargetPawn = Cast<APawn>(TargetActor))
        {
            if (APlayerState* PS = TargetPawn->GetPlayerState())
            {
                TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
            }
        }
    }
    
    if (!TargetASC) return;

    // 创建 Context
    FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
    ContextHandle.AddSourceObject(this); // 将陷阱本身设为伤害来源
    
    FTaFeiGameplayEffectContext* TaFeiContext =static_cast<FTaFeiGameplayEffectContext*>(ContextHandle.Get());

    if (TaFeiContext)
    {
        TaFeiContext->SetIsPerfectDodge(false); // 初始化
    }

    // 创建 Spec
    // 创建 Spec (使用配置的 TrapLevel)
    FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, TrapLevel, ContextHandle);

    if (SpecHandle.IsValid())
    {
        // =========================================================================================
        // 核心区别：将基础伤害和 Tag 打包进 Spec 的 SetByCaller 里
        // 这样 ExecCalc_Damage 里面的 Spec.GetSetByCallerMagnitude 就能完美提取到这个数值了。
        // =========================================================================================
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypeTag, BaseDamage);

        //  应用效果
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        // 判断是否销毁
        if (bDestroyOnDamage)
        {
            Destroy();
        }
    }
}