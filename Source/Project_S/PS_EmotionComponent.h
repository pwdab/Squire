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
    // Fade 관련 변수
    bool bIsFadingIn = false;
    bool bIsFadingOut = false;
    float FadeInProgress = 0.0f;
    float FadeOutProgress = 0.0f;
    const float FadeInDuration = 0.1f;  // Fade In 시간 (초)
    const float FadeOutDuration = 0.1f; // Fade Out 시간 (초)

    FVector InitialLocation;

    // 타이머 핸들
    FTimerHandle DestructionTimerHandle;

    UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;


    // Fade 관련 함수
    void StartFadeIn();
    void StartFadeOut();
    void UpdateFadeIn(float Progress);
    void UpdateFadeOut(float Progress);
    void UpdateOpacity(float Opacity);

    // 액터 제거
    void DestroySelf();
};