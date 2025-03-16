#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PS_EmotionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_S_API UPS_EmotionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UPS_EmotionComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // Fade ���� ����
    bool bIsFadingIn = false;
    bool bIsFadingOut = false;
    float FadeInProgress = 0.0f;
    float FadeOutProgress = 0.0f;
    const float FadeInDuration = 0.1f;  // Fade In �ð� (��)
    const float FadeOutDuration = 0.1f; // Fade Out �ð� (��)

    FVector InitialLocation;

    // Ÿ�̸� �ڵ�
    FTimerHandle DestructionTimerHandle;

    UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;


    // Fade ���� �Լ�
    void StartFadeIn();
    void StartFadeOut();
    void UpdateFadeIn(float Progress);
    void UpdateFadeOut(float Progress);
    void UpdateOpacity(float Opacity);

    // ���� ����
    void DestroySelf();
};