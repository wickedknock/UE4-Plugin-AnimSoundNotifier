// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "AnimSoundNotifierStyle.h"
#include "AnimSoundNotifier.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "SlateGameResources.h"
#include "IPluginManager.h"

TSharedPtr< FSlateStyleSet > FAnimSoundNotifierStyle::StyleInstance = NULL;

void FAnimSoundNotifierStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAnimSoundNotifierStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FAnimSoundNotifierStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AnimSoundNotifierStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FAnimSoundNotifierStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("AnimSoundNotifierStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("AnimSoundNotifier")->GetBaseDir() / TEXT("Resources"));

	Style->Set("AnimSoundNotifier.CreateNotifiesAction", new IMAGE_BRUSH(TEXT("ActivateIcon2"), Icon40x40));
	Style->Set("AnimSoundNotifier.DeleteNotifiesAction", new IMAGE_BRUSH(TEXT("DeactivateIcon"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FAnimSoundNotifierStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FAnimSoundNotifierStyle::Get()
{
	return *StyleInstance;
}
