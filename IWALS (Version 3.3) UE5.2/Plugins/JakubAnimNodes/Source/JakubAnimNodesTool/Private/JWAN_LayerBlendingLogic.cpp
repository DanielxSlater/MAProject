
#include "JWAN_LayerBlendingLogic.h"
#include "AnimationRuntime.h"
#include "Animation/AnimTrace.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimNode_Inertialization.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/Skeleton.h"

#define DEFAULT_SOURCEINDEX 0xFF

void FJWAN_LayerBlendingLogic::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
    //Funkcja inicjuj¹ca noda
    AnimInst = Cast<UAnimInstance>(Context.AnimInstanceProxy->GetAnimInstanceObject());
    BaseLayerInput.Initialize(Context);
    OverlayLayerInput.Initialize(Context);
    BasePosesInput.Initialize(Context);

}

void FJWAN_LayerBlendingLogic::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
    //Funkcja inicjuj¹ca noda
    DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
    BaseLayerInput.CacheBones(Context);
    OverlayLayerInput.CacheBones(Context);
    BasePosesInput.CacheBones(Context);

    TArray<FInputBlendPose> LayerBlendPelvis = CreateLayerBlendValue({ TEXT("Pelvis"),TEXT("thigh_l"),TEXT("thigh_r") }, { 0,-1,-1 });
    TArray<FInputBlendPose> LayerBlendSpine = CreateLayerBlendValue({ TEXT("spine_01") }, { 3 });
    TArray<FInputBlendPose> LayerBlendHead = CreateLayerBlendValue({ TEXT("neck") }, { 0 });
    TArray<FInputBlendPose> LayerBlendArmR = CreateLayerBlendValue({ TEXT("clavicle_r") }, { 1 });
    TArray<FInputBlendPose> LayerBlendArmL = CreateLayerBlendValue({ TEXT("clavicle_l") }, { 1 });
    TArray<FInputBlendPose> LayerBlendHandR = CreateLayerBlendValue({ TEXT("index_01_l"), TEXT("middle_01_l"), TEXT("ring_01_l"), TEXT("pinky_01_l"), TEXT("thumb_01_l"), TEXT("VB LHS_ik_hand_gun") }, { 0,0,0,0,0,0 });
    TArray<FInputBlendPose> LayerBlendHandL = CreateLayerBlendValue({ TEXT("index_01_r"), TEXT("middle_01_r"), TEXT("ring_01_r"), TEXT("pinky_01_r"), TEXT("thumb_01_r"), TEXT("VB RHS_ik_hand_gun") }, { 0,0,0,0,0,0 });
    TArray<FInputBlendPose> LayerBlendCurves = CreateLayerBlendValue({ TEXT("VB Curves") }, { 0 });
    TArray<FInputBlendPose> LayerBlendNone = {};

    //For Blending Legs And Pelvis
    if (CurrentBoneBlendWeights.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), CurrentBoneBlendWeights, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendPelvis, 1);
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending Blended(Legs, Pelvis) And Spine
    if (BoneBlendWeightSpine.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightSpine, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendSpine, 1);
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending Blended(Blended(Legs, Pelvis), Spine) And Head
    if (BoneBlendWeightHead.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightHead, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendHead, 1);
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending Blended(Blended(Blended(Legs, Pelvis), Spine), Head) And Arm R
    if (BoneBlendWeightArmR.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightArmR, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendArmR, 1);
        BoneBlendWeightArmRls = BoneBlendWeightArmR;
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending Blended(Blended(Blended(Blended(Legs, Pelvis), Spine), Head),Arm R) ANd Arm L
    if (BoneBlendWeightArmL.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightArmL, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendArmL, 1);
        BoneBlendWeightArmLls = BoneBlendWeightArmL;
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending Blended(Blended(Blended(Blended(Legs, Pelvis), Spine), Head),Arm R) ANd Arm L
    if (BoneBlendWeightHandR.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightHandR, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendHandR, 1);
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending Blended(Blended(Blended(Blended(Legs, Pelvis), Spine), Head),Arm R) ANd Arm L
    if (BoneBlendWeightHandL.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightHandL, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendHandL, 1);
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending CURVES
    if (BoneBlendWeightCurves.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightCurves, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendCurves, 1);
    }
    RequiredBonesSerialNumber = 0;
    CurvePoseSourceIndices.SetNum(0); DesiredBoneBlendWeights.SetNum(0); PerBoneBlendWeights.SetNum(0);
    //For Blending CURVES
    if (BoneBlendWeightOutput.Num() == 0)
    {
        UpdateCachedBoneData(Context.AnimInstanceProxy->GetRequiredBones(), Context.AnimInstanceProxy->GetSkeleton(), BoneBlendWeightOutput, SkeletonGuid, VirtualBoneGuid,
            RequiredBonesSerialNumber, CurvePoseSourceIndices, LayerBlendNone, 1);
    }
   

}

void FJWAN_LayerBlendingLogic::Update_AnyThread(const FAnimationUpdateContext& Context)
{
    //Funkcja aktualizowana z ka¿d¹ klatk¹
    DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread)
    GetEvaluateGraphExposedInputs().Execute(Context);
    BaseLayerInput.Update(Context);
    OverlayLayerInput.Update(Context);
    BasePosesInput.Update(Context);

    //Update Slots
    SlotUpdate(Context, AllSlotsName.Legs, BaseLayerInput, WeightData);
    SlotUpdate(Context, AllSlotsName.Pelvis, BaseLayerInput, WeightData);
    SlotUpdate(Context, AllSlotsName.Spine, BaseLayerInput, WeightData);
    SlotUpdate(Context, AllSlotsName.Head, BaseLayerInput, WeightData);
    SlotUpdate(Context, AllSlotsName.ArmL, BaseLayerInput, WeightData);
    SlotUpdate(Context, AllSlotsName.ArmR, BaseLayerInput, WeightData);
    TArray<FInputBlendPose> L;
    //UpdateMaskWeightsValue(Context, DesiredBoneBlendWeights, BoneBlendWeightArmL, { 1 }, true, SkeletonGuid, VirtualBoneGuid, RequiredBonesSerialNumber, CurvePoseSourceIndices, L, 1.0);
    //UpdateMaskWeightsValue(Context, DesiredBoneBlendWeights, BoneBlendWeightArmR, { 1 }, true, SkeletonGuid, VirtualBoneGuid, RequiredBonesSerialNumber, CurvePoseSourceIndices, L, 1.0);
    //UpdateMaskWeightsValue(Context, DesiredBoneBlendWeights, BoneBlendWeightHandL, { CurveValue(CurvesName.HandL) }, true, SkeletonGuid, VirtualBoneGuid, RequiredBonesSerialNumber, CurvePoseSourceIndices, L, 1.0);
    //UpdateMaskWeightsValue(Context, DesiredBoneBlendWeights, BoneBlendWeightHandR, { CurveValue(CurvesName.HandR) }, true, SkeletonGuid, VirtualBoneGuid, RequiredBonesSerialNumber, CurvePoseSourceIndices, L, 1.0);
    //UpdateMaskWeightsValue(Context, DesiredBoneBlendWeights, BoneBlendWeightArmLls, { CurveValue(CurvesName.ArmL_LS) }, true, SkeletonGuid, VirtualBoneGuid, RequiredBonesSerialNumber, CurvePoseSourceIndices, L, 1.0);
    //UpdateMaskWeightsValue(Context, DesiredBoneBlendWeights, BoneBlendWeightArmRls, { CurveValue(CurvesName.ArmR_LS) }, true, SkeletonGuid, VirtualBoneGuid, RequiredBonesSerialNumber, CurvePoseSourceIndices, L, 1.0);
}

void FJWAN_LayerBlendingLogic::Evaluate_AnyThread(FPoseContext& Output)
{
    //Funkcja aktualizowana z ka¿d¹ klatk¹
    DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
    ANIM_MT_SCOPE_CYCLE_COUNTER(BlendPosesInGraph, !IsInGameThread());
    //Init Inputs
    FPoseContext PoseBase(Output);
    FPoseContext PoseOverlay(Output);
    FPoseContext PoseDef(Output);
    BaseLayerInput.Evaluate(PoseBase);
    OverlayLayerInput.Evaluate(PoseOverlay);
    BasePosesInput.Evaluate(PoseDef);

    // Make Dynamic Additives Pose
    FPoseContext BaseAdditiveMS(Output);
    FPoseContext BaseAdditiveLSpace(Output);
    //Utworzenie animacji Additive z wejœæ noda
    MakeAdditiveContext(PoseDef, PoseBase, true, BaseAdditiveMS);
    MakeAdditiveContext(PoseDef, PoseBase, false, BaseAdditiveLSpace);

    //Zapis wyników do zmiennych lokalnych dla poszczególnych partii szkieletu
    FPoseContext PoseLegs(Output);
    FPoseContext PosePelvis(Output);
    FPoseContext PoseSpine(Output);
    FPoseContext PoseHead(Output);
    FPoseContext PoseArmL(Output);
    FPoseContext PoseArmR(Output);

    FPoseContext LayerBlendOut1(Output);
    FPoseContext LayerBlendOut2(Output);
    FPoseContext LayerBlendCurvesOut(Output);

    //Zaaplikowanie animacji Additive dla poszczególnych czêsci szkieletu - przestrzeñ szkieletu
    ApplyAdditiveWithSlotEvaluate(PoseLegs, PoseBase, PoseOverlay, BaseAdditiveMS, 1.0, CurveValue(CurvesName.Legs), AllSlotsName.Legs, WeightData, true);
    ApplyAdditiveWithSlotEvaluate(PosePelvis, PoseBase, PoseOverlay, BaseAdditiveMS, 1.0, CurveValue(CurvesName.Pelvis), AllSlotsName.Pelvis, WeightData, true);
    ApplyAdditiveWithSlotEvaluate(PoseSpine, PoseBase, PoseOverlay, BaseAdditiveMS, CurveValue(CurvesName.SpineAdd), CurveValue(CurvesName.Spine), AllSlotsName.Spine, WeightData, true);
    ApplyAdditiveWithSlotEvaluate(PoseHead, PoseBase, PoseOverlay, BaseAdditiveMS, CurveValue(CurvesName.HeadAdd), CurveValue(CurvesName.Head), AllSlotsName.Head, WeightData, true);
    //Zaaplikowanie animacji Additive dla poszczególnych czêsci szkieletu - przestrzeñ lokalna
    ApplyAdditiveWithSlotEvaluate(PoseArmL, PoseBase, PoseOverlay, BaseAdditiveLSpace, CurveValue(CurvesName.ArmL_Add), CurveValue(CurvesName.ArmL), AllSlotsName.ArmL, WeightData, false);
    ApplyAdditiveWithSlotEvaluate(PoseArmR, PoseBase, PoseOverlay, BaseAdditiveLSpace, CurveValue(CurvesName.ArmR_Add), CurveValue(CurvesName.ArmR), AllSlotsName.ArmR, WeightData, false);

    BlendPerBoneEvaluate(LayerBlendOut1, PoseLegs, PosePelvis, true, false, CurrentBoneBlendWeights, CurvePoseSourceIndices, false, 1);
    BlendPerBoneEvaluate(LayerBlendOut2, LayerBlendOut1, PoseSpine, true, false, BoneBlendWeightSpine, CurvePoseSourceIndices, false, 1);
    BlendPerBoneEvaluate(Output, LayerBlendOut2, PoseHead, true, false, BoneBlendWeightHead, CurvePoseSourceIndices, false, 1);
    BlendPerBoneEvaluate(Output, Output, PoseArmL, true, false, BoneBlendWeightArmL, CurvePoseSourceIndices, false, ArmL_MS);
    BlendPerBoneEvaluate(Output, Output, PoseArmL, false, false, BoneBlendWeightArmLls, CurvePoseSourceIndices, false, CurveValue(CurvesName.ArmL_LS));
    BlendPerBoneEvaluate(Output, Output, PoseArmR, true, false, BoneBlendWeightArmR, CurvePoseSourceIndices, false, ArmR_MS);
    BlendPerBoneEvaluate(Output, Output, PoseArmR, false, false, BoneBlendWeightArmRls, CurvePoseSourceIndices, false, CurveValue(CurvesName.ArmR_LS));
    BlendPerBoneEvaluate(Output, Output, PoseOverlay, false, false, BoneBlendWeightHandL, CurvePoseSourceIndices, false, CurveValue(CurvesName.HandL));
    BlendPerBoneEvaluate(Output, Output, PoseOverlay, false, false, BoneBlendWeightHandR, CurvePoseSourceIndices, false, CurveValue(CurvesName.HandR));

    BlendPerBoneEvaluate(LayerBlendCurvesOut, PoseBase, PoseOverlay, false, false, BoneBlendWeightCurves, CurvePoseSourceIndices, true, 1);
    BlendPerBoneEvaluate(Output, Output, LayerBlendCurvesOut, false, false, BoneBlendWeightOutput, CurvePoseSourceIndices, false, 1);

}

void FJWAN_LayerBlendingLogic::GatherDebugData(FNodeDebugData& DebugData)
{
    FString DebugLine = DebugData.GetNodeName(this);


    DebugData.AddDebugItem(DebugLine);

    BaseLayerInput.GatherDebugData(DebugData);
}

void FJWAN_LayerBlendingLogic::BlendTwoPosesContext(FPoseContext& PoseA, FPoseContext& PoseB, float Alpha, FPoseContext& OP)
{
    if (Alpha == 0.0)
    {
        OP = PoseA;
        return;
    }
    else if (Alpha == 1.0)
    {
        OP = PoseB;
        return;
    }
    FAnimationPoseData BlendedAnimationPoseData(OP);
    const FAnimationPoseData AnimationPoseOneData(PoseA);
    const FAnimationPoseData AnimationPoseTwoData(PoseB);
    FAnimationRuntime::BlendTwoPosesTogether(AnimationPoseOneData, AnimationPoseTwoData, Alpha, BlendedAnimationPoseData);
    OP.Curve.Lerp(PoseA.Curve, PoseB.Curve, Alpha);
    return;
}

//MAKE ADDITIVE POSE FROM TWO ANIMATIONS
void FJWAN_LayerBlendingLogic::MakeAdditiveContext(FPoseContext& Base, FPoseContext& Add, bool MeshSpace, FPoseContext& OP)
{
    FPoseContext BaseEvalContext(OP);
    BaseEvalContext = Base;
    OP = Add;
    if (MeshSpace)
    {
        FAnimationRuntime::ConvertPoseToMeshRotation(OP.Pose);
        FAnimationRuntime::ConvertPoseToMeshRotation(BaseEvalContext.Pose);
    }
    FAnimationRuntime::ConvertPoseToAdditive(OP.Pose, BaseEvalContext.Pose);
    OP.Curve.ConvertToAdditive(Base.Curve);
    UE::Anim::Attributes::ConvertToAdditive(OP.CustomAttributes, Base.CustomAttributes);
}

//LAYER BLENDING SINGLE - EVALUATE
void FJWAN_LayerBlendingLogic::ApplyAdditiveWithSlotEvaluate(FPoseContext& OP, FPoseContext& LBase, FPoseContext& LOverlay, FPoseContext& LAdditive, 
    float AddAlpha, float BlendAlpha, FName AnimSlotName, FSlotNodeWeightInfo SlotWeight, bool UseMeshSpace)
{
    FPoseContext OverlayWithSlot(OP);
    OverlayWithSlot = LOverlay;

    if (BlendAlpha == 0.0)
    {
        OP = LBase;
        return;
    }
    float a = 0.0;
    //Próba zaaplikowania slotu
    if (SlotWeight.SlotNodeWeight <= ZERO_ANIMWEIGHT_THRESH)
    {
        OverlayWithSlot = LOverlay;
    }
    else
    {
        //GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FVector(0, 0, 0).ToString());
        const FAnimationPoseData SourcePoseData(LOverlay);
        FAnimationPoseData OutputPoseData(OverlayWithSlot);
        OverlayWithSlot.AnimInstanceProxy->SlotEvaluatePose(AnimSlotName, SourcePoseData, SlotWeight.SourceWeight, OutputPoseData, SlotWeight.SlotNodeWeight, SlotWeight.TotalNodeWeight);
        checkSlow(!OverlayWithSlot.ContainsNaN());
        checkSlow(OverlayWithSlot.IsNormalized());
    }
    //Overlay Slot Powinien teraz zawierac zaaplikowany slot
    //Dodanie Animacji Additive
    if (AddAlpha > 0.0)
    {
        FAnimationPoseData AddBase(OverlayWithSlot);
        FAnimationPoseData AddAdditive(LAdditive);
        a = FMath::Clamp<float>(AddAlpha, 0.f, 1.f);
        if (UseMeshSpace == true)
        { FAnimationRuntime::AccumulateMeshSpaceRotationAdditiveToLocalPose(AddBase, LAdditive, a); }
        else
        { FAnimationRuntime::AccumulateAdditivePose(AddBase, LAdditive, a, AAT_LocalSpaceBase); }
        OverlayWithSlot.Pose.NormalizeRotations();
        //OverlayWithSlot.Curve.Accumulate(AddBase.GetCurve(), 1-a);
        
    }
    if (BlendAlpha >= 1.0)
    {
        OP = OverlayWithSlot;
        return;
    }
    a = FMath::Clamp<float>(BlendAlpha, 0.f, 1.f);
    FAnimationPoseData BlendedAnimationPoseData(OP);
    const FAnimationPoseData AnimationPoseOneData(LBase);
    const FAnimationPoseData AnimationPoseTwoData(OverlayWithSlot); //Ja Pierdole jakieœ 10h aby ostatecznie zamieniæ pozycje zmiennych dla blend                                XD
    FAnimationRuntime::BlendTwoPosesTogether(AnimationPoseTwoData, AnimationPoseOneData, a, BlendedAnimationPoseData);
    OP.Curve.Lerp(LBase.Curve, OverlayWithSlot.Curve, a);
    return;
}

//LAYER BLENDING SINGLE - UPDATE
void FJWAN_LayerBlendingLogic::ApplyAdditiveWithSlotUpdate(FPoseContext& LBase, FPoseContext& LOverlay, FPoseContext& LAdditive, 
    float AddAlpha, float BlendAlpha, FName AnimSlotName)
{

}

void FJWAN_LayerBlendingLogic::BlendPerBoneEvaluate(FPoseContext& OP, FPoseContext& PoseA, FPoseContext& PoseB, bool UseMeshSpaceRotation, bool UseMeshSpaceScaleBlend, 
    TArray<FPerBoneBlendWeight> CurrentBoneBlendW, TArray<uint8> CurvePoseSourceI, bool UseBlendByWeight, float BlendAlpha)
{
    ANIM_MT_SCOPE_CYCLE_COUNTER(BlendPosesInGraph, !IsInGameThread());

    const int NumPoses = 1;
    FPoseContext BasePoseContext(OP);
    BasePoseContext = PoseA;

    TArray<float> BlendWeights = { 1 };

    TArray<FCompactPose> TargetBlendPoses;
    TargetBlendPoses.SetNum(NumPoses);

    TArray<FBlendedCurve> TargetBlendCurves;
    TargetBlendCurves.SetNum(NumPoses);

    TArray<UE::Anim::FStackAttributeContainer> TargetBlendAttributes;
    TargetBlendAttributes.SetNum(NumPoses);

    if (BlendAlpha <= 0.05)
    {
        OP = PoseA;
        return;
    }

    for (int32 ChildIndex = 0; ChildIndex < NumPoses; ++ChildIndex)
    {
        if (FAnimWeight::IsRelevant(BlendWeights[ChildIndex]))
        {
            FPoseContext CurrentPoseContext(PoseB);
            CurrentPoseContext = PoseB;
            //GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, FVector(TargetBlendCurves[0].CurveWeights.Num(), 0, 0).ToString());
            TargetBlendPoses[ChildIndex].MoveBonesFrom(CurrentPoseContext.Pose);
            TargetBlendCurves[ChildIndex].MoveFrom(CurrentPoseContext.Curve);
            TargetBlendAttributes[ChildIndex].MoveFrom(CurrentPoseContext.CustomAttributes);
        }
        else
        {
            TargetBlendPoses[ChildIndex].ResetToRefPose(BasePoseContext.Pose.GetBoneContainer());
            TargetBlendCurves[ChildIndex].InitFrom(OP.Curve); //???
        }
    }

    // filter to make sure it only includes curves that is linked to the correct bone filter
    TArray<uint16> const* CurveUIDFinder = OP.Curve.UIDToArrayIndexLUT;
    const int32 TotalCount = OP.Curve.NumValidCurveCount;
    // now go through point to correct source indices. Curve only picks one source index
    for (int32 UIDIndex = 0; UIDIndex < CurveUIDFinder->Num(); ++UIDIndex)
    {
        int32 CurvePoseIndex = OP.Curve.GetArrayIndexByUID(UIDIndex);
        if (CurvePoseSourceI.IsValidIndex(CurvePoseIndex))
        {
            int32 SourceIndex = CurvePoseSourceI[CurvePoseIndex];
            if (SourceIndex != DEFAULT_SOURCEINDEX)
            {
                // if source index is set, clear base pose curve value
                //BasePoseContext.Curve.Set(UIDIndex, 0.f);
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
    if (UseMeshSpaceRotation)
    {
        BlendFlags |= FAnimationRuntime::EBlendPosesPerBoneFilterFlags::MeshSpaceRotation;
    }
    if (UseMeshSpaceScaleBlend)
    {
        BlendFlags |= FAnimationRuntime::EBlendPosesPerBoneFilterFlags::MeshSpaceScale;
    }
    //Set Curve BlendOption
    TEnumAsByte<enum ECurveBlendOption::Type> CurveBlendOption;
    if (UseBlendByWeight == true)
    { CurveBlendOption = ECurveBlendOption::BlendByWeight; }
    else
    { CurveBlendOption = ECurveBlendOption::Override; }

    //GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FVector(TargetBlendCurves[0].CurveWeights.Num(), 0,0).ToString());
    FAnimationPoseData AnimationPoseData(OP);
    FAnimationRuntime::BlendPosesPerBoneFilter(BasePoseContext.Pose, TargetBlendPoses, BasePoseContext.Curve, TargetBlendCurves, BasePoseContext.CustomAttributes, TargetBlendAttributes, AnimationPoseData, CurrentBoneBlendW, BlendFlags, CurveBlendOption);
    if (BlendAlpha < 0.99)
    {
        FAnimationPoseData AnimationBaseData(BasePoseContext);
        FAnimationRuntime::BlendTwoPosesTogether(AnimationPoseData, AnimationBaseData, BlendAlpha, AnimationPoseData);
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Simple Curve Value From Anim Inst
float FJWAN_LayerBlendingLogic::CurveValue(FName CurveName)
{
    //return 1.0;
    if(IsValid(AnimInst)==true)
    {
        return FMath::Clamp<float>(AnimInst->GetCurveValue(CurveName),0.0,1.0);
    }
    return 0.0f;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ||| L A Y E R   B L E N D   P E R   B O N E |||
void FJWAN_LayerBlendingLogic::UpdateMaskWeightsValue(const FAnimationUpdateContext& C, TArray<FPerBoneBlendWeight>& DBBW, TArray<FPerBoneBlendWeight> CBBW, 
    TArray<float> BlendWeightsPerPose, bool BlendRootMotionBasedOnRoot, FGuid& InSkelGuid, FGuid& InVirtualBonesGuid, uint16& RBSN, TArray<uint8>& CPSI, 
    TArray<FInputBlendPose> InLayerSetup, float BlendAlpha)
{
    bHasRelevantPoses = false;
    int32 RootMotionBlendPose = -1;
    float RootMotionWeight = 0.f;
    const float RootMotionClearWeight = BlendRootMotionBasedOnRoot ? 0.f : 1.f;
    int NumOfPoses = 1;
    int32 ChildIndex = 0;
    
    const float ChildWeight = BlendWeightsPerPose[ChildIndex];
    if (FAnimWeight::IsRelevant(ChildWeight) || 0==0)
    {
        if (bHasRelevantPoses == false)
        {
            // Update cached data now we know we might be valid
            //UpdateCachedBoneData(C.AnimInstanceProxy->GetRequiredBones(), C.AnimInstanceProxy->GetSkeleton(), CBBW, InSkelGuid, InVirtualBonesGuid, RBSN, CPSI, InLayerSetup, BlendAlpha);

            // Update weights
            FAnimationRuntime::UpdateDesiredBoneWeight(CBBW, CBBW, BlendWeightsPerPose);
            //GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FVector(BlendWeightsPerPose[0],0,0).ToString());
            int ii = 0;
            FString t;
            for (FPerBoneBlendWeight& BoneWeight : CBBW)
            {

                if (IsValid(AnimInst) == true && BoneWeight.BlendWeight > 0)
                {
                    t = AnimInst->GetOwningComponent()->GetBoneName(ii).ToString();
                    t = t + TEXT(" = ") + FVector(BoneWeight.SourceIndex, BoneWeight.BlendWeight, 0).ToString();
                    //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, t);
                }
                ii = ii + 1;
            }

            bHasRelevantPoses = true;

            if (BlendRootMotionBasedOnRoot)
            {
                const float NewRootMotionWeight = CBBW[0].BlendWeight;
                if (NewRootMotionWeight > ZERO_ANIMWEIGHT_THRESH)
                {
                    RootMotionWeight = NewRootMotionWeight;
                    RootMotionBlendPose = CBBW[0].SourceIndex;
                }
            }
        }

        const float ThisPoseRootMotionWeight = (ChildIndex == RootMotionBlendPose) ? RootMotionWeight : RootMotionClearWeight;
        //BlendPoses[ChildIndex].Update(C.FractionalWeightAndRootMotion(ChildWeight, ThisPoseRootMotionWeight));
    }
    
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ||| L A Y E R   B L E N D   P E R   B O N E |||  Rebuild cache per bone blend weights from the skeleton
void FJWAN_LayerBlendingLogic::RebuildPerBoneBlendWeights(const USkeleton* InSkeleton, TArray<FPerBoneBlendWeight>& PBBW, TArray<FInputBlendPose> InLayerSetup, FGuid& InSkelGuid, FGuid& InVirtualBonesGuid)
{
    //GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Yellow, FVector(InLayerSetup[0].BranchFilters.Num(), 1000, 0).ToString()); //Wypisz ile jest branch filtrów
    if (InSkeleton)
    {
        FAnimationRuntime::CreateMaskWeights(PBBW, InLayerSetup, InSkeleton);
        InSkelGuid = InSkeleton->GetGuid();
        InVirtualBonesGuid = InSkeleton->GetVirtualBoneGuid();
    }
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ||| L A Y E R   B L E N D   P E R   B O N E |||  Check whether per-bone blend weights are valid according to the skeleton (GUID check)
bool FJWAN_LayerBlendingLogic::ArePerBoneBlendWeightsValid(const USkeleton* InSkeleton, FGuid& InSkelGuid, FGuid& InVirtualBonesGuid)
{
    return (InSkeleton != nullptr && InSkeleton->GetGuid() == InSkelGuid && InSkeleton->GetVirtualBoneGuid() == InVirtualBonesGuid);
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ||| L A Y E R   B L E N D   P E R   B O N E |||  Update Cached Bone Data
void FJWAN_LayerBlendingLogic::UpdateCachedBoneData(const FBoneContainer& RequiredBones, const USkeleton* Skeleton, TArray<FPerBoneBlendWeight>& CurrentBoneBlendOutput, FGuid& InSkelGuid,
    FGuid& InVirtualBonesGuid, uint16& RBSN, TArray<uint8>& CPSI, TArray<FInputBlendPose> InLayerSetup, float BlendAlpha)
{
    if (RequiredBones.GetSerialNumber() == RBSN)
    {
        return;
    }
    //GEngine->AddOnScreenDebugMessage(-1, 4, FColor::White, FVector(CurrentBoneBlendWeights.Num(), DesiredBoneBlendWeights.Num(), 0).ToString()); //Print Lenght
    //if (!ArePerBoneBlendWeightsValid(Skeleton, InSkelGuid, InVirtualBonesGuid))
    //{
        //Nie wiem czy powinienem u¿yc zmiennej PerBoneBlendWeight!!!
        RebuildPerBoneBlendWeights(Skeleton, PerBoneBlendWeights, InLayerSetup, InSkelGuid, InVirtualBonesGuid);
    //}

    // build desired bone weights
    const TArray<FBoneIndexType>& RequiredBoneIndices = RequiredBones.GetBoneIndicesArray();
    const int32 NumRequiredBones = RequiredBoneIndices.Num();
    DesiredBoneBlendWeights.SetNumZeroed(NumRequiredBones);
    for (int32 RequiredBoneIndex = 0; RequiredBoneIndex < NumRequiredBones; RequiredBoneIndex++)
    {
        const int32 SkeletonBoneIndex = RequiredBones.GetSkeletonIndex(FCompactPoseBoneIndex(RequiredBoneIndex));
        if (ensure(SkeletonBoneIndex != INDEX_NONE))
        {
            if (DesiredBoneBlendWeights.IsValidIndex(RequiredBoneIndex) == true && PerBoneBlendWeights.IsValidIndex(SkeletonBoneIndex) == true)
            {
                DesiredBoneBlendWeights[RequiredBoneIndex] = PerBoneBlendWeights[SkeletonBoneIndex];
            }
            //GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, FVector(RequiredBoneIndex, SkeletonBoneIndex, 0).ToString());
            //ta funkcja wykonuje sie podczas BeginPlay oraz zawiera wszystkie indeksy kosci (podstawowo 79) Required == Skeleton index
        }
    }

    CurrentBoneBlendOutput.Reset(DesiredBoneBlendWeights.Num());
    CurrentBoneBlendOutput.AddZeroed(DesiredBoneBlendWeights.Num());

    //Reinitialize bone blend weights now that we have cleared them
    TArray<float> BlendWeights = { BlendAlpha };
    FAnimationRuntime::UpdateDesiredBoneWeight(DesiredBoneBlendWeights, CurrentBoneBlendOutput, BlendWeights);

    TArray<uint16> const& CurveUIDFinder = RequiredBones.GetUIDToArrayLookupTable();
    const int32 CurveUIDCount = CurveUIDFinder.Num();
    const int32 TotalCount = FBlendedCurve::GetValidElementCount(&CurveUIDFinder);
    if (TotalCount > 0)
    {
        CPSI.Reset(TotalCount);
        // initialize with FF - which is default
        CPSI.Init(DEFAULT_SOURCEINDEX, TotalCount);

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
                                CPSI[CurrentPoseIndex] = DesiredBoneBlendWeights[CompactPoseIndex.GetInt()].SourceIndex;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    { CPSI.Reset(); }
    RBSN = RequiredBones.GetSerialNumber();

    //                                                              PRINT INFO
    int ii = 0;
    FString t;
    for (FPerBoneBlendWeight& BoneWeight : CurrentBoneBlendOutput)
    {

        if (IsValid(AnimInst) == true && BoneWeight.BlendWeight > 0)
        {
            t = AnimInst->GetOwningComponent()->GetBoneName(ii).ToString();
            t = t + TEXT(" = ") + FVector(BoneWeight.SourceIndex, BoneWeight.BlendWeight, 0).ToString();
            //GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, t);
        }
        ii = ii + 1;
    }
    
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ||| L A Y E R   B L E N D   P E R   B O N E ||| MACRO - CREATE LayerBlend Mask
TArray<FInputBlendPose> FJWAN_LayerBlendingLogic::CreateLayerBlendValue(TArray<FName> BonesName, TArray<int> BlendDepth)
{
    TArray<FInputBlendPose> LBP = {};
    FInputBlendPose AddBlendPose;
    FBranchFilter AddFilter;
    int ii = -1;
    if (BonesName.Num() > 0 && BlendDepth.Num() > 0)
    {
        for (FName& CurrentName : BonesName)
        {
            ii = ii + 1;
            AddFilter.BoneName = CurrentName;
            if (BlendDepth.IsValidIndex(ii) == true)
            { AddFilter.BlendDepth = BlendDepth[ii]; }
            else
            { AddFilter.BlendDepth = 0; }
            AddBlendPose.BranchFilters.Add(AddFilter);
        }
        LBP.Add(AddBlendPose);
        return LBP;
    }
    return LBP;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//SLOT UPDATE
void FJWAN_LayerBlendingLogic::SlotUpdate(const FAnimationUpdateContext& Context, FName SName, FPoseLink InputPose, FSlotNodeWeightInfo Weight)
{
    // Update weights.
    Context.AnimInstanceProxy->GetSlotWeight(SName, Weight.SlotNodeWeight, Weight.SourceWeight, Weight.TotalNodeWeight);
    // Update cache in AnimInstance.
    Context.AnimInstanceProxy->UpdateSlotNodeWeight(SName, Weight.SlotNodeWeight, Context.GetFinalBlendWeight());

    UE::Anim::FSlotInertializationRequest InertializationRequest;
    if (Context.AnimInstanceProxy->GetSlotInertializationRequest(SName, InertializationRequest))
    {
        UE::Anim::IInertializationRequester* InertializationRequester = Context.GetMessage<UE::Anim::IInertializationRequester>();
        if (InertializationRequester)
        {
            InertializationRequester->RequestInertialization(InertializationRequest.Get<0>(), InertializationRequest.Get<1>());
        }
        else
        {
            FAnimNode_Inertialization::LogRequestError(Context, InputPose);
        }
    }
}

