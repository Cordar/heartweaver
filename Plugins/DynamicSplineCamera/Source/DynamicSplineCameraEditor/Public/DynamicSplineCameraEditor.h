#pragma once

DECLARE_LOG_CATEGORY_EXTERN(DynamicSplineCameraEditor, All, All);

class FDynamicSplineCameraEditor : public IModuleInterface
{
	public:

	/* Called when the module is loaded */
	virtual void StartupModule() override;

	/* Called when the module is unloaded */
	virtual void ShutdownModule() override;

private:

	void RegisterSplineCameraTab();
	TSharedPtr<SDockTab> ConstructedDockTab;

	void RegisterSplineCameraWidgetUMG();
	void RegisterSplineCameraWidgetObject();
	void OpenSplineCameraWidget();
	
	TSharedPtr<FSlateStyleSet> StyleSet;
};