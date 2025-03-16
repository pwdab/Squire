#include "PS_EmotionComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UPS_EmotionComponent::UPS_EmotionComponent()
{
    PrimaryComponentTick.bCanEverTick = true; // Tick Ȱ��ȭ
}

// Called when the game starts
void UPS_EmotionComponent::BeginPlay()
{
    Super::BeginPlay();

    // �ʱ� ��ġ ����
    InitialLocation = GetOwner()->GetActorLocation();

    // Fade In ����
    StartFadeIn();

    // 1�� �� Fade Out ����
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

    // Fade In ����
    if (bIsFadingIn)
    {
        FadeInProgress += DeltaTime / FadeInDuration;
        if (FadeInProgress >= 1.0f)
        {
            FadeInProgress = 1.0f;
            bIsFadingIn = false; // Fade In ����
        }
        UpdateFadeIn(FadeInProgress);
    }

    // Fade Out ����
    if (bIsFadingOut)
    {
        FadeOutProgress += DeltaTime / FadeOutDuration;
        if (FadeOutProgress >= 1.0f)
        {
            FadeOutProgress = 1.0f;
            bIsFadingOut = false; // Fade Out ����
            DestroySelf();        // ������Ʈ ����
        }
        UpdateFadeOut(FadeOutProgress);
    }
}

// Fade In ����
void UPS_EmotionComponent::StartFadeIn()
{
    bIsFadingIn = true;
    FadeInProgress = 0.0f;
}

// Fade Out ����
void UPS_EmotionComponent::StartFadeOut()
{
    bIsFadingOut = true;
    FadeOutProgress = 0.0f;
}

// Fade In ������Ʈ
void UPS_EmotionComponent::UpdateFadeIn(float Progress)
{
    FVector NewLocation = InitialLocation + FVector(0.0f, 0.0f, -30.0f * (1.0f - Progress)); // �Ʒ����� ���� �̵�
    GetOwner()->SetActorLocation(NewLocation);
}

// Fade Out ������Ʈ
void UPS_EmotionComponent::UpdateFadeOut(float Progress)
{
    FVector NewLocation = InitialLocation + FVector(0.0f, 0.0f, -20.0f * Progress); // ������ �Ʒ��� �̵�
    GetOwner()->SetActorLocation(NewLocation);
}

// DestroySelf �Լ�
void UPS_EmotionComponent::DestroySelf()
{
    AActor* Owner = GetOwner();
    if (Owner)
    {
        Owner->Destroy();
    }
}
