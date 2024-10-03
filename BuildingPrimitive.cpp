#include "BuildingPrimitive.h"
#include "Net/UnrealNetwork.h"

ABuildingPrimitive::ABuildingPrimitive()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    bReplicates = true;
    bAlwaysRelevant = true;

    bIsBuilt = false;
}

void ABuildingPrimitive::BeginPlay()
{
    Super::BeginPlay();

    if (!bIsBuilt)
    {
        SetHolographicMaterial();
    }
}

void ABuildingPrimitive::SetHolographicMaterial()
{
    if (HolographicMaterial && MeshComponent)
    {
        MeshComponent->SetMaterial(0, HolographicMaterial);
    }
}

bool ABuildingPrimitive::Server_AddResource_Validate(FName ResourceType, int32 Amount)
{
    return true;
}

void ABuildingPrimitive::Server_AddResource_Implementation(FName ResourceType, int32 Amount)
{
    int32 CurrentAmount = GetCurrentResourceAmount(ResourceType);
    int32 RequiredAmount = GetRequiredResourceAmount(ResourceType);

    int32 NewAmount = CurrentAmount + Amount;
    NewAmount = FMath::Min(NewAmount, RequiredAmount);

    SetCurrentResourceAmount(ResourceType, NewAmount);

    UpdateMaterial();

    // Check if fully built
    bIsBuilt = IsBuilt();
    if (bIsBuilt)
    {
        // Set default material
        if (DefaultMaterial && MeshComponent)
        {
            MeshComponent->SetMaterial(0, DefaultMaterial);
        }
    }
}

void ABuildingPrimitive::Server_ModifyPrimitive_Implementation(FName ModificationType)
{
    // Implement logic to modify the primitive
}

bool ABuildingPrimitive::Server_ModifyPrimitive_Validate(FName ModificationType)
{
    return true;
}

bool ABuildingPrimitive::IsBuilt() const
{
    for (const FResourceAmount& RequiredResource : RequiredResources)
    {
        FName ResourceType = RequiredResource.ResourceType;
        int32 RequiredAmount = RequiredResource.Amount;
        int32 CurrentAmount = GetCurrentResourceAmount(ResourceType);

        if (CurrentAmount < RequiredAmount)
        {
            return false;
        }
    }
    return true;
}

void ABuildingPrimitive::UpdateMaterial()
{
    // Optionally implement logic to update the material based on build progress
}

void ABuildingPrimitive::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABuildingPrimitive, CurrentResources);
    DOREPLIFETIME(ABuildingPrimitive, bIsBuilt);
}

void ABuildingPrimitive::OnRep_CurrentResources()
{
    UpdateMaterial();
}

void ABuildingPrimitive::OnRep_IsBuilt()
{
    if (bIsBuilt && DefaultMaterial && MeshComponent)
    {
        MeshComponent->SetMaterial(0, DefaultMaterial);
    }
}

int32 ABuildingPrimitive::GetCurrentResourceAmount(FName ResourceType) const
{
    for (const FResourceAmount& Resource : CurrentResources)
    {
        if (Resource.ResourceType == ResourceType)
        {
            return Resource.Amount;
        }
    }
    return 0;
}

void ABuildingPrimitive::SetCurrentResourceAmount(FName ResourceType, int32 Amount)
{
    for (FResourceAmount& Resource : CurrentResources)
    {
        if (Resource.ResourceType == ResourceType)
        {
            Resource.Amount = Amount;
            return;
        }
    }
    // If not found, add a new entry
    CurrentResources.Add(FResourceAmount(ResourceType, Amount));
}

int32 ABuildingPrimitive::GetRequiredResourceAmount(FName ResourceType) const
{
    for (const FResourceAmount& Resource : RequiredResources)
    {
        if (Resource.ResourceType == ResourceType)
        {
            return Resource.Amount;
        }
    }
    return 0;
}
