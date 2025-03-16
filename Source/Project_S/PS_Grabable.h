#pragma once

#include "Project_S.h"
#include "UObject/Interface.h"
#include "PS_Grabable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
//UINTERFACE(MinimalAPI, BlueprintType)
class UPS_Grabable : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_S_API IPS_Grabable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab", meta = (DisplayName = "Grab"))
	void Grab(class APS_Character* CharacterInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab", meta = (DisplayName = "Grab"))
	void UnGrab(class APS_Character* CharacterInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab", meta = (DisplayName = "Grab"))
	void Throw(class APS_Character* CharacterInstigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab", meta = (DisplayName = "Can Grab"))
	bool CanGrab(APS_Character* CharacterInstigator) const;
};