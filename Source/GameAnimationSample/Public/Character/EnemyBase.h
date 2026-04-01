// Copyright Daisy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/TaFeiCombatInterface.h"
#include "EnemyBase.generated.h"

class UTaFeiAbilitySystemComponent;
class UTaFeiAttributeSet;

UCLASS()
class GAMEANIMATIONSAMPLE_API AEnemyBase : public ACharacter, public IAbilitySystemInterface, public ITaFeiCombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();
	// ~IAbilitySystemInterface 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UTaFeiAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// ~ITaFeiCombatInterface
	virtual int32 GetPlayerLevel_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	// --- GAS 核心组件 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TaFei|GAS")
	TObjectPtr<UTaFeiAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TaFei|GAS")
	TObjectPtr<UTaFeiAttributeSet> AttributeSet;


	// 敌人的初始属性 GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;
	
	// --- 基础属性与经验值 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	int32 XPReward = 50;

	// --- 插槽配置 (Socket) ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	FName WeaponSocketName = FName("WeaponSocket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	FName LeftHandSocketName = FName("LeftHandSocket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TaFei|Combat")
	FName RightHandSocketName = FName("RightHandSocket");

	// --- 韧性与受击系统 ---
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|Combat")
	void HitReact();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TaFei|Combat")
	void Die();
	
private:
	// 辅助函数：初始化 GAS
	void InitializeGAS();
};

