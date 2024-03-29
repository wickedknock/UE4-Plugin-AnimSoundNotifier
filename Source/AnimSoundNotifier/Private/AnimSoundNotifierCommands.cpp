// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AnimSoundNotifierCommands.h"

#define LOCTEXT_NAMESPACE "FAnimSoundNotifierModule"

void FAnimSoundNotifierCommands::RegisterCommands()
{
	UI_COMMAND(CreateNotifiesAction, "Activate Dialogues", "Auto create AnimNotifies on selected animations for sound dialogues", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(DeleteNotifiesAction, "Deactivate Dialogues", "Delete dialogue anim notifies", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
