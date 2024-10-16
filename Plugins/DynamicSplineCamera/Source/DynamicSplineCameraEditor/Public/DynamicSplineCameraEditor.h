#pragma once

#include "SlateWidgets/SSplineCameraWidget.h"

DECLARE_LOG_CATEGORY_EXTERN(DynamicSplineCameraEditor, All, All);

class FDynamicSplineCameraEditor : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	TSharedPtr<class ACameraSpline> SelectedCameraSpline;
	TSharedRef<class SSplineCameraWidget> CreatedWidget = SNew(SSplineCameraWidget);

	void RegisterEditorIntoCategory();
	
	void RegisterSplineCameraTab();
	TSharedPtr<SDockTab> ConstructedDockTab;

	TSharedRef<SDockTab> OnSpawnSplineCameraTab(const FSpawnTabArgs& SpawnTabArgs);
	bool CanSpawnSplineCameraTab(const FSpawnTabArgs& SpawnTabArgs);


	void OnSplineCameraTabClosed(TSharedRef<SDockTab> TabToClose);

	
	TSharedPtr<FSlateStyleSet> StyleSet;
};