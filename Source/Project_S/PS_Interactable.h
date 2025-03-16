#pragma once

#include "Project_S.h"
#include "UObject/Interface.h"
#include "PS_Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UPS_Interactable : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_S_API IPS_Interactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction", meta = (DisplayName = "Interact"))
	void Interact(class APS_Character* CharacterInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction", meta = (DisplayName = "Can Interact"))
	bool CanInteract(APS_Character* CharacterInstigator) const;
};