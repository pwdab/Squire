// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_TimeUtility.h"

FTimespan UPS_TimeUtility::ConvertSecondsToTimespan(float Seconds)
{
    return FTimespan::FromSeconds(Seconds);
}