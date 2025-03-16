// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Project_S��� Ŀ���� �α� ī�װ� ����
DECLARE_LOG_CATEGORY_EXTERN(Project_S, Log, All);

// FUNCTION �̸��� LINE�� ���
#define PS_LOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
// Verbosity(�α� ���� : Log, Warning, Error, ... )�� ���ڷ� �޾� Verbosity�� �´� PS_LOG_CALLINFO�� ���
#define PS_LOG_S(Verbosity) UE_LOG(Project_S, Verbosity, TEXT("%s"), *PS_LOG_CALLINFO)
//Verbosity�� ���ڷ� �޾� Verbosity�� �´� PS_LOG_CALLINFO�� ���Ҿ� Format�� ���
#define PS_LOG(Verbosity, Format, ...) UE_LOG(Project_S, Verbosity, TEXT("%s"), *PS_LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
// Expr�� False�̸� Error�� ���
#define PS_CHECK(Expr, ...) { if (!(Expr)) { PS_LOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }