#include "AnimNotifyState_CombatSystemCollision.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CombatSystemCollisionShapeInfo.h"
#include "Animation/AnimNotifyLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "AnimNotifyState_CombatSystemCollision"

UAnimNotifyState_CombatSystemCollision::UAnimNotifyState_CombatSystemCollision()
	: CreatedComponentBaseTag(TEXT("CombatSystemCollInfo"))
	, OwnerAbilityComponent(nullptr)
	, bApplyPointDamageToTarget(false)
	, BaseDamage(1.0f)
{
#ifdef WITH_EDITOR
	LastKnownShapeInfosNum = CollisionShapeInfos.Num();
	DestroyAllShapeComponents();
#endif
}

void UAnimNotifyState_CombatSystemCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                         float TotalDuration,
                                                         const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CurrentOwner = MeshComp->GetOwner();

	// Lets get our GAS component if we have one
	if (CurrentOwner != nullptr)
	{
		const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(CurrentOwner);
		if (AbilitySystemInterface != nullptr)
		{
			OwnerAbilityComponent = AbilitySystemInterface->GetAbilitySystemComponent();
		}
	}

#ifdef WITH_EDITOR
	if (IsRunningInEditor(MeshComp->GetWorld()))
	{
		CreateUniqueShapeComponents();
	}
#endif
}

void UAnimNotifyState_CombatSystemCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                        float FrameDeltaTime,
                                                        const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

#ifdef WITH_EDITOR
	// Dont bother with the collisions checks when editing this in the animation editor
	if (IsRunningInEditor(MeshComp->GetWorld()))
	{
		return;
	}
#endif

	if (!IsValid(CurrentOwner) || !IsValid(OwnerAbilityComponent))
	{
		return;
	}

	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("CombatSystem.DrawDebugShapes")); 
	const bool bDrawDebugShapes = CVar->GetInt() == 0 ? false : true;
	
	for (const FCombatSystemCollisionShapeInfo& ShapeInfo : CollisionShapeInfos)
	{
		FCollisionShape CollShape;
		
		const FTransform ActorTransform = CurrentOwner->GetActorTransform();
		// Need to manually offset this transform else ACharacter anim montages wont be aligned
		const FTransform OffsetTransform = FTransform(FRotator(0.0f, -90.0f, 0.0f), FVector(0.0f)); // TODO: Might cause weird results, do more tests
		const FTransform WorldTransform = ShapeInfo.Transform * (OffsetTransform * ActorTransform);
		const FVector3f Scale = FVector3f(WorldTransform.GetScale3D());
		
		FVector Loc = WorldTransform.GetLocation();
		Loc += -CurrentOwner->GetActorUpVector() * 88.0f; // TODO: Character capsule half height animation editor has root at feet, in-game this is not the case. Replace magic value

		switch (ShapeInfo.ShapeType) {
		case ECombatSystemCollisionShapeType::Box:
			{
				CollShape.SetBox(FVector3f(32.0f) * Scale);

				if (bDrawDebugShapes)
				{
					DrawDebugBox(CurrentOwner->GetWorld(), Loc, CollShape.GetExtent(), WorldTransform.GetRotation(), FColor::Purple, false, 0.5f);
				}
			}
			break;
		case ECombatSystemCollisionShapeType::Sphere:
			{
				CollShape.SetSphere(32.0f * Scale.GetMin());

				if (bDrawDebugShapes)
				{
					DrawDebugSphere(CurrentOwner->GetWorld(), Loc, CollShape.GetSphereRadius(), 8, FColor::Purple, false, 0.5f);
				}
			}
			break;
		case ECombatSystemCollisionShapeType::Capsule:
			{
				CollShape.SetCapsule(FVector3f(22.0f) * Scale);

				if (bDrawDebugShapes)
				{
					DrawDebugCapsule(CurrentOwner->GetWorld(), Loc, CollShape.GetCapsuleHalfHeight(), CollShape.GetCapsuleRadius(), WorldTransform.GetRotation(), FColor::Purple, false, 0.5f);
				}
			}
			break;
		default: ;
		}
		
		FCollisionObjectQueryParams ObjectParams;
		TArray<FHitResult> HitResults;
		CurrentOwner->GetWorld()->SweepMultiByObjectType(HitResults, Loc, Loc, WorldTransform.GetRotation(), ObjectParams, CollShape);

		// Lets do some GAS
		for (const FHitResult& HitResult : HitResults)
		{
			const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(HitResult.GetActor());
			if (AbilitySystemInterface != nullptr)
			{
				if (IsValid(GrantGameplayEffectToSelf))
				{
					const UGameplayEffect* EffectForSelf = GrantGameplayEffectToSelf->GetDefaultObject<UGameplayEffect>();
					OwnerAbilityComponent->ApplyGameplayEffectToSelf(EffectForSelf, 1, FGameplayEffectContextHandle());
				}
				
				UAbilitySystemComponent* TargetAbilityComponent = AbilitySystemInterface->GetAbilitySystemComponent();
				if (TargetAbilityComponent != nullptr)
				{
					if (IsValid(GrantGameplayEffectToTarget))
					{
						UGameplayEffect* EffectForTarget = GrantGameplayEffectToTarget->GetDefaultObject<UGameplayEffect>();
						OwnerAbilityComponent->ApplyGameplayEffectToTarget(EffectForTarget, TargetAbilityComponent, 1);	
					}
				}
			}

			if (bApplyPointDamageToTarget && OwnerAbilityComponent->AbilityActorInfo.IsValid())
			{
				auto PlayerController = OwnerAbilityComponent->AbilityActorInfo->PlayerController;
				if (PlayerController.IsValid())
				{
					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), BaseDamage, HitResult.ImpactNormal, HitResult, PlayerController.Get(), PlayerController.Get(), DamageTypeClass);
				}
			}
		}
	}
}

void UAnimNotifyState_CombatSystemCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                       const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

#ifdef WITH_EDITOR
	if (IsRunningInEditor(MeshComp->GetWorld()))
	{
		if (UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference))
		{
			DestroyAllShapeComponents();
		}
	}
#endif
}

void UAnimNotifyState_CombatSystemCollision::CreateUniqueShapeComponents()
{
	// If last known shape info num is not what we expected, expect a shape info got manually deleted and reset all shape componenets accordingly
	if (LastKnownShapeInfosNum != CollisionShapeInfos.Num())
	{
		LastKnownShapeInfosNum = CollisionShapeInfos.Num();
		DestroyAllShapeComponents();
	}
	
	for (int i = 0; i < CollisionShapeInfos.Num(); i++)
	{
		FCombatSystemCollisionShapeInfo& CollisionInfo = CollisionShapeInfos[i];
		
		// If phantom components does not exist, we create it
		if (!IsValid(CollisionInfo.PhantomSceneComponent))
		{
			CollisionInfo.PhantomSceneComponent = CreateShapeComponent(CollisionInfo, CurrentOwner);

			// Register our OnTransformUpdated lambda
			CollisionInfo.PhantomSceneComponent->TransformUpdated.AddLambda(
				[this, &CollisionInfo](USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
			{
				// If our component in question is active, pass transform to our Info
				if (InRootComponent->IsActive())
				{
					CollisionInfo.Transform = InRootComponent->GetRelativeTransform();
				}
				// If we arent active, we destroy and remove ourselves
				else
				{
					CollisionInfo.PhantomSceneComponent->TransformUpdated.Clear();
					CollisionInfo.PhantomSceneComponent->DestroyComponent();
					CollisionInfo.PhantomSceneComponent = nullptr;;
					CollisionShapeInfos.RemoveSingle(CollisionInfo);
				}
			});

			CollisionInfo.PhantomSceneComponent->Activate();
		}
	}
}

void UAnimNotifyState_CombatSystemCollision::AddShapeType(ECombatSystemCollisionShapeType CollisionType)
{
	FCombatSystemCollisionShapeInfo ShapeInfo;
	ShapeInfo.ShapeType = CollisionType;
	
	CollisionShapeInfos.Add(ShapeInfo);
	LastKnownShapeInfosNum = CollisionShapeInfos.Num();
}

USceneComponent* UAnimNotifyState_CombatSystemCollision::CreateShapeComponent(
	FCombatSystemCollisionShapeInfo& CollisionInfo, AActor* Owner) const
{
	TSubclassOf<UShapeComponent> SubclassToUse;
	switch (CollisionInfo.ShapeType)
	{
	case ECombatSystemCollisionShapeType::Box:
		SubclassToUse = UBoxComponent::StaticClass();
		break;
	case ECombatSystemCollisionShapeType::Sphere:
		SubclassToUse = USphereComponent::StaticClass();
		break;
	case ECombatSystemCollisionShapeType::Capsule:
		SubclassToUse = UCapsuleComponent::StaticClass();
		break;
	default:
		return nullptr;
	}

	USceneComponent* GeneratedComponent = Cast<USceneComponent>(
		Owner->AddComponentByClass(SubclassToUse, false, CollisionInfo.Transform, false));
	if (GeneratedComponent != nullptr)
	{
		GeneratedComponent->ComponentTags.Add(CreatedComponentBaseTag);
		GeneratedComponent->ComponentTags.Add(CollisionInfo.GetUniqueTag());
	}

	return GeneratedComponent;
}

void UAnimNotifyState_CombatSystemCollision::DestroyAllShapeComponents()
{
	for (FCombatSystemCollisionShapeInfo& ShapeInfo : CollisionShapeInfos)
	{
		if (ShapeInfo.PhantomSceneComponent != nullptr && IsValid(ShapeInfo.PhantomSceneComponent))
		{
			ShapeInfo.PhantomSceneComponent->DestroyComponent();
		}

		ShapeInfo.PhantomSceneComponent = nullptr;
	}

	// Lets make double sure we've destroyed all relevant shape components
	if (IsValid(CurrentOwner))
	{
		TArray<UActorComponent*> Components = CurrentOwner->GetComponentsByTag(UShapeComponent::StaticClass(), CreatedComponentBaseTag);
		for (UActorComponent* Component : Components)
		{
			Component->DestroyComponent();
		}
	}
}

void UAnimNotifyState_CombatSystemCollision::ClearCollisionShapeInfoArray()
{
	DestroyAllShapeComponents();
	
	CollisionShapeInfos.Empty();
}

bool UAnimNotifyState_CombatSystemCollision::IsRunningInEditor(const UWorld* World)
{
	return UGameplayStatics::GetGameInstance(World) == nullptr;
}
