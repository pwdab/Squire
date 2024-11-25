// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PS_TimeUtility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_S_API UPS_TimeUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Time")
	static FTimespan ConvertSecondsToTimespan(float Seconds);
};
