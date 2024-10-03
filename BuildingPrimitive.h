#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPrimitive.generated.h"

USTRUCT(BlueprintType)
struct FResourceAmount
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FName ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resource")
    int32 Amount;

    FResourceAmount()
        : ResourceType(NAME_None), Amount(0)
    {
    }

    FResourceAmount(FName InResourceType, int32 InAmount)
        : ResourceType(InResourceType), Amount(InAmount)
    {
    }
};

UCLASS()
class GAMEANIMATIONSAMPLE_API ABuildingPrimitive : public AActor
{
    GENERATED_BODY()

public:
    ABuildingPrimitive();

    /** Sets the material to a holographic (transparent) one */
    UFUNCTION()
    void SetHolographicMaterial();

    /** Modifies the primitive (e.g., adds a door or window) */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_ModifyPrimitive(FName ModificationType);

    /** Adds resources to the primitive */
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_AddResource(FName ResourceType, int32 Amount);

    /** Checks if the primitive is fully built */
    UFUNCTION(BlueprintCallable, Category = "Building")
    bool IsBuilt() const;

protected:
    virtual void BeginPlay() override;

    /** Updates the primitive's material based on build progress */
    void UpdateMaterial();

    /** Handles replication of variables */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Called when CurrentResources is replicated */
    UFUNCTION()
    void OnRep_CurrentResources();

    /** Called when bIsBuilt is replicated */
    UFUNCTION()
    void OnRep_IsBuilt();

    /** The static mesh component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* MeshComponent;

    /** Required resources to build this primitive */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
    TArray<FResourceAmount> RequiredResources;

    /** Current resources applied to this primitive */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentResources)
    TArray<FResourceAmount> CurrentResources;

    /** Whether the primitive is fully built */
    UPROPERTY(ReplicatedUsing = OnRep_IsBuilt)
    bool bIsBuilt;

private:
    /** The default material for built primitives */
    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* DefaultMaterial;

    /** The holographic material for preview */
    UPROPERTY(EditDefaultsOnly)
    UMaterialInterface* HolographicMaterial;

    /** Helper functions to manage resources */
    int32 GetCurrentResourceAmount(FName ResourceType) const;
    void SetCurrentResourceAmount(FName ResourceType, int32 Amount);
    int32 GetRequiredResourceAmount(FName ResourceType) const;
};
