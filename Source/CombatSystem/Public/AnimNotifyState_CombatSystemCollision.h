#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CombatSystemCollisionShapeInfo.h"
#include "GameplayEffect.h"
#include "AnimNotifyState_CombatSystemCollision.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class COMBATSYSTEM_API UAnimNotifyState_CombatSystemCollision : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_CombatSystemCollision();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	void AddShapeType(ECombatSystemCollisionShapeType CollisionType);

	// Used when creating phantom components for use when editing this anim notify in editor
	void CreateUniqueShapeComponents();
	
	// Used when destroying all phantom components
	void DestroyAllShapeComponents();

	void ClearCollisionShapeInfoArray();

protected:
	USceneComponent* CreateShapeComponent(FCombatSystemCollisionShapeInfo& CollisionInfo, AActor* Owner) const;

	bool IsRunningInEditor(const UWorld* World);

	// Gameplay effect to apply to self when we cause a collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CombatSystem")
	TSubclassOf<UGameplayEffect> GrantGameplayEffectToSelf;

	// Gameplay effect to apply to target when we cause a collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CombatSystem")
	TSubclassOf<UGameplayEffect> GrantGameplayEffectToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CombatSystem")
	TArray<FCombatSystemCollisionShapeInfo> CollisionShapeInfos;

	int LastKnownShapeInfosNum;

	UPROPERTY()
	AActor* CurrentOwner;
	
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilityComponent;;
	
	const FName CreatedComponentBaseTag;

	UPROPERTY(EditAnywhere, Category="PointDamage")
	bool bApplyPointDamageToTarget;
	UPROPERTY(EditAnywhere, Category="PointDamage | Data", meta = (EditCondition = "bApplyPointDamageToTarget", EditConditionHides))
	float BaseDamage;
	UPROPERTY(EditAnywhere, Category="PointDamage | Data", meta = (EditCondition = "bApplyPointDamageToTarget", EditConditionHides))
	TSubclassOf<UDamageType> DamageTypeClass;

	TArray<AActor*> ActorsWeHaveOverlape+
};