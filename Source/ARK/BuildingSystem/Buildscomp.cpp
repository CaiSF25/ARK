#include "Buildscomp.h"

UBuildscomp::UBuildscomp()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UBuildscomp::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void UBuildscomp::ClientLaunchBuildMode_Implementation(int32 StructureID)
{
}


void UBuildscomp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

