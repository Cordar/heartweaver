#include "DynamicSplineCameraEditor.h"

#include "IPlacementModeModule.h"
#include "CustomStyle/DynamicSplineCameraStyle.h"
#include "DebugUtils/DynamicSplineCameraDebugUtils.h"
#include "Interfaces/IPluginManager.h"
#include "SlateWidgets/SSplineCameraWidget.h"
#include "Styling/SlateStyleRegistry.h"

DEFINE_LOG_CATEGORY(DynamicSplineCameraEditor);

#define LOCTEXT_NAMESPACE "FDynamicSplineCameraEditor"

void FDynamicSplineCameraEditor::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Ya no nos hace falta la categoría
	// RegisterEditorIntoCategory();
	RegisterSplineCameraTab();
}

void FDynamicSplineCameraEditor::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	IPlacementModeModule::Get().UnregisterPlacementCategory(FName("DynamicSplineCamera"));

	FDynamicSplineCameraStyle::Shutdown();
}

void FDynamicSplineCameraEditor::RegisterEditorIntoCategory()
{
	// Register editor category
	const int Priority = 42;
	FPlacementCategoryInfo DynamicSplineCamera(
		LOCTEXT("Dynamic Spline Camera", "Spline Camera Tools"),
		FSlateIcon("DynamicSplineCamera", "DynamicSplineCamera.CategoryIcon"), "DynamicSplineCamera",
		TEXT("PMDynamicSplineCamera"), Priority);
	
	IPlacementModeModule::Get().RegisterPlacementCategory(DynamicSplineCamera);


	UBlueprint* TestActor = Cast<UBlueprint>(
		FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/TestActor")).TryLoad());
	if (TestActor)
	{
		IPlacementModeModule::Get().RegisterPlaceableItem(DynamicSplineCamera.UniqueHandle, MakeShareable(
			                                                  new FPlaceableItem(
				                                                  *UActorFactory::StaticClass(),
				                                                  FAssetData(TestActor, true),
				                                                  FName("TestActor.Thumbnail"),
#if ENGINE_MAJOR_VERSION == 5
				                                                  FName("TestActor.Icon"),
#endif
				                                                  TOptional<FLinearColor>(),
				                                                  TOptional<int32>(),
				                                                  NSLOCTEXT("PlacementMode", "Test Actor", "Test Actor")
			                                                  )));
	}

	UBlueprint* CameraSpline = Cast<UBlueprint>(
		FSoftObjectPath(TEXT("/DynamicSplineCamera/Blueprints/CameraSpline")).TryLoad());
	if (CameraSpline)
	{
		IPlacementModeModule::Get().RegisterPlaceableItem(DynamicSplineCamera.UniqueHandle, MakeShareable(
			                                                  new FPlaceableItem(
				                                                  *UActorFactory::StaticClass(),
				                                                  FAssetData(CameraSpline, true),
				                                                  FName("CameraSpline.Thumbnail"),
#if ENGINE_MAJOR_VERSION == 5
				                                                  FName("CameraSpline.Icon"),
#endif
				                                                  TOptional<FLinearColor>(),
				                                                  TOptional<int32>(),
				                                                  NSLOCTEXT("PlacementMode", "Camera Spline",
				                                                            "CameraSpline")
			                                                  )));
	}

	// Set Brush Images and register the style
	StyleSet = MakeShareable(new FSlateStyleSet("DynamicSplineCamera"));

	FString BlockoutIconPath = IPluginManager::Get().FindPlugin(TEXT("DynamicSplineCamera"))->GetBaseDir() +
		TEXT("/Resources/");

	StyleSet->Set("DynamicSplineCamera.CategoryIcon",
	              new FSlateImageBrush(BlockoutIconPath + TEXT("DynamicSplineCameraIcon16.png"),
	                                   FVector2D(16.f, 16.f)));

	StyleSet->Set("TestActor.Thumbnail",
	              new FSlateImageBrush(BlockoutIconPath + TEXT("TestActorIcon64.png"), FVector2D(64.f, 64.f)));
	StyleSet->Set("CameraSpline.Thumbnail",
	              new FSlateImageBrush(BlockoutIconPath + TEXT("CameraSplineIcon64.png"), FVector2D(64.f, 64.f)));
	// StyleSet->Set("Blockout_Cone.Thumbnail", new FSlateImageBrush(BlockoutIconPath + TEXT("Blockout_Cone_64.png"), FVector2D(64.f, 64.f)));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FDynamicSplineCameraEditor::RegisterSplineCameraTab()
{
	// Registramos la opción del menú de "tools". También repite dos veces en "window"
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("SplineCamera"),
	                                                  FOnSpawnTab::CreateRaw(
		                                                  this, &FDynamicSplineCameraEditor::OnSpawnSplineCameraTab),
	                                                  FCanSpawnTab::CreateRaw(
		                                                  this, &FDynamicSplineCameraEditor::CanSpawnSplineCameraTab)
	                        )
	                        .SetDisplayName(FText::FromString(TEXT("Spline camera")));
	// .SetIcon(FSlateIcon(FDynamicSplineCameraStyle::GetStyleSetName(),
	//                     "ContentBrowser.SplineCamera"));
}

TSharedRef<SDockTab> FDynamicSplineCameraEditor::OnSpawnSplineCameraTab(const FSpawnTabArgs& SpawnTabArgs)
{
	/*if (FolderPathsSelected.Num() == 0)*/
	// return SNew(SDockTab).TabRole(NomadTab);

	CreatedWidget->OnSpawnTab();

	ConstructedDockTab = SNew(SDockTab).TabRole(NomadTab)
	[
		CreatedWidget
		// .AssetsDataToStore(GetAllAssetDataUnderSelectedFolder())
		// .CurrentSelectedFolder(FolderPathsSelected[0])
	];

	ConstructedDockTab->SetOnTabClosed(
		SDockTab::FOnTabClosedCallback::CreateRaw(this, &FDynamicSplineCameraEditor::OnSplineCameraTabClosed));

	return ConstructedDockTab.ToSharedRef();
}

bool FDynamicSplineCameraEditor::CanSpawnSplineCameraTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// SpawnTabArgs.GetOwnerWindow().name

	return true;
}


void FDynamicSplineCameraEditor::OnSplineCameraTabClosed(TSharedRef<SDockTab> TabToClose)
{
	if (ConstructedDockTab.IsValid())
	{
		DynamicSplineCameraDebugUtils::Print(TEXT("Reseteamos dock tab"), FColor::Yellow);
		CreatedWidget->Reset();
		ConstructedDockTab.Reset();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDynamicSplineCameraEditor, DynamicSplineCameraEditor)