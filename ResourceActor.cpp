#include "ResourceActor.h"
#include "Net/UnrealNetwork.h"

AResourceActor::AResourceActor()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
}

void AResourceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AResourceActor, ResourceType);
    DOREPLIFETIME(AResourceActor, Amount);
}
