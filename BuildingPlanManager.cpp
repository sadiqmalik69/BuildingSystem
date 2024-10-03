#include "BuildingPlanManager.h"
#include "BuildingPrimitive.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h" // Include for UUserWidget


ABuildingPlanManager::ABuildingPlanManager()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    bAlwaysRelevant = true;
}

void ABuildingPlanManager::BeginPlay()
{
    Super::BeginPlay();

    PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void ABuildingPlanManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsBuildingPlanActive && HolographicPrimitive)
    {
        UpdateHolographicPreview();
    }
}

void ABuildingPlanManager::ActivateBuildingPlan()
{
    bIsBuildingPlanActive = true;

    // Create and display the building plan UI widget
    if (BuildingPlanWidgetClass && !BuildingPlanWidgetInstance)
    {
        BuildingPlanWidgetInstance = CreateWidget<UUserWidget>(PlayerController, BuildingPlanWidgetClass);
        if (BuildingPlanWidgetInstance)
        {
            BuildingPlanWidgetInstance->AddToViewport();
        }
    }
}

void ABuildingPlanManager::SelectBuildingPrimitive(TSubclassOf<ABuildingPrimitive> PrimitiveClass)
{
    if (HasAuthority())
    {
        SelectedPrimitiveClass = PrimitiveClass;
        SpawnHolographicPreview();
    }
    else
    {
        Server_SelectBuildingPrimitive(PrimitiveClass);
    }

    // Close the building plan UI widget after selection
    if (BuildingPlanWidgetInstance)
    {
        BuildingPlanWidgetInstance->RemoveFromParent();
        BuildingPlanWidgetInstance = nullptr;
    }
}

bool ABuildingPlanManager::Server_SelectBuildingPrimitive_Validate(TSubclassOf<ABuildingPrimitive> PrimitiveClass)
{
    return true;
}

void ABuildingPlanManager::Server_SelectBuildingPrimitive_Implementation(TSubclassOf<ABuildingPrimitive> PrimitiveClass)
{
    SelectedPrimitiveClass = PrimitiveClass;
    SpawnHolographicPreview();
}

void ABuildingPlanManager::SpawnHolographicPreview()
{
    if (HolographicPrimitive)
    {
        HolographicPrimitive->Destroy();
        HolographicPrimitive = nullptr;
    }

    if (SelectedPrimitiveClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        HolographicPrimitive = GetWorld()->SpawnActor<ABuildingPrimitive>(SelectedPrimitiveClass, SpawnParams);
        HolographicPrimitive->SetHolographicMaterial();
    }
}

void ABuildingPlanManager::DestroyHolographicPreview()
{
    if (HolographicPrimitive)
    {
        HolographicPrimitive->Destroy();
        HolographicPrimitive = nullptr;
    }
}

void ABuildingPlanManager::PlacePrimitive()
{
    if (HasAuthority())
    {
        if (IsPlacementValid() && SelectedPrimitiveClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            FVector Location = HolographicPrimitive->GetActorLocation();
            FRotator Rotation = HolographicPrimitive->GetActorRotation();

            // Spawn the actual building primitive
            ABuildingPrimitive* PlacedPrimitive = GetWorld()->SpawnActor<ABuildingPrimitive>(SelectedPrimitiveClass, Location, Rotation, SpawnParams);
            PlacedPrimitive->SetReplicates(true);

            // Reset holographic preview
            DestroyHolographicPreview();
        }
    }
    else
    {
        Server_PlacePrimitive();
    }
}

bool ABuildingPlanManager::Server_PlacePrimitive_Validate()
{
    return true;
}

void ABuildingPlanManager::Server_PlacePrimitive_Implementation()
{
    if (IsPlacementValid() && SelectedPrimitiveClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        FVector Location = HolographicPrimitive->GetActorLocation();
        FRotator Rotation = HolographicPrimitive->GetActorRotation();

        // Spawn the actual building primitive
        ABuildingPrimitive* PlacedPrimitive = GetWorld()->SpawnActor<ABuildingPrimitive>(SelectedPrimitiveClass, Location, Rotation, SpawnParams);
        PlacedPrimitive->SetReplicates(true);

        // Reset holographic preview
        DestroyHolographicPreview();
    }
}

void ABuildingPlanManager::UpdateHolographicPreview()
{
    if (!PlayerController || !HolographicPrimitive) return;

    FVector WorldLocation;
    FVector WorldDirection;
    if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        FVector Start = WorldLocation;
        FVector End = Start + (WorldDirection * 10000.0f);

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        Params.AddIgnoredActor(HolographicPrimitive);

        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
        {
            FVector HitLocation = HitResult.Location;
            HolographicPrimitive->SetActorLocation(HitLocation);
        }
    }
}

bool ABuildingPlanManager::IsPlacementValid() const
{
    // Implement logic to check for valid placement (e.g., not colliding with other objects)
    return true;
}

void ABuildingPlanManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuildingPlanManager, SelectedPrimitiveClass);
    DOREPLIFETIME(ABuildingPlanManager, bIsBuildingPlanActive);
}
