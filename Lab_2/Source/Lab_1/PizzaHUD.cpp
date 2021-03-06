// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "PizzaHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Lab_1GameMode.h"

#include "Engine/Canvas.h"
#include "Engine/Font.h"

APizzaHUD::APizzaHUD()
{
    // Use the RobotoDistanceField font from the engine.
    static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
    HUDFont = HUDFontOb.Object;
}

void APizzaHUD::DrawHUD()
{
    // Get the screen dimensions.
    FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);

    // Call to the parent versions of DrawHUD.
    Super::DrawHUD();

    auto* MyGameMode = Cast<ALab_1GameMode>(UGameplayStatics::GetGameMode(this));

    double percentile = 0.9;

    TArray<FString> Messages;
    Messages.Add(FString::Printf(TEXT("Pending orders: %d"), MyGameMode->GetPendingPizzaOrderCount()));
    Messages.Add(FString::Printf(TEXT("Delivered orders: %d"), MyGameMode->GetDeliveredPizzaOrderCount()));
    Messages.Add(FString::Printf(TEXT("%d-percentile wait time %.3f"),
        static_cast<int>(100 * percentile),
        MyGameMode->GetDeliveredPizzaPercentileWaitTime(percentile)));

    float yShift = 0;
    for (int i = 0 ; i < Messages.Num(); ++i) {
        DrawText(Messages[i], FColor::White, 30, 30 + yShift, HUDFont);

        FVector2D Size;
        GetTextSize(Messages[i], Size.X, Size.Y, HUDFont);
        yShift += 5 + Size.Y;
    }

    // If the game is over.
    if (MyGameMode->GetCurrentState() == ELab_1PlayState::EGameOver)
    {
        // Create a variable for storing the size of printing Game Over.
        FVector2D GameOverSize;
        GetTextSize(TEXT("GAME OVER"), GameOverSize.X, GameOverSize.Y, HUDFont);
        DrawText(TEXT("GAME OVER"), FColor::White, (ScreenDimensions.X - GameOverSize.X) / 2.0f, (ScreenDimensions.Y - GameOverSize.Y) / 2.0f, HUDFont);
    }
}
