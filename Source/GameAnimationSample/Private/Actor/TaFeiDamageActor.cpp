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

void ATaFeiDamageActor::BeginPlay()
{
    Super::BeginPlay();
}

void ATaFeiDamageActor::OnOverlap(AActor* TargetActor)
{
    ApplyDamageToTarget(TargetActor);
}

void ATaFeiDamageActor::OnEndOverlap(AActor* TargetActor)
{
    if (!bIsInfinite) return;

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    if (!TargetASC)
    {
        if (APawn* Pawn = Cast<APawn>(TargetActor))
        {
            if (APlayerState* PS = Pawn->GetPlayerState())
            {
                TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
            }
        }
    }

    if (!TargetASC) return;

    TArray<FActiveGameplayEffectHandle> HandlesToRemove;

    for (auto& Pair : ActiveDamageEffects)
    {
        if (Pair.Value == TargetASC)
        {
            TargetASC->RemoveActiveGameplayEffect(Pair.Key);
            HandlesToRemove.Add(Pair.Key);
        }
    }

    for (auto& Handle : HandlesToRemove)
    {
        ActiveDamageEffects.Remove(Handle);
    }
}

void ATaFeiDamageActor::ApplyDamageToTarget(AActor* TargetActor)
{
    if (!TargetActor || !DamageEffectClass || !DamageTypeTag.IsValid()) return;

    if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyDamageToEnemies) return;

    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    if (!TargetASC)
    {
        if (APawn* Pawn = Cast<APawn>(TargetActor))
        {
            if (APlayerState* PS = Pawn->GetPlayerState())
            {
                TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PS);
            }
        }
    }

    if (!TargetASC) return;

    // 防止重复叠加 Infinite
    if (bIsInfinite)
    {
        for (auto& Pair : ActiveDamageEffects)
        {
            if (Pair.Value == TargetASC)
            {
                return;
            }
        }
    }

    // Context
    FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
    ContextHandle.AddSourceObject(this);

    // 可扩展：PerfectDodge 标记
    FTaFeiGameplayEffectContext* TaFeiContext =
        static_cast<FTaFeiGameplayEffectContext*>(ContextHandle.Get());

    if (TaFeiContext)
    {
        TaFeiContext->SetIsPerfectDodge(false);
    }

    // Spec
    FGameplayEffectSpecHandle SpecHandle =
        TargetASC->MakeOutgoingSpec(DamageEffectClass, TrapLevel, ContextHandle);

    if (SpecHandle.IsValid())
    {
        //  核心：ExecCalc 输入
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
            SpecHandle,
            DamageTypeTag,
            BaseDamage
        );

        FActiveGameplayEffectHandle Handle =
            TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        // 记录 Infinite
        if (bIsInfinite)
        {
            ActiveDamageEffects.Add(Handle, TargetASC);
        }
    }
}