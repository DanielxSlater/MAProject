// Jakub W

#pragma once

#include "CoreMinimal.h"
#include "IWALS_EnumsAndStruct.h"
#include "Animation/AnimInstance.h"
#include "IWALS_AnimInstanceCpp.generated.h"

/**
 * 
 */
UCLASS()
class IWALS_ABILITYSYSTEM_API UIWALS_AnimInstanceCpp : public UAnimInstance
{
	GENERATED_BODY()

	/* Zdefiniuj podstawowe zmienne, które bêd¹ potrzebne dla systemu Overlay States. ¿eby nie odwo³ywaæ siê do klasy ALS_AnimBP przy pomocy Property Access utworzony zosta³ w³aœnie
	Anim Instance. W nim zdefiniowane s¹ potrzebne zmienne, wiêc dziêki temu odwo³ywaæ siê bêdziemy w³aœnie do tej klasy a nie ca³ego ALS_AnimBP */

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		float SecondaryMotionMaskC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		float BlendOverlayWithCoverModeC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		float LandPredictionC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		bool IsHeldArrowC = false;

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		bool AddRecoilImpulseC = false;

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		int OverlayOverrideStateC = 0;

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		FVector2D CoverCrouchWithDirectionC = FVector2D(0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "IWALS Anim Graph", meta = (AllowPrivateAccess = "True"))
		FVector RelativeAccelerationAmoutC = FVector(0, 0, 0);

	//Foots IK Variables

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		float FootLock_L_AlphaC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		float FootLock_R_AlphaC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		float CMC_FootsIK_AlphaC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FVector FootLock_L_LocationC = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FVector FootLock_R_LocationC = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FRotator FootLock_L_RotationC = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FRotator FootLock_R_RotationC = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FVector FootOffset_L_LocationC = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FVector FootOffset_R_LocationC = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FRotator FootOffset_L_RotationC = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FRotator FootOffset_R_RotationC = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Foot IK", meta = (AllowPrivateAccess = "True"))
		FVector PelvisOffsetC = FVector(0, 0, 0);

	//Ragdoll

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Ragdoll", meta = (AllowPrivateAccess = "True"))
		float FlailRateC = 0.0;

	//Aiming

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		FRotator SpineRotationC = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		FRotator SmoothedAimingRotationC = FRotator(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		FVector2D AimingAngleC = FVector2D(0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		FVector2D SmoothedAimingAngleC = FVector2D(0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		float AimSweepTimeC = 0.5;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		float InputYawOffsetTimeC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		float ForwardYawTimeC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		float LeftYawTimeC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		float RightYawTimeC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		FVector AnimPrepertiesCustomC = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Aiming Values", meta = (AllowPrivateAccess = "True"))
		FTransform NeckTransformFromSnapshot = FTransform::Identity;

	//Anim Graph - Grounded

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		HipsDirectionC TrackedHipsDirectionC = HipsDirectionC::F;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		bool ShouldMoveC = false;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		bool PivotC = false;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		bool PivotPlayingC = false;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		bool PlayStopMovementTransitionC = false;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		bool FinishStopTransitionC = false;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float RotateRateC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float RotationScaleC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float DiagonalScaleAmoutC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		FVelocityBlendC VelocityBlend = {};

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		FLeanAmoutC LeanAmountC = {};

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float FYawC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float BYawC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float LYawC = 0.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - Grounded", meta = (AllowPrivateAccess = "True"))
		float RYawC = 0.0;

	//Anim Graph - In Air

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - In Air", meta = (AllowPrivateAccess = "True"))
		bool JumpedC = false;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - In Air", meta = (AllowPrivateAccess = "True"))
		float JumpPlayRateC = 1.0;

	UPROPERTY(BlueprintReadWrite, Category = "Anim Graph - In Air", meta = (AllowPrivateAccess = "True"))
		float FallSpeedC = 0.0;

};
