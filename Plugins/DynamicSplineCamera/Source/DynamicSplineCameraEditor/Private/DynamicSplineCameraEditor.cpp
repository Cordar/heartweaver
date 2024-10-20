#include "DynamicSplineCameraEditor.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "IPlacementModeModule.h"
#include "CustomStyle/DynamicSplineCameraStyle.h"
#include "DebugUtils/DynamicSplineCameraDebugUtils.h"


DEFINE_LOG_CATEGORY(DynamicSplineCameraEditor);

#define LOCTEXT_NAMESPACE "FDynamicSplineCameraEditor"

void FDynamicSplineCameraEditor::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FDynamicSplineCameraStyle::InitializeIcons();
	RegisterSplineCameraWidgetUMG();
}

void FDynamicSplineCameraEditor::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	IPlacementModeModule::Get().UnregisterPlacementCategory(FName("DynamicSplineCamera"));
	UToolMenus::UnregisterOwner(this);

	FDynamicSplineCameraStyle::Shutdown();
}

void FDynamicSplineCameraEditor::RegisterSplineCameraWidgetUMG()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(
			this, &FDynamicSplineCameraEditor::RegisterSplineCameraWidgetObject));
}

void FDynamicSplineCameraEditor::RegisterSplineCameraWidgetObject()
{
	FToolMenuOwnerScoped MenuOwner("DynamicSplineCamera");
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

	FToolMenuSection& Section = Menu->AddSection("DynamicSplineCamera", FText::FromString("Tools"));

	Section.AddMenuEntry(
		"DynamicSplineCamera",
		FText::FromString("Dynamic Spline Camera"),
		FText::FromString("Open Dynamic Spline Camera Editor"),
		FSlateIcon("DynamicSplineCamera", "DynamicSplineCamera.CategoryIcon"),
		FUIAction(FExecuteAction::CreateRaw(this, &FDynamicSplineCameraEditor::OpenSplineCameraWidget))
	);
}

void FDynamicSplineCameraEditor::OpenSplineCameraWidget()
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();

	if (EditorUtilitySubsystem)
	{
		// Cargar el blueprint del Editor Utility Widget (debe ser del tipo UEditorUtilityWidget)
		UEditorUtilityWidgetBlueprint* WidgetBlueprint = LoadObject<UEditorUtilityWidgetBlueprint>(
			nullptr, TEXT("/DynamicSplineCamera/Widgets/DynamicSplineCameraWidget"));

		if (WidgetBlueprint)
		{
			// Mostrar el widget en una pestaña flotante
			EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBlueprint);
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicSplineCameraEditor, DynamicSplineCameraEditor)
