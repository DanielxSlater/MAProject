

#include "MyAnimNode.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "Components/SkinnedMeshComponent.h"
#include "Animation/AnimTrace.h"

#define DEFAULT_SOURCEINDEX 0xFF
/////////////////////////////////////////////////////
// FAnimNode_LayeredBoneBlend

void FAnimNode_LayeredBone::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(Context);

	const int NumPoses = BlendPoses.Num();
	checkSlow(BlendWeights.Num() == NumPoses);

	// initialize children
	BasePose.Initialize(Context);

	if (NumPoses > 0)
	{
		for (int32 ChildIndex = 0; ChildIndex < NumPoses; ++ChildIndex)
		{
			BlendPoses[ChildIndex].Initialize(Context);
		}
	}
}

void FAnimNode_LayeredBone::RebuildPerBoneBlendWeights(const USkeleton* InSkeleton)
{
	//GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Yellow, FVector(LayerSetup[0].BranchFilters.Num(), 1000, 0).ToString()); //Wypisz ile jest branch filtrów

	if (InSkeleton)
	{
		if (BlendMode == ELayeredBlendMode::BranchFilter)
		{
			FAnimationRuntime::CreateMaskWeights(PerBoneBlendWeights, LayerSetup, InSkeleton);
		}
		else
		{
			FAnimationRuntime::CreateMaskWeights(PerBoneBlendWeights, BlendMasks, InSkeleton);
		}

		SkeletonGuid = InSkeleton->GetGuid();
		VirtualBoneGuid = InSkeleton->GetVirtualBoneGuid();
	}
}

bool FAnimNode_LayeredBone::ArePerBoneBlendWeightsValid(const USkeleton* InSkeleton) const
{
	return (InSkeleton != nullptr && InSkeleton->GetGuid() == SkeletonGuid && InSkeleton->GetVirtualBoneGuid() == VirtualBoneGuid);
}

void FAnimNode_LayeredBone::UpdateCachedBoneData(const FBoneContainer& RequiredBones, const USkeleton* Skeleton)
{
	if (RequiredBones.GetSerialNumber() == RequiredBonesSerialNumber)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Blue, FVector(CurrentBoneBlendWeights.Num(), DesiredBoneBlendWeights.Num(), 0).ToString()); //Print Lenght
	if (!ArePerBoneBlendWeightsValid(Skeleton))
	{
		RebuildPerBoneBlendWeights(Skeleton);
	}

	// build desired bone weights
	const TArray<FBoneIndexType>& RequiredBoneIndices = RequiredBones.GetBoneIndicesArray();
	const int32 NumRequiredBones = RequiredBoneIndices.Num();
	DesiredBoneBlendWeights.SetNumZeroed(NumRequiredBones);
	for (int32 RequiredBoneIndex = 0; RequiredBoneIndex < NumRequiredBones; RequiredBoneIndex++)
	{
		const int32 SkeletonBoneIndex = RequiredBones.GetSkeletonIndex(FCompactPoseBoneIndex(RequiredBoneIndex));
		if (ensure(SkeletonBoneIndex != INDEX_NONE))
		{
			DesiredBoneBlendWeights[RequiredBoneIndex] = PerBoneBlendWeights[SkeletonBoneIndex];
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FVector(RequiredBoneIndex, SkeletonBoneIndex, 0).ToString());
			//ta funkcja wykonuje sie podczas BeginPlay oraz zawiera wszystkie indeksy kosci (podstawowo 79) Required == Skeleton index
		}
	}

	CurrentBoneBlendWeights.Reset(DesiredBoneBlendWeights.Num());
	CurrentBoneBlendWeights.AddZeroed(DesiredBoneBlendWeights.Num());

	//Reinitialize bone blend weights now that we have cleared them
	FAnimationRuntime::UpdateDesiredBoneWeight(DesiredBoneBlendWeights, CurrentBoneBlendWeights, BlendWeights);

	TArray<uint16> const& CurveUIDFinder = RequiredBones.GetUIDToArrayLookupTable();
	const int32 CurveUIDCount = CurveUIDFinder.Num();
	const int32 TotalCount = FBlendedCurve::GetValidElementCount(&CurveUIDFinder);
	if (TotalCount > 0)
	{
		CurvePoseSourceIndices.Reset(TotalCount);
		// initialize with FF - which is default
		CurvePoseSourceIndices.Init(DEFAULT_SOURCEINDEX, TotalCount);

		// now go through point to correct source indices. Curve only picks one source index
		for (int32 UIDIndex = 0; UIDIndex < CurveUIDCount; ++UIDIndex)
		{
			int32 CurrentPoseIndex = CurveUIDFinder[UIDIndex];
			if (CurrentPoseIndex != MAX_uint16)
			{
				SmartName::UID_Type CurveUID = (SmartName::UID_Type)UIDIndex;

				const FCurveMetaData* CurveMetaData = Skeleton->GetCurveMetaData(CurveUID);
				if (CurveMetaData)
				{
					const TArray<FBoneReference>& LinkedBones = CurveMetaData->LinkedBones;
					for (int32 LinkedBoneIndex = 0; LinkedBoneIndex < LinkedBones.Num(); ++LinkedBoneIndex)
					{
						FCompactPoseBoneIndex CompactPoseIndex = LinkedBones[LinkedBoneIndex].GetCompactPoseIndex(RequiredBones);
						if (CompactPoseIndex != INDEX_NONE)
						{
							if (DesiredBoneBlendWeights[CompactPoseIndex.GetInt()].BlendWeight > 0.f)
							{
								CurvePoseSourceIndices[CurrentPoseIndex] = DesiredBoneBlendWeights[CompactPoseIndex.GetInt()].SourceIndex;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		CurvePoseSourceIndices.Reset();
	}

	RequiredBonesSerialNumber = RequiredBones.GetSerialNumber();
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FVector(CurrentBoneBlendWeights.Num(), DesiredBoneBlendWeights.Num(), 0).ToString()); //Print Lenght
}

void FAnimNode_LayeredBone::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
	BasePose.CacheBones(Context);
	int32 NumPoses = BlendPoses.Num();
	for (int32 ChildIndex = 0; ChildIndex < NumPoses; ChildIndex++)
	{
		BlendPoses[ChildIndex].CacheBones(Context);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FVector(LayerSetup.Num(), LayerSetup[0].BranchFilters[0].BlendDepth, 10000).ToString() + (LayerSetup[0].BranchFilters[0].BoneName).ToString()); //Print Lenght
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FVector(CurrentBoneBlendWeights.Num(), DesiredBoneBlendWeights.Num(), RequiredBonesSerialNumber).ToString()); //Print Lenght
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Orange, FVector(CurvePoseSourceIndices.Num(), PerBoneBlendWeights.Num(), RequiredBonesSerialNumber).ToString()); //Print Lenght
	UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton());

	AnimInst = Cast<UAnimInstance>(Context.AnimInstanceProxy->GetAnimInstanceObject());
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FVector(CurrentBoneBlendWeights.Num(), DesiredBoneBlendWeights.Num(), RequiredBonesSerialNumber).ToString()); //Print Lenght
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Yellow, FVector(CurvePoseSourceIndices.Num(), PerBoneBlendWeights.Num(), RequiredBonesSerialNumber).ToString()); //Print Lenght
}

void FAnimNode_LayeredBone::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
	bHasRelevantPoses = false;
	int32 RootMotionBlendPose = -1;
	float RootMotionWeight = 0.f;
	const float RootMotionClearWeight = bBlendRootMotionBasedOnRootBone ? 0.f : 1.f;
	Skel = Context.AnimInstanceProxy->GetSkeleton();



	int ii = 0;
	FString t;
	for (FPerBoneBlendWeight& BoneWeight : DesiredBoneBlendWeights)
	{

		if (IsValid(AnimInst) == true && BoneWeight.BlendWeight > 0)
		{
			t = AnimInst->GetOwningComponent()->GetBoneName(ii).ToString();
			t = t + TEXT(" = ") + FVector(BoneWeight.SourceIndex, BoneWeight.BlendWeight, 0).ToString();
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, t);
		}
		ii = ii + 1;
	}









	if (IsLODEnabled(Context.AnimInstanceProxy))
	{
		GetEvaluateGraphExposedInputs().Execute(Context);

		for (int32 ChildIndex = 0; ChildIndex < BlendPoses.Num(); ++ChildIndex)
		{
			const float ChildWeight = BlendWeights[ChildIndex];
			if (FAnimWeight::IsRelevant(ChildWeight))
			{
				if (bHasRelevantPoses == false)
				{
					//Update cached data now we know we might be valid
					//UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton());
					//DesiredBoneBlendWeights.SetNum(0);
					// Update weights
					FAnimationRuntime::UpdateDesiredBoneWeight(DesiredBoneBlendWeights, CurrentBoneBlendWeights, BlendWeights);
					bHasRelevantPoses = true;
					//GEngine->AddOnScreenDebugMessage(-1, 0.0, FColor::White, FVector(BlendWeights.Num(), DesiredBoneBlendWeights.Num(), BlendPoses.Num()).ToString()); //Print Lenght

					if (bBlendRootMotionBasedOnRootBone)
					{
						const float NewRootMotionWeight = CurrentBoneBlendWeights[0].BlendWeight;
						if (NewRootMotionWeight > ZERO_ANIMWEIGHT_THRESH)
						{
							RootMotionWeight = NewRootMotionWeight;
							RootMotionBlendPose = CurrentBoneBlendWeights[0].SourceIndex;
						}
					}
				}

				const float ThisPoseRootMotionWeight = (ChildIndex == RootMotionBlendPose) ? RootMotionWeight : RootMotionClearWeight;
				BlendPoses[ChildIndex].Update(Context.FractionalWeightAndRootMotion(ChildWeight, ThisPoseRootMotionWeight));
				//GEngine->AddOnScreenDebugMessage(-1, 0.0, FColor::White, FVector(ChildWeight, ThisPoseRootMotionWeight, 100000).ToString()); //Print Lenght
			}
		}
	}
	else
	{
		// Clear BlendWeights if disabled by LODThreshold.
		BlendWeights.Init(0.f, BlendWeights.Num());
	}


	// initialize children
	const float BaseRootMotionWeight = 1.f - RootMotionWeight;

	if (BaseRootMotionWeight < ZERO_ANIMWEIGHT_THRESH)
	{
		BasePose.Update(Context.FractionalWeightAndRootMotion(1.f, BaseRootMotionWeight));
	}
	else
	{
		BasePose.Update(Context);
	}

	TRACE_ANIM_NODE_VALUE(Context, TEXT("Num Poses"), BlendPoses.Num());
}

void FAnimNode_LayeredBone::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	ANIM_MT_SCOPE_CYCLE_COUNTER(BlendPosesInGraph, !IsInGameThread());

	const int NumPoses = BlendPoses.Num();
	if ((NumPoses == 0) || !bHasRelevantPoses)
	{
		BasePose.Evaluate(Output);
	}
	else
	{
		FPoseContext BasePoseContext(Output);

		// evaluate children
		BasePose.Evaluate(BasePoseContext);

		TArray<FCompactPose> TargetBlendPoses;
		TargetBlendPoses.SetNum(NumPoses);

		TArray<FBlendedCurve> TargetBlendCurves;
		TargetBlendCurves.SetNum(NumPoses);

		TArray<UE::Anim::FStackAttributeContainer> TargetBlendAttributes;
		TargetBlendAttributes.SetNum(NumPoses);

		for (int32 ChildIndex = 0; ChildIndex < NumPoses; ++ChildIndex)
		{
			if (FAnimWeight::IsRelevant(BlendWeights[ChildIndex]))
			{
				FPoseContext CurrentPoseContext(Output);
				BlendPoses[ChildIndex].Evaluate(CurrentPoseContext);

				TargetBlendPoses[ChildIndex].MoveBonesFrom(CurrentPoseContext.Pose);
				TargetBlendCurves[ChildIndex].MoveFrom(CurrentPoseContext.Curve);
				TargetBlendAttributes[ChildIndex].MoveFrom(CurrentPoseContext.CustomAttributes);
			}
			else
			{
				TargetBlendPoses[ChildIndex].ResetToRefPose(BasePoseContext.Pose.GetBoneContainer());
				TargetBlendCurves[ChildIndex].InitFrom(Output.Curve);
			}
		}
		FName CurveName;
		SmartName::UID_Type NameUID = Skel->GetUIDByName(USkeleton::AnimCurveMappingName, CurveName);


		// filter to make sure it only includes curves that is linked to the correct bone filter
		TArray<uint16> const* CurveUIDFinder = Output.Curve.UIDToArrayIndexLUT;
		const int32 TotalCount = Output.Curve.NumValidCurveCount;
		// now go through point to correct source indices. Curve only picks one source index
		for (int32 UIDIndex = 0; UIDIndex < CurveUIDFinder->Num(); ++UIDIndex)
		{
			int32 CurvePoseIndex = Output.Curve.GetArrayIndexByUID(UIDIndex);
			if (IsValid(Skel) == true)
			{
				CurveName = TEXT("Layering_Arm_R_Add");
				NameUID = Skel->GetUIDByName(USkeleton::AnimCurveMappingName, CurveName);
				//GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FVector(CurvePoseIndex, BasePoseContext.Curve.Get(UIDIndex), 0).ToString());
			}

			if (CurvePoseSourceIndices.IsValidIndex(CurvePoseIndex))
			{
				int32 SourceIndex = CurvePoseSourceIndices[CurvePoseIndex];
				if (SourceIndex != DEFAULT_SOURCEINDEX)
				{
					// if source index is set, clear base pose curve value
					BasePoseContext.Curve.Set(UIDIndex, 0.f);
					GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FVector(CurvePoseIndex, BasePoseContext.Curve.Get(UIDIndex), 0).ToString());
					for (int32 ChildIndex = 0; ChildIndex < NumPoses; ++ChildIndex)
					{
						if (SourceIndex != ChildIndex)
						{
							// if not source, clear it
							TargetBlendCurves[ChildIndex].Set(UIDIndex, 0.f);
						}
					}
				}
			}
		}

		FAnimationRuntime::EBlendPosesPerBoneFilterFlags BlendFlags = FAnimationRuntime::EBlendPosesPerBoneFilterFlags::None;
		if (bMeshSpaceRotationBlend)
		{
			BlendFlags |= FAnimationRuntime::EBlendPosesPerBoneFilterFlags::MeshSpaceRotation;
		}
		if (bMeshSpaceScaleBlend)
		{
			BlendFlags |= FAnimationRuntime::EBlendPosesPerBoneFilterFlags::MeshSpaceScale;
		}
		int ii = 0;
		FString t;
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, FVector(CurrentBoneBlendWeights.Num(), RequiredBonesSerialNumber, TargetBlendCurves.Num()).ToString());
		for (FPerBoneBlendWeight& BoneWeight : CurrentBoneBlendWeights)
		{

			if (IsValid(AnimInst) == true && BoneWeight.BlendWeight > 0 && 1==0)
			{
				t = AnimInst->GetOwningComponent()->GetBoneName(ii).ToString();
				t = t + TEXT(" = ") + FVector(BoneWeight.SourceIndex, BoneWeight.BlendWeight, 0).ToString();
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Cyan, t);
			}
			ii = ii + 1;
		}
		
		FAnimationPoseData AnimationPoseData(Output);
		FAnimationRuntime::BlendPosesPerBoneFilter(BasePoseContext.Pose, TargetBlendPoses, BasePoseContext.Curve, TargetBlendCurves, BasePoseContext.CustomAttributes, TargetBlendAttributes, AnimationPoseData, CurrentBoneBlendWeights, BlendFlags, CurveBlendOption);
	}
}


void FAnimNode_LayeredBone::GatherDebugData(FNodeDebugData& DebugData)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(GatherDebugData)
	const int NumPoses = BlendPoses.Num();

	FString DebugLine = DebugData.GetNodeName(this);
	DebugLine += FString::Printf(TEXT("(Num Poses: %i)"), NumPoses);
	DebugData.AddDebugItem(DebugLine);

	BasePose.GatherDebugData(DebugData.BranchFlow(1.f));

	for (int32 ChildIndex = 0; ChildIndex < NumPoses; ++ChildIndex)
	{
		BlendPoses[ChildIndex].GatherDebugData(DebugData.BranchFlow(BlendWeights[ChildIndex]));
	}
}
