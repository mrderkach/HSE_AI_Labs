// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_3.h"
#include "SimpleAIController.h"
#include "Kismet/KismetMathLibrary.h"

#include "MazeExit.h"

ASimpleAIController::ASimpleAIController()
{
    InitialScale = 100.0f;
    ScaleDecayRate = 0.8f;
    MinAllowedScale = 1.0f;

    bHasDirection = false;
    MovementDecisionPeriod = 1.0f;
    MovementDecisionTimer = 0.0f;
    PreviousLocation = GetCharacterLocation();

    ChooseDirectionProbeCount = 16;
	EPS = 40;
	current_node = -1;
	bMovingToTarget = false;

}

void ASimpleAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ASimpleAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    MovementDecisionTimer += DeltaSeconds;

    if (TryEscape()) {
        return;
    }
	//UE_LOG(LogTemp, Warning, TEXT("HERE"));
	if (!bMovingToTarget) {
		auto new_target = ChooseNewNode();
		PreviousLocation = GetCharacterLocation();
		bMovingToTarget = true;
		SetNewMoveDestination(new_target);
		UE_LOG(LogTemp, Warning, TEXT("Running to %s"), *new_target.ToString());
		
	}
	else {
		float Distance = GetDistanceToDestination(to_visit_coords[current_node]);
		if (bMovingToTarget && Distance <= 120.0f) {
			bMovingToTarget = false;
			UE_LOG(LogTemp, Warning, TEXT("Arrived at %s"), *to_visit_coords[current_node].ToString());
			--current_node;
		}
	}
}

bool ASimpleAIController::TryEscape()
{
    auto exitLocations = GetExitLocations();
    auto currentLocation = GetCharacterLocation();
    for (int exitIndex = 0; exitIndex < exitLocations.Num(); ++exitIndex) {
        const auto& location = exitLocations[exitIndex];
        auto distance = (currentLocation - location).Size();
        if (distance < AMazeExit::EscapeRadius) {
            bool bSuccess = Escape(exitIndex);
            if (bSuccess) {
                return true;
            }
        }
    }
    return false;
}

float ASimpleAIController::GetDistanceToDestination(const FVector DestLocation)
{
	auto currentLocation = GetCharacterLocation();
	UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
	return FVector::Dist(DestLocation, currentLocation);

}

float ASimpleAIController::GetDirectionScale(FVector direction)
{
    auto currentLocation = GetCharacterLocation();
    float scale = InitialScale;
	while (!CheckVisibility(currentLocation + (scale+0.05) * direction)) {
        scale *= ScaleDecayRate;
    }
    return scale;
}

FVector ASimpleAIController::MoveInDirection(FVector direction)
{
    auto currentLocation = GetCharacterLocation();
    float scale = GetDirectionScale(direction);
    if (scale < MinAllowedScale) {
        bHasDirection = false;
		UE_LOG(LogTemp, Warning, TEXT("MoveInDirectionError"));
		return currentLocation;
    } else {
		//PreviousLocation = currentLocation;
        return currentLocation + scale * direction;
    }
}

FVector ASimpleAIController::GetAngleDirection(float angle)
{
    FVector direction;
    FMath::SinCos(&direction.X, &direction.Y, angle);
    direction.Z = 0;
    return direction;
}

FVector ASimpleAIController::GetRandomDirection()
{
	UE_LOG(LogTemp, Warning, TEXT("Choosing Random direction"));
    float angle = FMath::DegreesToRadians(RandomStream.RandRange(0.0f, 360.0f));
    return GetAngleDirection(angle);
}

bool ASimpleAIController::Inside(std::vector <FVector> arr, FVector vect) {

	for (int i = 0; i < arr.size(); ++i) {
		if ((arr[i] - vect).Size() < EPS) {
			return true;
		}
	}
	return false;
}

FVector ASimpleAIController::ChooseNewNode()
{
    auto currentLocation = GetCharacterLocation();

	if (Inside(visited, currentLocation)) {
		if (current_node >= 0) {
			//UE_LOG(LogTemp, Warning, TEXT("HERE"));
			return to_visit_coords[current_node];
		}

		UE_LOG(LogTemp, Warning, TEXT("ChooseNewNodeError"));
		return currentLocation;
	} else {
		//UE_LOG(LogTemp, Warning, TEXT("THERE"));
		visited.push_back(currentLocation);

		auto deltaPosition = PreviousLocation - currentLocation;
		deltaPosition /= deltaPosition.Size();

		for (float angle123 = 0.0f; angle123 <= 360.0f; angle123 += 360.0f / ChooseDirectionProbeCount) {
			auto direction = GetAngleDirection(FMath::DegreesToRadians(angle123));

			if (GetDirectionScale(direction) < MinAllowedScale) {
				continue;
			}

			auto new_node = MoveInDirection(direction);
			if (Inside(visited, new_node) || Inside(to_visit_coords, new_node)) {
				continue;
			}
			float angleDiff = acosf(FVector::DotProduct(deltaPosition, direction));
			// Checking Dot Product and Cross Product
			if (FVector::DotProduct(deltaPosition, direction) > 0 &&
				abs(FMath::Sin(FMath::DegreesToRadians(angleDiff)) * deltaPosition.Size() * direction.Size()) < 0.3) {
				continue;
			}
			if (to_visit_coords.size() == current_node + 1) {
				to_visit_coords.push_back(new_node);
			}
			else {
				to_visit_coords[current_node + 1] = new_node;
			}
			UE_LOG(LogTemp, Warning, TEXT("New node %s"), *new_node.ToString());
			current_node += 1;
		}
	}
	return to_visit_coords[current_node];
}
