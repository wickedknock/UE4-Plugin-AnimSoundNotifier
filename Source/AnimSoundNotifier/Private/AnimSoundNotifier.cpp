// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AnimSoundNotifier.h"
#include "AnimSoundNotifierStyle.h"
#include "AnimSoundNotifierCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine.h"

#include "LevelEditor.h"
#include "CustomAnimNotify.h"

#include "./DialogueAnimNotify.h"

static const FName AnimSoundNotifierTabName("AnimSoundNotifier");

#define LOCTEXT_NAMESPACE "FAnimSoundNotifierModule"

void FAnimSoundNotifierModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FAnimSoundNotifierStyle::Initialize();
	FAnimSoundNotifierStyle::ReloadTextures();

	FAnimSoundNotifierCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAnimSoundNotifierCommands::Get().CreateNotifiesAction,
		FExecuteAction::CreateRaw(this, &FAnimSoundNotifierModule::CreateAnimNotifies_Func),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FAnimSoundNotifierCommands::Get().DeleteNotifiesAction,
		FExecuteAction::CreateRaw(this, &FAnimSoundNotifierModule::DeleteAnimNotifies_Func),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FAnimSoundNotifierModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
		
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FAnimSoundNotifierModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FAnimSoundNotifierModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FAnimSoundNotifierStyle::Shutdown();

	FAnimSoundNotifierCommands::Unregister();
}

// Put your "OnButtonClicked" stuff here
void FAnimSoundNotifierModule::CreateAnimNotifies_Func()
{
	
	FText DialogText = FText::Format(
							LOCTEXT("Auto Create AnimNotifies", "Adding notifiers now"),
							FText::FromString(TEXT(""))
					   );

	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	
	USelection* selectedActors = GEditor->GetSelectedObjects();
	
	TArray<AActor*> Actors;
	TArray <ULevel*> UniqueLevels;
	for (FSelectionIterator Iter(*selectedActors); Iter; ++Iter) {

		bool isAnimInTable = false;
		//FString tempAnimName = "";

		UAnimSequence* animAsset = Cast <UAnimSequence>(*Iter);
		if (animAsset) {

			FString dataTablePath = FString("DataTable'/Game/DataSets/");
			dataTablePath += UCustomAnimNotify::GetSceneNameFromAnimation(animAsset->GetName());
			dataTablePath += "_";
			dataTablePath += UCustomAnimNotify::GetCharacterNameFromAnimation(animAsset->GetName());
			dataTablePath += ".";
			dataTablePath += UCustomAnimNotify::GetSceneNameFromAnimation(animAsset->GetName());
			dataTablePath += "_";
			dataTablePath += UCustomAnimNotify::GetCharacterNameFromAnimation(animAsset->GetName());
			dataTablePath += "'";

			UE_LOG(LogTemp, Warning, TEXT("Animation: %s"), *animAsset->GetName());

			UE_LOG(LogTemp, Warning, TEXT("Data Table Path: %s"), *dataTablePath);

			UDataTable* dataTable = LoadObject<UDataTable>(NULL, *dataTablePath, NULL, LOAD_None);

			if (dataTable) {

				UE_LOG(LogTemp, Warning, TEXT("Found data table: %s"), *dataTable->GetName());

				int32 columnIndex = 0;

				

				for (int i = 1; i < dataTable->GetTableData().Num(); i++) {

					if (animAsset->GetName().Equals(dataTable->GetTableData()[i][1])) {


						//if (!tempAnimName.Equals(animAsset->GetName())) {
						if(!isAnimInTable) {
							
							//tempAnimName = animAsset->GetName();
							isAnimInTable = true;
							
							UE_LOG(LogTemp, Warning, TEXT("Animation %s found in table at row %d"), *animAsset->GetName(), i);

						}
						
						

						FString audioName = dataTable->GetTableData()[i][2];
						FString frameNoStr = dataTable->GetTableData()[i][3];
						uint32 frameNo = FCString::Atoi(*frameNoStr);

						if (audioName.Compare("") != 0 || frameNoStr.Compare("") != 0) {

							//FString leftData, rightData;
							//currentCellText.Split(TEXT(":"), &leftData, &rightData);

							//uint32 frameNo = FCString::Atoi(*rightData);
							//FString audioName = leftData;

							UDialogueAnimNotify* customNotify = NewObject<UDialogueAnimNotify>(animAsset);
							customNotify->NotifyID = audioName;

							//** THE FOLLOWING LINE CAUSES ENGINE TO CRASH WHEN THE CREATE ANIM NOTIFIES BUTTON IS PRESSED THE SECOND TIME **
							//customNotify->Rename(*notifyName);

							int32 NewNotifyIndex = animAsset->Notifies.Add(FAnimNotifyEvent());
							FAnimNotifyEvent& NewEvent = animAsset->Notifies[NewNotifyIndex];
							NewEvent.NotifyName = FName(*audioName);
							NewEvent.Notify = customNotify;
							NewEvent.ChangeSlotIndex(0);
							NewEvent.NotifyTriggerChance = 1;
							
							NewEvent.LinkSequence(animAsset, animAsset->GetTimeAtFrame(frameNo));

							animAsset->RefreshCacheData();
							animAsset->MarkPackageDirty();

						}
						//}

					}
					else {
						//UE_LOG(LogTemp, Warning, TEXT("Animation %s not found in data table"), *animAsset->GetName());
						
					}
				}

				

			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Data table not found. Path: %s"), *dataTablePath);
				//return;
			}
			
		}
		else {
			DialogText = FText::Format(
				LOCTEXT("Auto Create AnimNotifies", "Please select animation asset"),
				FText::FromString(TEXT(""))
			);
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
			
		}

		if (isAnimInTable) {

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Animation %s not found in data table"), *animAsset->GetName());
			//tempAnimName = "";
		}
		
	}

	DialogText = FText::Format(
		LOCTEXT("Auto Create AnimNotifies", "Done!"),
		FText::FromString(TEXT(""))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	
}

void FAnimSoundNotifierModule::DeleteAnimNotifies_Func()
{

	FText DialogText = FText::Format(
		LOCTEXT("Auto Create AnimNotifies", "Deleting notifiers now"),
		FText::FromString(TEXT(""))
	);

	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	USelection* selectedActors = GEditor->GetSelectedObjects();

	TArray<AActor*> Actors;
	TArray <ULevel*> UniqueLevels;

	DialogText = FText::Format(
		LOCTEXT("Auto Dialogues Plugin", "selectedActors assigned"),
		FText::FromString(TEXT(""))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	for (FSelectionIterator Iter(*selectedActors); Iter; ++Iter) {

		DialogText = FText::Format(
			LOCTEXT("Auto Dialogues Plugin", "Found selected objects"),
			FText::FromString(TEXT(""))
		);
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		

		UAnimSequence* animAsset = Cast <UAnimSequence>(*Iter);
		if (animAsset) {

			for (int i = 0; i < animAsset->Notifies.Num(); i++) {
				if (animAsset->Notifies[i].NotifyName.ToString().Contains("DialogueAnim")) {
					//animAsset->Notifies[i].Notify->BeginDestroy();
					animAsset->Notifies[i].Clear();
					//animAsset->Notifies.RemoveAt(i);

					animAsset->RefreshCacheData();
					animAsset->MarkPackageDirty();

					
					UE_LOG(LogTemp, Warning, TEXT("NotifierName: %s"), *animAsset->Notifies[i].NotifyName.ToString());

					DialogText = FText::Format(
						LOCTEXT("Auto Dialogues Plugin", "Notifiers successfully removed!"),
						FText::FromString(TEXT(""))
					);
					FMessageDialog::Open(EAppMsgType::Ok, DialogText);
				}
			}

		}
		else {
			DialogText = FText::Format(
				LOCTEXT("Auto Create AnimNotifies", "Please select animation asset"),
				FText::FromString(TEXT(""))
			);
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);

		}

	}

}

void FAnimSoundNotifierModule::AddMenuExtension(FMenuBuilder& Builder)
{
	//Builder.AddMenuEntry(FAnimSoundNotifierCommands::Get().PluginAction);
}

void FAnimSoundNotifierModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FAnimSoundNotifierCommands::Get().CreateNotifiesAction);
	Builder.AddToolBarButton(FAnimSoundNotifierCommands::Get().DeleteNotifiesAction);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnimSoundNotifierModule, AnimSoundNotifier)