
#include "JWAN_LayerBlendingGraph.h"

#define LOCTEXT_NAMESPACE "A3Nodes"


FLinearColor UJWAN_LayerBlendingGraph::GetNodeTitleColor() const
{
    return FLinearColor(0.0, 0.1, 0.3, 1.0);
}

FText UJWAN_LayerBlendingGraph::GetTooltipText() const
{
    return LOCTEXT("LayerBlending For ALS", "LayerBlending For ALS");
}

FText UJWAN_LayerBlendingGraph::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return LOCTEXT("Layer Blending", "Layer Blending");
}

FString UJWAN_LayerBlendingGraph::GetNodeCategory() const
{
    return TEXT("JakubW|Blends");
}

#undef LOCTEXT_NAMESPACE