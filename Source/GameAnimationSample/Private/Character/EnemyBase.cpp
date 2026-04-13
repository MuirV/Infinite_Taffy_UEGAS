// Copyright Daisy


#include "Character/EnemyBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"
#include "AbilitySystem/TaFeiAbilitySystemComponent.h"
#include "AbilitySystem/TaFeiAttributeSet.h"
#include "TaFeiGameplayTags.h" 
#include "AbilitySystem/TaFeiAbilitySystemLibrary.h"
#include "AI/TaFeiAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "UI/Widget/TaFeiUserWidget.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	
	PrimaryActorTick.bCanEverTick = false; 
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->bUseControllerDesiredRotation = false; 
	bUseControllerRotationYaw = false;
	
	TaFeiAbilitySystemComponent = CreateDefaultSubobject<UTaFeiAbilitySystemComponent>("AbilitySystemComponent");
	TaFeiAbilitySystemComponent->SetIsReplicated(true);
	
	
	TaFeiAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	TaFeiAttributeSet = CreateDefaultSubobject<UTaFeiAttributeSet>("AttributeSet");


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

	
	if (MontageTag.MatchesTagExact(FTaFeiGameplayTags::Get().Abilities_Attack_ComboLMB))
	{
		return GetMesh()->GetSocketLocation(WeaponSocketName);
	}
	
	
	return GetMesh()->GetSocketLocation(WeaponSocketName);
}

ETaFeiCharacterClass AEnemyBase::GetCharacterClass_Implementation()
{
	
	return CharacterClass;
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
	
	InitAbilityActorInfo();

	UE_LOG(LogTemp, Error, TEXT("Enemy BeginPlay Class = %d"), (int32)CharacterClass);
	
	// 给血条 UI 设置控制器 
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
		// 受击 Tag 是注册过的，FName("Effects.HitReact")
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
	
	if (!HasAuthority()) return; //PossessedBy本来就只在服务器运行，加上这个判断略显冗余

	TaFeiAIController = Cast<ATaFeiAIController>(NewController); 
	if (TaFeiAIController && BehaviorTree)
	{
		// 初始化黑板并运行行为树
		TaFeiAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		TaFeiAIController->RunBehaviorTree(BehaviorTree);
		
		TaFeiAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	}
}

void AEnemyBase::InitAbilityActorInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Init GAS: %s"), *GetName());

	
	TaFeiAbilitySystemComponent->InitAbilityActorInfo(this, this);
	TaFeiAbilitySystemComponent->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		
		//   ASC 里的函数
		TaFeiAbilitySystemComponent->AddStartupAbilitiesFromData(CharacterData, CharacterClass, Level);
	}
}

void AEnemyBase::InitializeDefaultAttributes() const
{
	//这里初始化了才能让敌人获取DefaultAttributes（初始生命值等）
	UTaFeiAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, TaFeiAbilitySystemComponent);
}

UAnimMontage* AEnemyBase::GetHitReactMontage_Implementation()
{
	
	return HitReactMontage; //重写...并返回HitReactMontage，自由在相应角色里面配置
}

void AEnemyBase::PerformAttack()
{
	TArray<FTaFeiTaggedMontage> AttackMontages = ITaFeiCombatInterface::Execute_GetAttackMontages(this);

	UE_LOG(LogTemp, Warning, TEXT("AI Log: GetAttackMontages Count = %d"), AttackMontages.Num());

	if (AttackMontages.Num() == 0) 
	{
		
		return;
	}

	FTaFeiTaggedMontage Selected = GetRandomTaggedMontageFromArray(AttackMontages);
   
	
	UE_LOG(LogTemp, Warning, TEXT("AI Log: Sending Gameplay Event Tag = %s"), *Selected.MontageTag.ToString());

	FGameplayEventData Payload;
	Payload.EventTag = Selected.MontageTag;
	Payload.Instigator = this;
    
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Selected.MontageTag, Payload);
}

FTaFeiTaggedMontage AEnemyBase::GetRandomTaggedMontageFromArray(const TArray<FTaFeiTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() > 0)
	{
		const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);
		return TaggedMontages[Selection];
	}
	return FTaFeiTaggedMontage(); 
}

void AEnemyBase::HitReact_Implementation()
{
	
	
	
}

void AEnemyBase::Die_Implementation()
{
	// 留给蓝图实现：布娃娃系统、掉落物、死亡特效等
	SetLifeSpan(LifeSpan);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}