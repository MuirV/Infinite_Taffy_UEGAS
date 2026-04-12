// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/TaFeiCombatInterface.h"
#include "EnemyBase.generated.h"

class UTaFeiAbilitySystemComponent;
class UTaFeiAttributeSet;
class UWidgetComponent;
class UBehaviorTree;
class AAIController;
class UCharacterClassInfo; // 你的数据资产类

// 属性变化的多播委托（给蓝图血条用的）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaFeiAttributeChangedSignature, float, NewValue);

UCLASS()
class GAMEANIMATIONSAMPLE_API AEnemyBase : public ACharacter, public IAbilitySystemInterface, public ITaFeiCombatInterface
{
	GENERATED_BODY()

public:
	
	AEnemyBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTaFeiAttributeSet* GetAttributeSet() const { return TaFeiAttributeSet; }

	
	virtual int32 GetPlayerLevel_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual ETaFeiCharacterClass GetCharacterClass_Implementation() override;

	UPROPERTY(BlueprintAssignable)
	FOnTaFeiAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnTaFeiAttributeChangedSignature OnMaxHealthChanged;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 3.5f;
	
protected:
	
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const ;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TaFei|GAS")
	TObjectPtr<UTaFeiAbilitySystemComponent> TaFeiAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TaFei|GAS")
	TObjectPtr<UTaFeiAttributeSet> TaFeiAttributeSet;


	// 敌人的初始属性 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	int32 XPReward = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ETaFeiCharacterClass CharacterClass = ETaFeiCharacterClass::Mob;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	FName WeaponSocketName = FName("WeaponSocket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	FName LeftHandSocketName = FName("LeftHandSocket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	FName RightHandSocketName = FName("RightHandSocket");

	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|Combat")
	void HitReact();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|Combat")
	void Die();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAIController> TaFeiAIController;
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
};

