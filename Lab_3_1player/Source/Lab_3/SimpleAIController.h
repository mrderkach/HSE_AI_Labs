// Fill out your copyright notice in the Description page of Project Settings.

#include <vector>

#pragma once

#include "BaseAIController.h"
#include "SimpleAIController.generated.h"

/**
 *
 */
UCLASS()
class LAB_3_API ASimpleAIController : public ABaseAIController
{
    GENERATED_BODY()

public:
    ASimpleAIController();

    void BeginPlay() override;

    void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = Movement)
    float InitialScale;

    UPROPERTY(EditAnywhere, Category = Movement)
    float ScaleDecayRate;

    UPROPERTY(EditAnywhere, Category = Movement)
    float MinAllowedScale;

    UPROPERTY(EditAnywhere, Category = Movement)
    float MovementDecisionPeriod;

    UPROPERTY(EditAnywhere, Category = Movement)
    float ChooseDirectionProbeCount;

private:
    FVector Direction;
    bool bHasDirection;

    FVector PreviousLocation;

    float MovementDecisionTimer;

    FRandomStream RandomStream;

	int current_node;
	float EPS;
	std::vector <FVector> to_visit_coords;

	std::vector <FVector> visited;
	bool bMovingToTarget;
	FVector last_goal;

	bool Inside(std::vector <FVector> arr, FVector vect);

    FVector MoveInDirection(FVector direction);

    float GetDirectionScale(FVector direction);

	float GetDistanceToDestination(const FVector DestLocation);

    FVector GetAngleDirection(float angle);

    FVector GetRandomDirection();

    FVector ChooseNewNode();

    bool TryEscape();
};
