#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeometryScript.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API UGeometryScript : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Adds a window to the primitive */
    UFUNCTION(BlueprintCallable, Category = "Building|Modification")
    static void AddWindow(class ABuildingPrimitive* TargetPrimitive, FVector2D Position, FVector2D Size);

    /** Adds a door to the primitive */
    UFUNCTION(BlueprintCallable, Category = "Building|Modification")
    static void AddDoor(class ABuildingPrimitive* TargetPrimitive, FVector2D Position, FVector2D Size);
};
