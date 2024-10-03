#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPlanManager.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API ABuildingPlanManager : public AActor
{
    GENERATED_BODY()

public:
    ABuildingPlanManager();

    virtual void Tick(float DeltaTime) override;

    /** Activates the building plan UI */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void ActivateBuildingPlan();

    /** Selects a building primitive to place */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void SelectBuildingPrimitive(TSubclassOf<class ABuildingPrimitive> PrimitiveClass);

    /** Places the selected primitive in the world */
    UFUNCTION(BlueprintCallable, Category = "Building")
    void PlacePrimitive();

    /** Widget class to use for the building plan UI */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> BuildingPlanWidgetClass;

protected:
    virtual void BeginPlay() override;

    /** Updates the holographic preview's position and orientation */
    void UpdateHolographicPreview();

    /** Checks if the placement location is valid */
    bool IsPlacementValid() const;

    /** Spawns the holographic preview */
    void SpawnHolographicPreview();

    /** Destroys the holographic preview */
    void DestroyHolographicPreview();

    /** Handles replication of variables */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Server function to select a building primitive */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SelectBuildingPrimitive(TSubclassOf<class ABuildingPrimitive> PrimitiveClass);

    /** Server function to place the primitive */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_PlacePrimitive();

private:
    /** The currently selected building primitive class */
    UPROPERTY(Replicated)
    TSubclassOf<class ABuildingPrimitive> SelectedPrimitiveClass;

    /** The holographic preview of the primitive */
    UPROPERTY()
    ABuildingPrimitive* HolographicPrimitive;

    /** Whether the building plan is active */
    UPROPERTY(Replicated)
    bool bIsBuildingPlanActive;

    /** Player controller reference */
    APlayerController* PlayerController;

    /** Instance of the building plan widget */
    UPROPERTY()
    UUserWidget* BuildingPlanWidgetInstance;
};
