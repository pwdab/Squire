// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Project_S라는 커스텀 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(Project_S, Log, All);

// FUNCTION 이름과 LINE을 출력
#define PS_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
// Verbosity(로깅 수준 : Log, Warning, Error, ... )를 인자로 받아 Verbosity에 맞는 PS_LOG_CALLINFO를 출력
#define PS_LOG_S(Verbosity) UE_LOG(Project_S, Verbosity, TEXT("%s"), *PS_LOG_CALLINFO)
//Verbosity를 인자로 받아 Verbosity에 맞는 PS_LOG_CALLINFO에 더불어 Format도 출력
#define PS_LOG(Verbosity, Format, ...) UE_LOG(Project_S, Verbosity, TEXT("%s"), *PS_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
// Expr이 False이면 Error를 출력
#define PS_CHECK(Expr, ...) { if (!(Expr)) { PS_LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }