

#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "Engine/DataTable.h"
#include "ALS_StructuresAndEnumsCpp.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class CMC_ActionTypeC : uint8
{
	None,
	ShortMove,
	CornerOuter,
	CornerInner,
	Turn180,
	JumpNextLedge,
	JumpBackToNextLedge,
	ForwardMove,
	PullUpToNarrowFloor,
	DropToNarrowFloor,
	DropFromNarrowFloor,
	JumpToBeamSwinging,
	JumpForwardToBeam,
	StartHoldingLedge
};

USTRUCT(BlueprintType)
struct FCALS_ComponentAndTransform : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transformation")
		FTransform Transform = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transformation")
		UPrimitiveComponent* Component = nullptr;
};

USTRUCT(BlueprintType)
struct FCMC_SingleClimbPointC : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
	bool ValidPoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		FVector Location = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		FVector Normal = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		FTransform ActorTransform = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		UPrimitiveComponent* Component = nullptr;
};

USTRUCT(BlueprintType)
struct FCMC_LedgeC : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		FTransform LeftPoint = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		FTransform RightPoint = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		FTransform Origin = FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ClimbingPoint")
		UPrimitiveComponent* Component = nullptr;
};



class HELPFULFUNCTIONS_API ALS_StructuresAndEnumsCpp
{
public:
	ALS_StructuresAndEnumsCpp();
	~ALS_StructuresAndEnumsCpp();
};
