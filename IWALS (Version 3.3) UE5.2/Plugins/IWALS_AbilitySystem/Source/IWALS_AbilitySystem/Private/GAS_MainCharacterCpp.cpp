


#include "GAS_MainCharacterCpp.h"
#include "GameplayTagsManager.h"

// Sets default values
AGAS_MainCharacterCpp::AGAS_MainCharacterCpp()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UIWALS_BaseAttributeSet>("Attributes");

}

// Called when the game starts or when spawned
void AGAS_MainCharacterCpp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGAS_MainCharacterCpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGameplayTag AGAS_MainCharacterCpp::ConvertLiteralNameToTag(FName TagName)
{
	return FGameplayTag::RequestGameplayTag(TagName);
}

FString AGAS_MainCharacterCpp::GetSubTag(const FGameplayTag& Tag, int32 DesiredDepth)
{
	FString FullTagName = Tag.ToString();
	TArray<FString> SplitTags;
	FullTagName.ParseIntoArray(SplitTags, TEXT("."));

	// Jeœli DesiredDepth przekracza iloœæ segmentów lub jest ujemny, zwróæ pusty string.
	if (DesiredDepth < 0 || DesiredDepth >= SplitTags.Num())
	{
		return FString();
	}

	// ZnajdŸ odpowiedni¹ g³êbokoœæ od koñca.
	int32 IndexFromEnd = SplitTags.Num() - DesiredDepth - 1;
	if (IndexFromEnd >= 0 && IndexFromEnd < SplitTags.Num())
	{
		return SplitTags[IndexFromEnd];
	}

	return FString();
}

bool AGAS_MainCharacterCpp::IsTagLeaf(const FGameplayTag& Tag)
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
	TSharedPtr<FGameplayTagNode> NodePtr = TagsManager.FindTagNode(Tag);
	const FGameplayTagNode* Node = NodePtr.Get();

	if (Node)
	{
		return Node->GetChildTagNodes().Num() == 0;
	}

	return false;
}

bool AGAS_MainCharacterCpp::SwitchOnOwnedTags(const FGameplayTag& NewState)
{
	if (IsValid(AbilitySystemComponent) == false) return false;

	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

	const bool LastInGroup = IsTagLeaf(NewState);

	TArray<FGameplayTag> TagArray;
	OwnedTags.GetGameplayTagArray(TagArray);

	TArray<FGameplayTag> TagsToRemove = {};

	for (FGameplayTag& Tag : TagArray)
	{
		int SelectDepth = 0;
		if (LastInGroup == true) SelectDepth = 1;
		const FString a = GetSubTag(Tag, 1);
		const FString b = GetSubTag(NewState, SelectDepth);
		if (a == b)
		{
			TagsToRemove.Add(Tag);
		}
	}
	//Convert To TagContainer
	FGameplayTagContainer TagsContainerToRemove;
	for (const FGameplayTag& Tag : TagsToRemove)
	{
		TagsContainerToRemove.AddTag(Tag);
	}
	//Remove Tags
	AbilitySystemComponent->RemoveLooseGameplayTags(TagsContainerToRemove);
	if (LastInGroup == true)
	{
		AbilitySystemComponent->AddLooseGameplayTag(NewState);
	}
	return true;
}

bool AGAS_MainCharacterCpp::SwitchOnOwnedTagsWithIgnore(const FGameplayTag& NewState, const FGameplayTagContainer& DoNotEdit)
{
	if (IsValid(AbilitySystemComponent) == false) return false;

	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

	const bool LastInGroup = IsTagLeaf(NewState);

	TArray<FGameplayTag> TagArray;
	OwnedTags.GetGameplayTagArray(TagArray);

	TArray<FGameplayTag> TagsToRemove = {};

	for (FGameplayTag& Tag : TagArray)
	{
		int SelectDepth = 0;
		if (LastInGroup == true) SelectDepth = 1;
		const FString a = GetSubTag(Tag, 1);
		const FString b = GetSubTag(NewState, SelectDepth);
		//Find Match
		FGameplayTagContainer TagAsContainer;
		TagAsContainer.AddTag(Tag);

		if (a == b && TagAsContainer.HasAllExact(DoNotEdit) == false)
		{
			TagsToRemove.Add(Tag);
		}
	}
	//Convert To TagContainer
	FGameplayTagContainer TagsContainerToRemove;
	for (const FGameplayTag& Tag : TagsToRemove)
	{
		TagsContainerToRemove.AddTag(Tag);
	}
	//Remove Tags
	AbilitySystemComponent->RemoveLooseGameplayTags(TagsContainerToRemove);
	if (LastInGroup == true)
	{
		AbilitySystemComponent->AddLooseGameplayTag(NewState);
	}
	return true;
}

// Called to bind functionality to input
void AGAS_MainCharacterCpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AGAS_MainCharacterCpp::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGAS_MainCharacterCpp::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AGAS_MainCharacterCpp::GiveAbilities()
{
	//GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, "Pysk", true);
	if (AbilitiesData && AbilitySystemComponent)
	{
		AbilitiesData->GiveAbilities(AbilitySystemComponent, this);
	}
}

void AGAS_MainCharacterCpp::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent->AbilityActorInfo.IsValid() == false)
	{
		GEngine->AddOnScreenDebugMessage(0, 3, FColor::Red, "GAS ERROR - AbilityActorInfo is NOT valid ", true);
		return;
	}
	AbilitySystemComponent->AbilityActorInfo->InitFromActor(this, this, AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GiveAbilities();

}

void AGAS_MainCharacterCpp::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
}

void AGAS_MainCharacterCpp::TryCreateInputsGAS()
{
	if (AbilitySystemComponent && InputComponent)
	{
		FTopLevelAssetPath PathToEnum = FTopLevelAssetPath(GetPathNameSafe(UClass::TryFindTypeSlow<UEnum>("EIWALS_AbilityInputBinds")));
		const FGameplayAbilityInputBinds Binds("JumpAction", "AimActionType_2", PathToEnum, static_cast<int32>(EIWALS_AbilityInputBinds::JumpAction), static_cast<int32>(EIWALS_AbilityInputBinds::AimActionType_2));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

