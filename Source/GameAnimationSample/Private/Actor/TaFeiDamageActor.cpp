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
    ApplyBurning(TargetActor);
}

void ATaFeiDamageActor::OnEndOverlap(AActor* TargetActor)
{
    RemoveBurning(TargetActor);
}

void ATaFeiDamageActor::ApplyBurning(AActor* TargetActor)
{
    if (!TargetActor || !DamageEffectClass || !DamageTypeTag.IsValid()) return;

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

    // 避免重复上同一个 GE
    if (ActiveEffects.Contains(TargetASC)) return;

    FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
    ContextHandle.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, TrapLevel, ContextHandle);

    if (SpecHandle.IsValid())
    {
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
            SpecHandle, DamageTypeTag, BaseDamage);

        FActiveGameplayEffectHandle Handle =
            TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        //  记录
        ActiveEffects.Add(TargetASC, Handle);
    }
}

void ATaFeiDamageActor::RemoveBurning(AActor* TargetActor)
{
    if (!TargetActor) return;

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

    if (ActiveEffects.Contains(TargetASC))
    {
        TargetASC->RemoveActiveGameplayEffect(ActiveEffects[TargetASC]);
        ActiveEffects.Remove(TargetASC);
    }
}
