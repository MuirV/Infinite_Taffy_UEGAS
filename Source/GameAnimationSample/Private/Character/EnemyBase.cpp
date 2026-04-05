// Copyright Daisy


#include "Character/EnemyBase.h"

#include "AIController.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "TaFeiGameplayTags.h" 
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/TaFeiUserWidget.h"

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
	TaFeiAbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	TaFeiAbilitySystemComponent->SetIsReplicated(true);
	
	// AI 敌人强烈推荐用 Minimal 模式，节省服务器带宽
	TaFeiAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	TaFeiAttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");

	// 运动设置
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 血条 UI
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	
}

UAbilitySystemComponent* AEnemyBase::GetAbilitySystemComponent() const
{
	return TaFeiAbilitySystemComponent;
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

ETaFeiCharacterClass AEnemyBase::GetCharacterClass_Implementation()
{
	return ITaFeiCombatInterface::GetCharacterClass_Implementation();
}

void AEnemyBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	
	if(TaFeiAIController && TaFeiAIController->GetBlackboardComponent())
	{
		TaFeiAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	// 初始化 GAS 信息
	InitAbilityActorInfo();

	// 给血条 UI 设置控制器 (敌人自己就是控制器)
	if (UTaFeiUserWidget* TaFeiUserWidget = Cast<UTaFeiUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		TaFeiUserWidget->SetWidgetController(this);
	}

	// 绑定属性变化监听
	if (TaFeiAbilitySystemComponent && TaFeiAttributeSet)
	{
		TaFeiAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
		);
		TaFeiAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TaFeiAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
		);

		// 监听受击 Tag
		// 注意：请确保你的受击 Tag 是确切注册过的，比如 FName("Effects.HitReact")
		FGameplayTag HitReactTag = FGameplayTag::RequestGameplayTag(FName("Effects.HitReact"));
		TaFeiAbilitySystemComponent->RegisterGameplayTagEvent(HitReactTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AEnemyBase::HitReactTagChanged
		);
       
		// 广播初始值
		OnHealthChanged.Broadcast(TaFeiAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(TaFeiAttributeSet->GetMaxHealth());
	}
}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// AI 只有在服务器端被 Controller 附身时才初始化 GAS
	if (HasAuthority())
	{
		InitializeGAS();
	}

	// if (!HasAuthority()) return;
	//
	// TaFeiAIController = Cast<AAIController>(NewController);
	// if (TaFeiAIController && BehaviorTree)
	// {
	// 	TaFeiAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	// 	TaFeiAIController->RunBehaviorTree(BehaviorTree);
	// 	TaFeiAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	// 	// 判断是否是远程攻击者，写入黑板
	// 	TaFeiAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ETaFeiCharacterClass::Warrior);
	// }
}

void AEnemyBase::InitAbilityActorInfo()
{
	// 对于敌人：Owner 和 Avatar 都是自己！
	TaFeiAbilitySystemComponent->InitAbilityActorInfo(this, this);
	TaFeiAbilitySystemComponent->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		
		// ★ 直接复用我们上次写在 ASC 里的完美函数！极度优雅！
		TaFeiAbilitySystemComponent->AddStartupAbilitiesFromData(CharacterData, CharacterClass, Level);
	}
}

void AEnemyBase::InitializeDefaultAttributes() const
{
	
}

void AEnemyBase::InitializeGAS()
{
	if (TaFeiAbilitySystemComponent)
	{
		// 对于敌人，灵魂(Owner)和肉体(Avatar)都是它自己
		TaFeiAbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		// 应用初始属性 GE
		if (HasAuthority() && DefaultAttributes)
		{
			FGameplayEffectContextHandle ContextHandle = TaFeiAbilitySystemComponent->MakeEffectContext();
			ContextHandle.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = TaFeiAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, ITaFeiCombatInterface::Execute_GetPlayerLevel(this), ContextHandle);
			
			if (SpecHandle.IsValid())
			{
				TaFeiAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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