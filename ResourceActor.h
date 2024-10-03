#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceActor.generated.h"

UCLASS()
class GAMEANIMATIONSAMPLE_API AResourceActor : public AActor
{
    GENERATED_BODY()

public:
    AResourceActor();

    /** The type of resource this actor represents */
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Resource")
    FName ResourceType;

    /** The amount of resource */
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Resource")
    int32 Amount;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
