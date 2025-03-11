#pragma once

#include "Project_S.h"
#include "Engine/DataTable.h"
#include "PS_Words.generated.h"

USTRUCT(BlueprintType)
struct PROJECT_S_API FPS_Words : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 MapNum;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Difficulty;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Word;
};