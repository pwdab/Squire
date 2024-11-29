#include "PS_EmotionComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UPS_EmotionComponent::UPS_EmotionComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // Tick 활성화
}

// Called when the game starts
void UPS_EmotionComponent::BeginPlay()
{
    Super::BeginPlay();

    // 초기 위치 저장
    InitialLocation = GetOwner()->GetActorLocation();

    // Static Mesh Component 가져오기
    UStaticMeshComponent* MeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
    if (MeshComponent)
    {
        // Dynamic Material Instance 생성
        DynamicMaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
        if (DynamicMaterialInstance)
        {
            // 초기 Opacity 설정 (0으로 설정하여 투명하게 시작)
            DynamicMaterialInstance->SetScalarParameterValue(FName("Opacity"), 0.0f);
        }
    }

    // Fade In 시작
    StartFadeIn();

    // 1초 후 Fade Out 시작
    GetWorld()->GetTimerManager().SetTimer(
        DestructionTimerHandle,
        this,
        &UPS_EmotionComponent::StartFadeOut,
        1.0f,
        false
    );
}

// Called every frame
void UPS_EmotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Fade In 진행
    if (bIsFadingIn)
    {
        FadeInProgress += DeltaTime / FadeInDuration;
        if (FadeInProgress >= 1.0f)
        {
            FadeInProgress = 1.0f;
            bIsFadingIn = false; // Fade In 종료
        }
        UpdateFadeIn(FadeInProgress);
    }

    // Fade Out 진행
    if (bIsFadingOut)
    {
        FadeOutProgress += DeltaTime / FadeOutDuration;
        if (FadeOutProgress >= 1.0f)
        {
            FadeOutProgress = 1.0f;
            bIsFadingOut = false; // Fade Out 종료
            DestroySelf();        // 오브젝트 제거
        }
        UpdateFadeOut(FadeOutProgress);
    }
}

// Fade In 시작
void UPS_EmotionComponent::StartFadeIn()
{
    bIsFadingIn = true;
    FadeInProgress = 0.0f;
}

// Fade Out 시작
void UPS_EmotionComponent::StartFadeOut()
{
    bIsFadingOut = true;
    FadeOutProgress = 0.0f;
}

// Fade In 업데이트
void UPS_EmotionComponent::UpdateFadeIn(float Progress)
{
    FVector NewLocation = InitialLocation + FVector(0.0f, 0.0f, -30.0f * (1.0f - Progress)); // 아래에서 위로 이동
    GetOwner()->SetActorLocation(NewLocation);

    UpdateOpacity(Progress); // 투명도 업데이트
}

// Fade Out 업데이트
void UPS_EmotionComponent::UpdateFadeOut(float Progress)
{
    FVector NewLocation = InitialLocation + FVector(0.0f, 0.0f, -20.0f * Progress); // 위에서 아래로 이동
    GetOwner()->SetActorLocation(NewLocation);

    UpdateOpacity(1.0f - Progress); // 투명도 업데이트
}

// 투명도 업데이트
void UPS_EmotionComponent::UpdateOpacity(float Opacity)
{
    if (DynamicMaterialInstance)
    {
        DynamicMaterialInstance->SetScalarParameterValue(FName("Opacity"), Opacity);
    }
}

// DestroySelf 함수
void UPS_EmotionComponent::DestroySelf()
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        Owner->Destroy();
    }
}
