// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AnimSoundNotifierStyle.h"

class FAnimSoundNotifierCommands : public TCommands<FAnimSoundNotifierCommands>
{
public:

	FAnimSoundNotifierCommands()
		: TCommands<FAnimSoundNotifierCommands>(TEXT("AnimSoundNotifier"), NSLOCTEXT("Contexts", "AnimSoundNotifier", "AnimSoundNotifier Plugin"), NAME_None, FAnimSoundNotifierStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > CreateNotifiesAction;
	TSharedPtr< FUICommandInfo > DeleteNotifiesAction;
};
