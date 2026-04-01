// Copyright Daisy


#include "Character/EnemyBase.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "TaFeiGameplayTags.h" 
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; 

	// --- 默认网格体与胶囊体设置 ---
	// 让敌人的胶囊体忽略摄像机碰撞，防止视角推近时穿模抖动
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// 调整默认自带 Mesh 的位置和旋转，让它完美贴合胶囊体底盘 (面向X轴正方向)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	// 同样让模型忽略摄像机，但阻挡发射物/能见度（用于你的武器射线检测）
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	// --- 实例化 GAS 组件 ---
	AbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	
	// AI 敌人强烈推荐用 Minimal 模式，节省服务器带宽
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");

}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

int32 AEnemyBase::GetPlayerLevel_Implementation() const
{
	return Level;
}

FVector AEnemyBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (!GetMesh()) return FVector::ZeroVector;

	// 演示逻辑：如果传入的是左键平A，返回武器插槽
	if (MontageTag.MatchesTagExact(FTaFeiGameplayTags::Get().Abilities_Attack_ComboLMB))
	{
		return GetMesh()->GetSocketLocation(WeaponSocketName);
	}
	
	// 默认返回武器插槽
	return GetMesh()->GetSocketLocation(WeaponSocketName);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// AI 只有在服务器端被 Controller 附身时才初始化 GAS
	if (HasAuthority())
	{
		InitializeGAS();
	}
}

void AEnemyBase::InitializeGAS()
{
	if (AbilitySystemComponent)
	{
		// 对于敌人，灵魂(Owner)和肉体(Avatar)都是它自己
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		// 应用初始属性 GE
		if (HasAuthority() && DefaultAttributes)
		{
			FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetPlayerLevel(), ContextHandle);
			
			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void AEnemyBase::HitReact_Implementation()
{
	// 留给蓝图实现：播放受击蒙太奇或特效
}

void AEnemyBase::Die_Implementation()
{
	// 留给蓝图实现：布娃娃系统、掉落物、死亡特效等
}