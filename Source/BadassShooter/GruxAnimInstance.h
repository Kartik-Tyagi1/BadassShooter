// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GruxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BADASSSHOOTER_API UGruxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/* Tick of the AnimInstance Class */
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemy* Enemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;
};
