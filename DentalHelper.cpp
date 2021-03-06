#include "stdafx.h"
#include "DentalHelper.h"

enum WorkState
{
	ImageLoading,
	preplanning,
	drawimplant
};
enum DrawImplantState
{
	none,
	first
};
DentalHelper::DentalHelper(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);
	ui.ImageTabelWidget->setColumnWidth(1, 150);
	ui.ImageTabelWidget->setColumnWidth(2, 70);
	ui.ImageTabelWidget->setColumnWidth(3, 70);
	ui.ImageTabelWidget->setColumnWidth(4, 130);
	m_WorkingState = WorkState::ImageLoading;
	m_DrawImplantState = DrawImplantState::none;
	m_CurrentIndex = -1;
	m_CurrentImplant = -1;
	m_CurrentParallel = -1;

	isShowUpLeftView = true;
	isShowLowerLeftView = true;
	isShowUpRightView = true;
	isShowLowerRightView = true;

	isAxial = true;
	isCoronal = true;
	isSagital = true;

	isPanaromic = false;
	isArchCurve = false;
	isCross = false;
	isRotation = false;
	isPosition = false;
	for (int i=0;i<3;i++)
	{
		centerOfPreviousMoveWiget[i] = 0;
	}
	m_ProgressDialog = new QProgressWidget(0);
	m_ProgressDialog->hide();
	m_ImageData = vtkSmartPointer<vtkImageData>::New();

	m_AxialViewer = vtkSmartPointer<vtkImageViewer2>::New();
	m_AxialPlane = vtkSmartPointer<vtkPlane>::New();

	m_CoronalViewer = vtkSmartPointer<vtkImageViewer2>::New();
	m_CoronalPlane = vtkSmartPointer<vtkPlane>::New();

	m_SagitalViewer = vtkSmartPointer<vtkImageViewer2>::New();
	m_SagitalPlane = vtkSmartPointer<vtkPlane>::New();



	m_VolumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
	m_VolumeFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	m_VolumeColor = vtkSmartPointer<vtkColorTransferFunction>::New();
	m_VolumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	m_VolumeActor = vtkSmartPointer<vtkVolume>::New();

	AxialColor[0] = 0.845;
	AxialColor[1] = 0.250;
	AxialColor[2] = 0.11;

	CoronalColor[0] = 0.14;
	CoronalColor[1] = 0.44;
	CoronalColor[2] = 0.76;

	
	SagitalColor[0] = 0.686;
	SagitalColor[1] = 0.792;
	SagitalColor[2] = 0.216;

	this->InitializeView();

	m_PlaneWidgetForArchCurve = vtkSmartPointer<vtkPlaneWidget>::New();

	m_ResliceMapperForArchCurve = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropForArchCurve = vtkSmartPointer<vtkImageSlice>::New();

	m_ContourRepForArchCurve = vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
	m_ContourWidgetForArchCurve = vtkSmartPointer<vtkContourWidget>::New();
	m_ContourLineInterpolator = vtkSmartPointer<vtkBezierContourLineInterpolator>::New();
	m_ContourLineInterpolator->SetMaximumCurveLineSegments(200);
	m_ContourFocalPlanePlacer = vtkSmartPointer<vtkFocalPlanePointPlacer>::New();
	m_ContourInitialData = vtkSmartPointer<vtkPolyData>::New();

	m_ResliceMapperOfCrossInModel = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropOfCrossInModel = vtkSmartPointer<vtkImageSlice>::New();

	m_ResliceMapperOfCrossIn2D = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropOfCrossIn2D = vtkSmartPointer<vtkImageSlice>::New();


	m_ResliceMapperOfRotationIn2D = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropOfRotationIn2D = vtkSmartPointer<vtkImageSlice>::New();

	m_ResliceMapperOfRotationIn3D = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropOfRotationIn3D = vtkSmartPointer<vtkImageSlice>::New();

	m_RotationPlane = vtkSmartPointer<vtkPlane>::New();

	m_ResliceMapperOfPositionIn2D = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropOfPositionIn2D = vtkSmartPointer<vtkImageSlice>::New();

	m_ResliceMapperOfPositionIn3D = vtkSmartPointer<vtkImageResliceMapper>::New();
	m_ReslicePropOfPositionIn3D = vtkSmartPointer<vtkImageSlice>::New();

	m_PositionPlane = vtkSmartPointer<vtkPlane>::New();

	m_ContourSplineFilter = vtkSmartPointer<vtkSplineFilter>::New();
	m_ContourOffSetSplineFilter = vtkSmartPointer<vtkSplineFilter>::New();
	m_ContourOffSetActor = vtkSmartPointer<vtkActor>::New();

	m_ContourOffSetSplineFilter2D = vtkSmartPointer<vtkSplineFilter>::New();

	m_CrossPlane = vtkSmartPointer<vtkPlane>::New();

	m_PanaromicProbeFilterInModel = vtkSmartPointer<vtkProbeFilter>::New();
	m_PanaromicActorInModel = vtkSmartPointer<vtkActor>::New();
	m_PanaromicActorForDrawingNurve = vtkSmartPointer<vtkActor>::New();
	m_PanaromicActorIn2D = vtkSmartPointer<vtkActor>::New();


	m_ContourForLeftNurve = vtkSmartPointer<vtkContourWidget>::New();
	m_ContourLineForLeftNurve = vtkSmartPointer<vtkBezierContourLineInterpolator>::New();
	m_ContourPointPlacerForLeftNurve = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
	m_ContourRepForLeftNurve = vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
	m_TubeForLeftNurve = vtkSmartPointer<vtkTubeFilter>::New();
	m_ActorForLeftNurve = vtkSmartPointer<vtkActor>::New();


	m_ContourForRightNurve = vtkSmartPointer<vtkContourWidget>::New();
	m_ContourLineForRightNurve = vtkSmartPointer<vtkBezierContourLineInterpolator>::New();
	m_ContourPointPlacerForRightNurve = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
	m_ContourRepForRightNurve = vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
	m_TubeForRightNurve = vtkSmartPointer<vtkTubeFilter>::New();
	m_ActorForRightNurve = vtkSmartPointer<vtkActor>::New();


	m_CutActorForLeftNurveInCross = vtkSmartPointer<vtkActor>::New();
	m_CutActorForLeftNurveInAxial = vtkSmartPointer<vtkActor>::New();
	m_CutActorForLeftNurveInCoronal = vtkSmartPointer<vtkActor>::New();
	m_CutActorForLeftNurveInSagital = vtkSmartPointer<vtkActor>::New();
	m_CutActorForLeftNurveInRotation = vtkSmartPointer<vtkActor>::New();
	m_CutActorForLeftNurveInPosition = vtkSmartPointer<vtkActor>::New();


	m_CutActorForRightNurveInCross = vtkSmartPointer<vtkActor>::New();
	m_CutActorForRightNurveInAxial = vtkSmartPointer<vtkActor>::New();
	m_CutActorForRightNurveInCoronal = vtkSmartPointer<vtkActor>::New();
	m_CutActorForRightNurveInSagital = vtkSmartPointer<vtkActor>::New();
	m_CUtActorForRightNurveInRotation = vtkSmartPointer<vtkActor>::New();
	m_CutActorForRightNurveInPosition = vtkSmartPointer<vtkActor>::New();


	m_UpProthesisData = vtkSmartPointer<vtkPolyData>::New();
	m_UpProthesisActor = vtkSmartPointer<vtkActor>::New();

	m_CutActorForUpProthesisInAxial = vtkSmartPointer<vtkActor>::New();
	m_CutActorForUpProthesisInCoronal = vtkSmartPointer<vtkActor>::New();
	m_CutActorForUpProthesisInSagital = vtkSmartPointer<vtkActor>::New();
	m_CutActorForUpProthesisInCross = vtkSmartPointer<vtkActor>::New();
	m_CutActorForUpProthesisInRotation = vtkSmartPointer<vtkActor>::New();
	m_CutActorForUpProthesisInPosition = vtkSmartPointer<vtkActor>::New();


	m_DownProthesisData = vtkSmartPointer<vtkPolyData>::New();
	m_DownProthesisActor = vtkSmartPointer<vtkActor>::New();

	m_CutActorForDownProthesisInAxial = vtkSmartPointer<vtkActor>::New();
	m_CutActorForDownProthesisInCoronal = vtkSmartPointer<vtkActor>::New();
	m_CutActorForDownProthesisInSagital = vtkSmartPointer<vtkActor>::New();
	m_CutActorForDownProthesisInCross = vtkSmartPointer<vtkActor>::New();
	m_CutActorForDownProthesisInRotation = vtkSmartPointer<vtkActor>::New();
	m_CutActorForDownProthesisInPosition = vtkSmartPointer<vtkActor>::New();



	m_FirstRotateWidget = vtkSmartPointer<vtkSphereWidget>::New();
	m_FirstRotateWidget->SetInteractor(m_LowerLeftInteractor);
	m_FirstRotateWidget->SetRadius(1);
	m_FirstRotateWidget->SetRepresentationToWireframe();
	m_FirstRotateWidget->GetSphereProperty()->SetColor(0, 1, 0);
	m_FirstRotateWidget->ScaleOff();

	m_SecondRotateWidget = vtkSmartPointer<vtkSphereWidget>::New();
	m_SecondRotateWidget->SetInteractor(m_LowerLeftInteractor);
	m_SecondRotateWidget->SetRadius(1);
	m_SecondRotateWidget->SetRepresentationToWireframe();
	m_SecondRotateWidget->GetSphereProperty()->SetColor(0, 0, 1);
	m_SecondRotateWidget->ScaleOff();

	m_MoveWidget = vtkSmartPointer<vtkSphereWidget>::New();
	m_MoveWidget->SetInteractor(m_LowerLeftInteractor);
	m_MoveWidget->SetRadius(1);
	m_MoveWidget->SetRepresentationToWireframe();
	m_MoveWidget->GetSphereProperty()->SetColor(1, 0, 0);
	m_MoveWidget->ScaleOff();


	m_EventQtConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();


	
	m_Change2ArchCurve = new QAction("Arch Curve", this);
	m_Change2ArchCurve->setCheckable(true);
	m_Change2Axial = new QAction("Axial", this);
	m_Change2Axial->setCheckable(true);
	m_Change2Position = new QAction("Position", this);
	m_Change2Position->setCheckable(true);

	m_MenuForUpRight = new QMenu(this);
	m_MenuForUpRight->addAction(m_Change2ArchCurve);
	m_MenuForUpRight->addAction(m_Change2Axial);
	m_MenuForUpRight->addAction(m_Change2Position);
	ui.ChangeUpRightViewButton->setMenu(m_MenuForUpRight);

	m_Change2Coronal = new QAction("Coronal", this);
	m_Change2Coronal->setCheckable(true);
	m_Change2Panaromic = new QAction("Panaromic", this);
	m_Change2Panaromic->setCheckable(true);
	m_Change2Rotation = new QAction("Rotation", this);
	m_Change2Rotation->setCheckable(true);

	m_MenuForLowerLeft = new QMenu(this);

	m_MenuForLowerLeft->addAction(m_Change2Coronal);
	m_MenuForLowerLeft->addAction(m_Change2Panaromic);
	m_MenuForLowerLeft->addAction(m_Change2Rotation);

	ui.ChangeLowerLeftButton->setMenu(m_MenuForLowerLeft);

	m_Change2Cross = new QAction("Cross", this);
	m_Change2Cross->setCheckable(true);
	m_Change2Sagital = new QAction("Sagital", this);
	m_Change2Sagital->setCheckable(true);
	m_MenuForLowerRight = new QMenu(this);
	m_MenuForLowerRight->addAction(m_Change2Sagital);
	m_MenuForLowerRight->addAction(m_Change2Cross);
	ui.ChangeLowerRightButton->setMenu(m_MenuForLowerRight);


	m_FirstPointWidget = vtkSmartPointer<vtkSphereWidget>::New();
	m_FirstPointWidget->SetInteractor(m_LowerRightInteractor);
	m_FirstPointWidget->SetRadius(5);
	m_FirstPointWidget->GetSphereProperty()->SetColor(1, 0, 0);
	m_FirstPointWidget->ScaleOff();


	m_SaveProjectPath = "";

	connect(ui.LoadDicomButton, SIGNAL(clicked()), this, SLOT(OnReadDICOM()));
	connect(ui.ImageTabelWidget, SIGNAL(cellClicked(int, int)), this, SLOT(OnImageTableCellClicked(int, int)));
	connect(ui.PreviewImageButton, SIGNAL(clicked()), this, SLOT(OnPreviewData()));
	connect(ui.RemoveImageButton, SIGNAL(clicked()), this, SLOT(OnRemoveImage()));

	connect(ui.UpRightSlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDateUpRightView(int)));
	connect(ui.LowerLeftSlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDateLowerLeftView(int)));
	connect(ui.LowerRightSlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDateLowerRightView(int)));

	connect(ui.ModelOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDateVolumeRendering()));
	connect(ui.MaxThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDateVolumeRendering()));
	connect(ui.MinThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDateVolumeRendering()));

	connect(ui.UpLeftViewButton, SIGNAL(clicked()), this, SLOT(OnUpLeftViewButton()));
	connect(ui.LowerLeftViewButton, SIGNAL(clicked()), this, SLOT(OnLowerLeftViewButton()));
	connect(ui.UpRightViewButton, SIGNAL(clicked()), this, SLOT(OnUpRightViewButton()));
	connect(ui.LowerRightViewButton, SIGNAL(clicked()), this, SLOT(OnLowerRightViewButton()));

	connect(ui.BackGround1Button, SIGNAL(clicked()), this, SLOT(OnSetBackGroundColor1()));
	connect(ui.BackGround2Button, SIGNAL(clicked()), this, SLOT(OnSetBackGroundColor2()));

	connect(ui.ImageNextButton, SIGNAL(clicked()), this, SLOT(OnPreplanSetting()));
	connect(ui.PlaneWidgetVisibilityButton, SIGNAL(clicked()), this, SLOT(OnPlaneWidgetForArchCurveVisibility()));
	connect(ui.PanaromicInModelVisivilityButton, SIGNAL(clicked()), this, SLOT(OnPanaromicInModelVisibility()));
	connect(ui.CrossViewVisibilityButton, SIGNAL(clicked()), this, SLOT(OnCrossViewInModelVisibility()));
	connect(ui.DeleteLastContourNodeButton, SIGNAL(clicked()), this, SLOT(OnDeleteLastContourNode()));
	connect(ui.Panaromic2DRadioButton, SIGNAL(clicked()), this, SLOT(OnChangePanaromicInteractionStyle()));
	connect(ui.Panaromic3DRadioButton, SIGNAL(clicked()), this, SLOT(OnChangePanaromicInteractionStyle()));
	connect(ui.DrawLeftNurveButton, SIGNAL(clicked()), this, SLOT(OnDrawLeftNurve()));
	connect(ui.DeleteLastLeftNurvePointButton, SIGNAL(clicked()), this, SLOT(OnDeleteLastNodeForLeftNurve()));
	connect(ui.DrawRightNurveButton, SIGNAL(clicked()), this, SLOT(OnDrawRightNurve()));
	connect(ui.DeleteLastRightNurvePointButton, SIGNAL(clicked()), this, SLOT(OnDeleteLastNodeForRightNurve()));

	connect(m_Change2Axial, SIGNAL(triggered()), this, SLOT(OnChange2AxialView()));
	connect(m_Change2ArchCurve, SIGNAL(triggered()), this, SLOT(OnChange2ArchCurve()));
	connect(m_Change2Coronal, SIGNAL(triggered()), this, SLOT(OnChange2Coronal()));
	connect(m_Change2Panaromic, SIGNAL(triggered()), this, SLOT(OnChange2Panaromic()));
	connect(m_Change2Rotation, SIGNAL(triggered()), this, SLOT(OnChange2Rotation()));
	connect(m_Change2Position, SIGNAL(triggered()), this, SLOT(OnChange2Position()));

	connect(m_Change2Cross, SIGNAL(triggered()), this, SLOT(OnChange2Cross()));
	connect(m_Change2Sagital, SIGNAL(triggered()), this, SLOT(OnChange2Sagital()));

	connect(ui.LoadUpProthesisButton, SIGNAL(clicked()), this, SLOT(OnLoadUpProthesis()));
	connect(ui.DeleteUpProthesisButton, SIGNAL(clicked()), this, SLOT(OnDeleteUpProthesis()));
	connect(ui.UpProthesisOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OnChangeUpProthesisOpacity(int)));
	connect(ui.UpProthesisVisibilityButton, SIGNAL(clicked()), this, SLOT(OnUpProthesisVisibility()));
	connect(ui.LoadDownProthesisButton, SIGNAL(clicked()), this, SLOT(OnLoadDownProthesis()));
	connect(ui.DeleteDownProthesisButton, SIGNAL(clicked()), this, SLOT(OnDeleteDownProthesis()));
	connect(ui.DownProthesisOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(OnChangeDownProthesisOpacity(int)));
	connect(ui.DownProthesisVisibilityButton, SIGNAL(clicked()), this, SLOT(OnDownProthesisVisibility()));
	connect(ui.RotationIn3DVisibilityButton, SIGNAL(clicked()), this, SLOT(OnRotationVisibilityIn3DView()));
	connect(ui.PositionIn3DVisibilityButton, SIGNAL(clicked()), this, SLOT(OnPositionVisibilityIn3DView()));

	connect(ui.StartDrawImplantButton, SIGNAL(clicked()), this, SLOT(OnStartDrawImplant()));
	connect(ui.AddImplantButton, SIGNAL(clicked()), this, SLOT(OnAddImplant()));

	connect(ui.SaveProjectButton, SIGNAL(clicked()), this, SLOT(OnSaveProject()));
	connect(ui.OpenProjectButton, SIGNAL(clicked()), this, SLOT(OnOpenProject()));
}

DentalHelper::~DentalHelper()
{
	delete m_ProgressDialog;
}
void DentalHelper::AddImageItem(XDicomItem* item)
{
	m_DicomList.append(item);
	int numOfItem = m_DicomList.size();
	ui.ImageTabelWidget->setRowCount(numOfItem - 1);
	ui.ImageTabelWidget->insertRow(numOfItem - 1);

	for each(QString text in item->GetMainList())
	{
		int indexOfItem = item->GetMainList().indexOf(text);
		QTableWidgetItem *itemText = new QTableWidgetItem(text);
		itemText->setTextAlignment(Qt::AlignCenter);
		ui.ImageTabelWidget->setItem(numOfItem - 1, indexOfItem, itemText);
	}
	QTableWidgetItem *LastText = new QTableWidgetItem("...");
	LastText->setTextAlignment(Qt::AlignCenter);
	ui.ImageTabelWidget->setItem(numOfItem - 1, item->GetMainList().size(), LastText);
}


void DentalHelper::CalculateParallelAngle()
{
	if (m_CurrentParallel==-1)
	{
		return;
	}
	double standardNormal[3];
	for (int i=0;i<3;i++)
	{
		standardNormal[i] = m_ImplantList.at(m_CurrentParallel)->m_NormalOfTube[i];
	}
	for each (ImplantItem* var in m_ImplantList)
	{
		double angle = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(var->m_NormalOfTube, standardNormal));
		var->SetParallelAngle(angle);
		var->SetParallelRadioButton(false);
	}
	m_ImplantList.at(m_CurrentParallel)->SetParallelRadioButton(true);

}


void DentalHelper::CutDownProthesisInAxialView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_DownProthesisData);
	cutter->SetCutFunction(m_AxialPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForDownProthesisInAxial->SetMapper(mapper);
	m_CutActorForDownProthesisInAxial->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForDownProthesisInAxial->GetProperty()->SetAmbient(1);
	m_CutActorForDownProthesisInAxial->GetProperty()->SetSpecular(0);
	m_CutActorForDownProthesisInAxial->GetProperty()->SetDiffuse(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInAxial))
	{
		m_UpRightRenderer->AddActor(m_CutActorForDownProthesisInAxial);
	}
	m_UpRightRendWin->Render();

}

void DentalHelper::CutDownProthesisInCoronalView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_DownProthesisData);
	cutter->SetCutFunction(m_CoronalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForDownProthesisInCoronal->SetMapper(mapper);
	m_CutActorForDownProthesisInCoronal->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForDownProthesisInCoronal->GetProperty()->SetAmbient(1);
	m_CutActorForDownProthesisInCoronal->GetProperty()->SetSpecular(0);
	m_CutActorForDownProthesisInCoronal->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCoronal))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForDownProthesisInCoronal);
	}
	m_LowerLeftRendWin->Render();
}
void DentalHelper::CutDownProthesisInCrossView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_DownProthesisData);
	cutter->SetCutFunction(m_CrossPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForDownProthesisInCross->SetMapper(mapper);
	m_CutActorForDownProthesisInCross->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForDownProthesisInCross->GetProperty()->SetAmbient(1);
	m_CutActorForDownProthesisInCross->GetProperty()->SetSpecular(0);
	m_CutActorForDownProthesisInCross->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCross))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForDownProthesisInCross);
	}
	m_LowerRightRendWin->Render();
}
void DentalHelper::CutDownProthesisInPositionView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}
	//如果没有implant,返回
	if (m_ImplantList.isEmpty())
	{
		return;
	}
	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_DownProthesisData);
	cutter->SetCutFunction(m_PositionPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForDownProthesisInPosition->SetMapper(mapper);
	m_CutActorForDownProthesisInPosition->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForDownProthesisInPosition->GetProperty()->SetAmbient(1);
	m_CutActorForDownProthesisInPosition->GetProperty()->SetSpecular(0);
	m_CutActorForDownProthesisInPosition->GetProperty()->SetDiffuse(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInPosition))
	{
		m_UpRightRenderer->AddActor(m_CutActorForDownProthesisInPosition);
	}
	m_UpRightRendWin->Render();

}
void DentalHelper::CutDownProthesisInRotationView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}
	//如果没有implant,返回
	if (m_ImplantList.isEmpty())
	{
		return;
	}
	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_DownProthesisData);
	cutter->SetCutFunction(m_RotationPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForDownProthesisInRotation->SetMapper(mapper);
	m_CutActorForDownProthesisInRotation->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForDownProthesisInRotation->GetProperty()->SetAmbient(1);
	m_CutActorForDownProthesisInRotation->GetProperty()->SetSpecular(0);
	m_CutActorForDownProthesisInRotation->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInRotation))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForDownProthesisInRotation);
	}
	m_LowerLeftRendWin->Render();
}
void DentalHelper::CutDownProthesisInSagitalView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_DownProthesisData);
	cutter->SetCutFunction(m_SagitalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForDownProthesisInSagital->SetMapper(mapper);
	m_CutActorForDownProthesisInSagital->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForDownProthesisInSagital->GetProperty()->SetAmbient(1);
	m_CutActorForDownProthesisInSagital->GetProperty()->SetSpecular(0);
	m_CutActorForDownProthesisInSagital->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInSagital))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForDownProthesisInSagital);
	}
	m_LowerRightRendWin->Render();
}





void DentalHelper::CutLeftNurveInAxialView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}
	

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForLeftNurve->GetOutput());
	cutter->SetCutFunction(m_AxialPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForLeftNurveInAxial->SetMapper(mapper);
	m_CutActorForLeftNurveInAxial->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForLeftNurveInAxial->GetProperty()->SetAmbient(1);
	m_CutActorForLeftNurveInAxial->GetProperty()->SetDiffuse(0);
	m_CutActorForLeftNurveInAxial->GetProperty()->SetSpecular(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInAxial))
	{
		m_UpRightRenderer->AddActor(m_CutActorForLeftNurveInAxial);
	}
	m_UpRightRendWin->Render();
}

void DentalHelper::CutLeftNurveInCoronalView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForLeftNurve->GetOutput());
	cutter->SetCutFunction(m_CoronalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForLeftNurveInCoronal->SetMapper(mapper);
	m_CutActorForLeftNurveInCoronal->GetProperty()->SetColor(1,0, 0);
	m_CutActorForLeftNurveInCoronal->GetProperty()->SetAmbient(1);
	m_CutActorForLeftNurveInCoronal->GetProperty()->SetSpecular(0);
	m_CutActorForLeftNurveInCoronal->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInCoronal))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForLeftNurveInCoronal);
	}
	m_LowerLeftRendWin->Render();
}

void DentalHelper::CutLeftNurveInCrossView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}
	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForLeftNurve->GetOutput());
	cutter->SetCutFunction(m_CrossPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForLeftNurveInCross->SetMapper(mapper);
	m_CutActorForLeftNurveInCross->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForLeftNurveInCross->GetProperty()->SetAmbient(1);
	m_CutActorForLeftNurveInCross->GetProperty()->SetSpecular(0);
	m_CutActorForLeftNurveInCross->GetProperty()->SetDiffuse(1);
	//如果是第一次切割，添加actor
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInCross))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForLeftNurveInCross);
	}
	m_LowerRightRendWin->Render();
}
void DentalHelper::CUtLeftNurveInPositionView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}
	//如果没有implant 返回
	if (m_ImplantList.isEmpty())
	{
		return;
	}

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForLeftNurve->GetOutput());
	cutter->SetCutFunction(m_PositionPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForLeftNurveInPosition->SetMapper(mapper);
	m_CutActorForLeftNurveInPosition->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForLeftNurveInPosition->GetProperty()->SetAmbient(1);
	m_CutActorForLeftNurveInPosition->GetProperty()->SetSpecular(0);
	m_CutActorForLeftNurveInPosition->GetProperty()->SetDiffuse(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInPosition))
	{
		m_UpRightRenderer->AddActor(m_CutActorForLeftNurveInPosition);
	}
	m_UpRightRendWin->Render();
}



void DentalHelper::CutLeftNurveInRotationView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}
	//如果没有implant 返回
	if (m_ImplantList.isEmpty())
	{
		return;
	}
	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForLeftNurve->GetOutput());
	cutter->SetCutFunction(m_RotationPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForLeftNurveInRotation->SetMapper(mapper);
	m_CutActorForLeftNurveInRotation->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForLeftNurveInRotation->GetProperty()->SetAmbient(1);
	m_CutActorForLeftNurveInRotation->GetProperty()->SetSpecular(0);
	m_CutActorForLeftNurveInRotation->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInRotation))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForLeftNurveInRotation);
	}
	m_LowerLeftRendWin->Render();
}

void DentalHelper::CutLeftNurveInSagitalView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForLeftNurve->GetOutput());
	cutter->SetCutFunction(m_SagitalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForLeftNurveInSagital->SetMapper(mapper);
	m_CutActorForLeftNurveInSagital->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForLeftNurveInSagital->GetProperty()->SetAmbient(1);
	m_CutActorForLeftNurveInSagital->GetProperty()->SetSpecular(0);
	m_CutActorForLeftNurveInSagital->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInSagital))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForLeftNurveInSagital);
	}
	m_LowerRightRendWin->Render();
}
void DentalHelper::CutRightNurveInAxialView()
{
	//如果没有画右边神经则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetCutFunction(m_AxialPlane);
	cutter->SetInputData(m_TubeForRightNurve->GetOutput());
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForRightNurveInAxial->SetMapper(mapper);
	m_CutActorForRightNurveInAxial->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForRightNurveInAxial->GetProperty()->SetAmbient(1);
	m_CutActorForRightNurveInAxial->GetProperty()->SetDiffuse(0);
	m_CutActorForRightNurveInAxial->GetProperty()->SetSpecular(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInAxial))
	{
		m_UpRightRenderer->AddActor(m_CutActorForRightNurveInAxial);
	}
	m_UpRightRendWin->Render();

}
void DentalHelper::CutRightNurveInCoronalView()
{
	//如果没有画右边神经则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForRightNurve->GetOutput());
	cutter->SetCutFunction(m_CoronalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForRightNurveInCoronal->SetMapper(mapper);
	m_CutActorForRightNurveInCoronal->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForRightNurveInCoronal->GetProperty()->SetAmbient(1);
	m_CutActorForRightNurveInCoronal->GetProperty()->SetSpecular(0);
	m_CutActorForRightNurveInCoronal->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInCoronal))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForRightNurveInCoronal);
	}
	m_LowerLeftRendWin->Render();
}

void DentalHelper::CutRightNurveInCrossView()
{
	//如果没有画右边神经则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}
	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetCutFunction(m_CrossPlane);
	cutter->SetInputData(m_TubeForRightNurve->GetOutput());
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForRightNurveInCross->SetMapper(mapper);
	m_CutActorForRightNurveInCross->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForRightNurveInCross->GetProperty()->SetAmbient(1);
	m_CutActorForRightNurveInCross->GetProperty()->SetSpecular(0);
	m_CutActorForRightNurveInCross->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInCross))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForRightNurveInCross);
	}
	m_LowerRightRendWin->Render();
}



void DentalHelper::CutRightNurveInPositionView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}
	if (m_ImplantList.isEmpty())
	{
		return;
	}

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForRightNurve->GetOutput());
	cutter->SetCutFunction(m_PositionPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForRightNurveInPosition->SetMapper(mapper);
	m_CutActorForRightNurveInPosition->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForRightNurveInPosition->GetProperty()->SetAmbient(1);
	m_CutActorForRightNurveInPosition->GetProperty()->SetSpecular(0);
	m_CutActorForRightNurveInPosition->GetProperty()->SetDiffuse(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInPosition))
	{
		m_UpRightRenderer->AddActor(m_CutActorForRightNurveInPosition);
	}
	m_UpRightRendWin->Render();
}


void DentalHelper::CutRightNurveInRotationView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}
	if (m_ImplantList.isEmpty())
	{
		return;
	}
	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForRightNurve->GetOutput());
	cutter->SetCutFunction(m_RotationPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CUtActorForRightNurveInRotation->SetMapper(mapper);
	m_CUtActorForRightNurveInRotation->GetProperty()->SetColor(1, 0, 0);
	m_CUtActorForRightNurveInRotation->GetProperty()->SetAmbient(1);
	m_CUtActorForRightNurveInRotation->GetProperty()->SetSpecular(0);
	m_CUtActorForRightNurveInRotation->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CUtActorForRightNurveInRotation))
	{
		m_LowerLeftRenderer->AddActor(m_CUtActorForRightNurveInRotation);
	}
	m_LowerLeftRendWin->Render();
}

void DentalHelper::CutRightNurveInSagitalView()
{
	//如果没有画左边神经，则返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_TubeForRightNurve->GetOutput());
	cutter->SetCutFunction(m_SagitalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForRightNurveInSagital->SetMapper(mapper);
	m_CutActorForRightNurveInSagital->GetProperty()->SetColor(1, 0, 0);
	m_CutActorForRightNurveInSagital->GetProperty()->SetAmbient(1);
	m_CutActorForRightNurveInSagital->GetProperty()->SetSpecular(0);
	m_CutActorForRightNurveInSagital->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInSagital))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForRightNurveInSagital);
	}
	m_LowerRightRendWin->Render();
}


void DentalHelper::CutUpProthesisInAxialView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_UpProthesisData);
	cutter->SetCutFunction(m_AxialPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForUpProthesisInAxial->SetMapper(mapper);

	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInAxial))
	{		

		m_CutActorForUpProthesisInAxial->GetProperty()->SetColor(0, 1, 0);
		m_CutActorForUpProthesisInAxial->GetProperty()->SetAmbient(1);
		m_CutActorForUpProthesisInAxial->GetProperty()->SetSpecular(1);
		m_CutActorForUpProthesisInAxial->GetProperty()->SetDiffuse(1);
		m_UpRightRenderer->AddActor(m_CutActorForUpProthesisInAxial);
	}


}

void DentalHelper::CutUpProthesisInCoronalView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_UpProthesisData);
	cutter->SetCutFunction(m_CoronalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForUpProthesisInCoronal->SetMapper(mapper);
	m_CutActorForUpProthesisInCoronal->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForUpProthesisInCoronal->GetProperty()->SetAmbient(1);
	m_CutActorForUpProthesisInCoronal->GetProperty()->SetSpecular(0);
	m_CutActorForUpProthesisInCoronal->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCoronal))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForUpProthesisInCoronal);
	}
	m_LowerLeftRendWin->Render();
}
void DentalHelper::CutUpProthesisInCrossView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}



	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_UpProthesisData);
	cutter->SetCutFunction(m_CrossPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForUpProthesisInCross->SetMapper(mapper);
	m_CutActorForUpProthesisInCross->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForUpProthesisInCross->GetProperty()->SetAmbient(1);
	m_CutActorForUpProthesisInCross->GetProperty()->SetSpecular(0);
	m_CutActorForUpProthesisInCross->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCross))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForUpProthesisInCross);
	}
	m_LowerRightRendWin->Render();
}
void DentalHelper::CutUpProthesisInPositionView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}
	//如果没有implant 返回
	if (m_ImplantList.isEmpty())
	{
		return;
	}

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_UpProthesisData);
	cutter->SetCutFunction(m_PositionPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForUpProthesisInPosition->SetMapper(mapper);
	m_CutActorForUpProthesisInPosition->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForUpProthesisInPosition->GetProperty()->SetAmbient(1);
	m_CutActorForUpProthesisInPosition->GetProperty()->SetSpecular(0);
	m_CutActorForUpProthesisInPosition->GetProperty()->SetDiffuse(1);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInPosition))
	{
		m_UpRightRenderer->AddActor(m_CutActorForUpProthesisInPosition);
	}
	m_UpRightRendWin->Render();
}


void DentalHelper::CutUpProthesisInRotationView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}
	//如果没有implant 返回
	if (m_ImplantList.isEmpty())
	{
		return;
	}

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_UpProthesisData);
	cutter->SetCutFunction(m_RotationPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForUpProthesisInRotation->SetMapper(mapper);
	m_CutActorForUpProthesisInRotation->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForUpProthesisInRotation->GetProperty()->SetAmbient(1);
	m_CutActorForUpProthesisInRotation->GetProperty()->SetSpecular(0);
	m_CutActorForUpProthesisInRotation->GetProperty()->SetDiffuse(1);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInRotation))
	{
		m_LowerLeftRenderer->AddActor(m_CutActorForUpProthesisInRotation);
	}
	m_LowerLeftRendWin->Render();
}


void DentalHelper::CutUpProthesisInSagitalView()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}


	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetInputData(m_UpProthesisData);
	cutter->SetCutFunction(m_SagitalPlane);
	cutter->Update();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(cutter->GetOutput());

	m_CutActorForUpProthesisInSagital->SetMapper(mapper);
	m_CutActorForUpProthesisInSagital->GetProperty()->SetColor(0, 1, 0);
	m_CutActorForUpProthesisInSagital->GetProperty()->SetAmbient(1);
	m_CutActorForUpProthesisInSagital->GetProperty()->SetSpecular(0);
	m_CutActorForUpProthesisInSagital->GetProperty()->SetDiffuse(1);
	if (!m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInSagital))
	{
		m_LowerRightRenderer->AddActor(m_CutActorForUpProthesisInSagital);
	}
	m_LowerRightRendWin->Render();
}






void DentalHelper::DrawAxialLine()
{
	double Axial_CoronalPoint1[3];
	double Axial_CoronalPoint2[3];


	Axial_CoronalPoint1[0] = m_Spacing[0] * m_Extend[0] + m_Origin[0];
	Axial_CoronalPoint1[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Axial_CoronalPoint1[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];

	Axial_CoronalPoint2[0] = m_Spacing[0] * m_Extend[1] + m_Origin[0];
	Axial_CoronalPoint2[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Axial_CoronalPoint2[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];


	m_Axial_CoronalLine = vtkSmartPointer<vtkLine>::New();
	m_Axial_CoronalLine->GetPointIds()->InsertNextId(0);
	m_Axial_CoronalLine->GetPointIds()->InsertNextId(1);
	m_Axial_CoronalLine->GetPoints()->Initialize();
	m_Axial_CoronalLine->GetPoints()->InsertNextPoint(Axial_CoronalPoint1);
	m_Axial_CoronalLine->GetPoints()->InsertNextPoint(Axial_CoronalPoint2);
	auto axial_Coronalcell = vtkSmartPointer<vtkCellArray>::New();
	axial_Coronalcell->InsertNextCell(m_Axial_CoronalLine);

	m_Axial_CoronalLinePolyData = vtkSmartPointer<vtkPolyData>::New();
	m_Axial_CoronalLinePolyData->SetLines(axial_Coronalcell);
	m_Axial_CoronalLinePolyData->SetPoints(m_Axial_CoronalLine->GetPoints());

	auto axial_Coronalmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	axial_Coronalmapper->SetInputData(m_Axial_CoronalLinePolyData);
	if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_Axial_CoronalLineActor))
	{
		m_LowerLeftRenderer->RemoveActor(m_Axial_CoronalLineActor);
	}
	m_Axial_CoronalLineActor = vtkSmartPointer<vtkActor>::New();
	m_Axial_CoronalLineActor->SetMapper(axial_Coronalmapper);
	m_Axial_CoronalLineActor->GetProperty()->SetColor(AxialColor);
	m_Axial_CoronalLineActor->GetProperty()->SetLineWidth(1.5);
	m_Axial_CoronalLineActor->SetPickable(0);


	m_LowerLeftRenderer->AddActor(m_Axial_CoronalLineActor);
	m_LowerLeftRenderer->ResetCamera();
	m_LowerLeftRendWin->Render();



	double Axial_SagitalPoint1[3];
	double Axial_SagitalPoint2[3];

	Axial_SagitalPoint1[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Axial_SagitalPoint1[1] = m_Spacing[1] * m_Extend[2] + m_Origin[1];
	Axial_SagitalPoint1[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];

	Axial_SagitalPoint2[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Axial_SagitalPoint2[1] = m_Spacing[1] * m_Extend[3] + m_Origin[1];
	Axial_SagitalPoint2[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];


	m_Axial_SagitalLine = vtkSmartPointer<vtkLine>::New();
	m_Axial_SagitalLine->GetPointIds()->InsertNextId(0);
	m_Axial_SagitalLine->GetPointIds()->InsertNextId(1);
	m_Axial_SagitalLine->GetPoints()->Initialize();
	m_Axial_SagitalLine->GetPoints()->InsertNextPoint(Axial_SagitalPoint1);
	m_Axial_SagitalLine->GetPoints()->InsertNextPoint(Axial_SagitalPoint2);
	auto axial_Sagitalcell = vtkSmartPointer<vtkCellArray>::New();
	axial_Sagitalcell->InsertNextCell(m_Axial_SagitalLine);

	m_Axial_SagitalLinePolyData = vtkSmartPointer<vtkPolyData>::New();
	m_Axial_SagitalLinePolyData->SetLines(axial_Sagitalcell);
	m_Axial_SagitalLinePolyData->SetPoints(m_Axial_SagitalLine->GetPoints());

	auto axial_Sagitalmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	axial_Sagitalmapper->SetInputData(m_Axial_SagitalLinePolyData);
	if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_Axial_SagitalLineActor))
	{
		m_LowerRightRenderer->RemoveActor(m_Axial_SagitalLineActor);
	}
	m_Axial_SagitalLineActor = vtkSmartPointer<vtkActor>::New();
	m_Axial_SagitalLineActor->SetMapper(axial_Sagitalmapper);
	m_Axial_SagitalLineActor->GetProperty()->SetColor(AxialColor);
	m_Axial_SagitalLineActor->GetProperty()->SetLineWidth(1.5);
	m_Axial_SagitalLineActor->SetPickable(0);


	m_LowerRightRenderer->AddActor(m_Axial_SagitalLineActor);
	m_LowerRightRenderer->ResetCamera();
	m_LowerRightRendWin->Render();

}



void DentalHelper::DrawCoronalLine()
{
	double Coronal_AxialPoint1[3];
	double Coronal_AxialPoint2[3];

	Coronal_AxialPoint1[0] = m_Spacing[0] * m_Extend[0] + m_Origin[0];
	Coronal_AxialPoint1[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_AxialPoint1[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	Coronal_AxialPoint2[0] = m_Spacing[0] * m_Extend[1] + m_Origin[0];
	Coronal_AxialPoint2[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_AxialPoint2[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];



	m_Coronal_AxialLine = vtkSmartPointer<vtkLine>::New();
	m_Coronal_AxialLine->GetPointIds()->InsertNextId(0);
	m_Coronal_AxialLine->GetPointIds()->InsertNextId(1);
	m_Coronal_AxialLine->GetPoints()->Initialize();
	m_Coronal_AxialLine->GetPoints()->InsertNextPoint(Coronal_AxialPoint1);
	m_Coronal_AxialLine->GetPoints()->InsertNextPoint(Coronal_AxialPoint2);

	auto Coronal_Axialcell = vtkSmartPointer<vtkCellArray>::New();
	Coronal_Axialcell->InsertNextCell(m_Coronal_AxialLine);

	m_Coronal_AxialLinePolyData = vtkSmartPointer<vtkPolyData>::New();
	m_Coronal_AxialLinePolyData->SetLines(Coronal_Axialcell);
	m_Coronal_AxialLinePolyData->SetPoints(m_Coronal_AxialLine->GetPoints());

	auto Coronal_Axialmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	Coronal_Axialmapper->SetInputData(m_Coronal_AxialLinePolyData);
	if (m_UpRightRenderer->GetActors()->IsItemPresent(m_Coronal_AxialLineActor))
	{
		m_UpRightRenderer->RemoveActor(m_Coronal_AxialLineActor);
	}
	m_Coronal_AxialLineActor = vtkSmartPointer<vtkActor>::New();
	m_Coronal_AxialLineActor->SetMapper(Coronal_Axialmapper);
	m_Coronal_AxialLineActor->GetProperty()->SetColor(CoronalColor);
	m_Coronal_AxialLineActor->GetProperty()->SetLineWidth(1.5);
	m_Coronal_AxialLineActor->SetPickable(0);

	m_UpRightRenderer->AddActor(m_Coronal_AxialLineActor);
	m_UpRightRenderer->ResetCamera();
	m_UpRightRendWin->Render();



	double Coronal_SagitalPoint1[3];
	double Coronal_SagitalPoint2[3];

	Coronal_SagitalPoint1[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Coronal_SagitalPoint1[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_SagitalPoint1[2] = m_Spacing[2] * m_Extend[4] + m_Origin[2];

	Coronal_SagitalPoint2[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Coronal_SagitalPoint2[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_SagitalPoint2[2] = m_Spacing[2] * m_Extend[5] + m_Origin[2];


	m_Coronal_SagitalLine = vtkSmartPointer<vtkLine>::New();
	m_Coronal_SagitalLine->GetPointIds()->InsertNextId(0);
	m_Coronal_SagitalLine->GetPointIds()->InsertNextId(1);
	m_Coronal_SagitalLine->GetPoints()->Initialize();
	m_Coronal_SagitalLine->GetPoints()->InsertNextPoint(Coronal_SagitalPoint1);
	m_Coronal_SagitalLine->GetPoints()->InsertNextPoint(Coronal_SagitalPoint2);

	auto Coronal_Sagitalcell = vtkSmartPointer<vtkCellArray>::New();
	Coronal_Sagitalcell->InsertNextCell(m_Coronal_SagitalLine);

	m_Coronal_SagitalLinePolyData = vtkSmartPointer<vtkPolyData>::New();
	m_Coronal_SagitalLinePolyData->SetLines(Coronal_Sagitalcell);
	m_Coronal_SagitalLinePolyData->SetPoints(m_Coronal_SagitalLine->GetPoints());

	auto Coronal_Sagtialmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	Coronal_Sagtialmapper->SetInputData(m_Coronal_SagitalLinePolyData);
	if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_Coronal_SagitalLineActor))
	{
		m_LowerRightRenderer->RemoveActor(m_Coronal_SagitalLineActor);
	}
	m_Coronal_SagitalLineActor = vtkSmartPointer<vtkActor>::New();
	m_Coronal_SagitalLineActor->SetMapper(Coronal_Sagtialmapper);
	m_Coronal_SagitalLineActor->GetProperty()->SetColor(CoronalColor);
	m_Coronal_SagitalLineActor->GetProperty()->SetLineWidth(1.5);
	m_Coronal_SagitalLineActor->SetPickable(0);

	m_LowerRightRenderer->AddActor(m_Coronal_SagitalLineActor);
	m_LowerRightRenderer->ResetCamera();
	m_LowerRightRendWin->Render();
}


void DentalHelper::DrawSagitalLine()
{
	double Sagital_AxialPoint1[3];
	double Sagital_AxialPoint2[3];

	Sagital_AxialPoint1[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_AxialPoint1[1] = m_Spacing[1] * m_Extend[2] + m_Origin[1];
	Sagital_AxialPoint1[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	Sagital_AxialPoint2[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_AxialPoint2[1] = m_Spacing[1] * m_Extend[3] + m_Origin[1];
	Sagital_AxialPoint2[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	m_Sagital_AxialLine = vtkSmartPointer<vtkLine>::New();
	m_Sagital_AxialLine->GetPointIds()->InsertNextId(0);
	m_Sagital_AxialLine->GetPointIds()->InsertNextId(1);
	m_Sagital_AxialLine->GetPoints()->Initialize();
	m_Sagital_AxialLine->GetPoints()->InsertNextPoint(Sagital_AxialPoint1);
	m_Sagital_AxialLine->GetPoints()->InsertNextPoint(Sagital_AxialPoint2);
	auto Sagital_Axialcell = vtkSmartPointer<vtkCellArray>::New();
	Sagital_Axialcell->InsertNextCell(m_Sagital_AxialLine);



	m_Sagital_AxialLinePolyData = vtkSmartPointer<vtkPolyData>::New();
	m_Sagital_AxialLinePolyData->SetLines(Sagital_Axialcell);
	m_Sagital_AxialLinePolyData->SetPoints(m_Sagital_AxialLine->GetPoints());

	auto Sagital_Axialmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	Sagital_Axialmapper->SetInputData(m_Sagital_AxialLinePolyData);
	if (m_UpRightRenderer->GetActors()->IsItemPresent(m_Sagital_AxialLineActor))
	{
		m_UpRightRenderer->RemoveActor(m_Sagital_AxialLineActor);
	}
	m_Sagital_AxialLineActor = vtkSmartPointer<vtkActor>::New();
	m_Sagital_AxialLineActor->SetMapper(Sagital_Axialmapper);
	m_Sagital_AxialLineActor->GetProperty()->SetColor(SagitalColor);
	m_Sagital_AxialLineActor->GetProperty()->SetLineWidth(1.5);
	m_Sagital_AxialLineActor->SetPickable(0);

	m_UpRightRenderer->AddActor(m_Sagital_AxialLineActor);
	m_UpRightRenderer->ResetCamera();
	m_UpRightRendWin->Render();

	double Sagital_CoronalPoint1[3];
	double Sagital_CoronalPoint2[3];
	Sagital_CoronalPoint1[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_CoronalPoint1[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Sagital_CoronalPoint1[2] = m_Spacing[2] * m_Extend[4] + m_Origin[2];

	Sagital_CoronalPoint2[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_CoronalPoint2[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Sagital_CoronalPoint2[2] = m_Spacing[2] * m_Extend[5] + m_Origin[2];


	m_Sagital_CoronalLine = vtkSmartPointer<vtkLine>::New();
	m_Sagital_CoronalLine->GetPointIds()->InsertNextId(0);
	m_Sagital_CoronalLine->GetPointIds()->InsertNextId(1);
	m_Sagital_CoronalLine->GetPoints()->Initialize();
	m_Sagital_CoronalLine->GetPoints()->InsertNextPoint(Sagital_CoronalPoint1);
	m_Sagital_CoronalLine->GetPoints()->InsertNextPoint(Sagital_CoronalPoint2);
	auto Sagital_Coronalcell = vtkSmartPointer<vtkCellArray>::New();
	Sagital_Coronalcell->InsertNextCell(m_Sagital_CoronalLine);



	m_Sagital_CoronalLinePolyData = vtkSmartPointer<vtkPolyData>::New();
	m_Sagital_CoronalLinePolyData->SetLines(Sagital_Coronalcell);
	m_Sagital_CoronalLinePolyData->SetPoints(m_Sagital_CoronalLine->GetPoints());

	auto Sagital_Coronalmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	Sagital_Coronalmapper->SetInputData(m_Sagital_CoronalLinePolyData);
	if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_Sagital_CoronalLineActor))
	{
		m_LowerLeftRenderer->RemoveActor(m_Sagital_CoronalLineActor);
	}
	m_Sagital_CoronalLineActor = vtkSmartPointer<vtkActor>::New();
	m_Sagital_CoronalLineActor->SetMapper(Sagital_Coronalmapper);
	m_Sagital_CoronalLineActor->GetProperty()->SetColor(SagitalColor);
	m_Sagital_CoronalLineActor->GetProperty()->SetLineWidth(1.5);
	m_Sagital_CoronalLineActor->SetPickable(0);

	m_LowerLeftRenderer->AddActor(m_Sagital_CoronalLineActor);
	m_LowerLeftRenderer->ResetCamera();
	m_LowerLeftRendWin->Render();
	
}
void DentalHelper::GenerateContourInitialData(vtkPoints* points, vtkPolyData* initialData)
{
	auto idlist = vtkSmartPointer<vtkIdList>::New();
	idlist->Initialize();
	auto cellArrary = vtkSmartPointer<vtkCellArray>::New();
	cellArrary->Initialize();
	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		idlist->InsertNextId(i);
	}
	cellArrary->InsertNextCell(idlist);
	initialData->SetPoints(points);
	initialData->SetLines(cellArrary);
}


void DentalHelper::GenerateContourWidgetPolydata()
{
	QSettings widgetPoints;
	int numOfPoints;
	//判断是否存在上次选择的点
	if (widgetPoints.value("numOfPoints").isValid())
	{
		numOfPoints = widgetPoints.value("numOfPoints").toInt();
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->Initialize();
		auto cellArrary = vtkSmartPointer<vtkCellArray>::New();
		cellArrary->Initialize();
		auto idlist = vtkSmartPointer<vtkIdList>::New();
		idlist->Initialize();
		double sumx = 0;
		double sumy = 0;
		double sumz = 0;
		for (int i = 0; i < numOfPoints; i++)
		{
			double temp[3];
			QString nameOfPoint = "Point";
			QString nameOfIndex;
			temp[0] = widgetPoints.value(nameOfPoint + nameOfIndex.setNum(i) + "x").toDouble();
			temp[1] = widgetPoints.value(nameOfPoint + nameOfIndex.setNum(i) + "y").toDouble();
			temp[2] = widgetPoints.value(nameOfPoint + nameOfIndex.setNum(i) + "z").toDouble();
			points->InsertNextPoint(temp);
			idlist->InsertNextId(i);
			sumx = sumx + temp[0]; sumy = sumy + temp[1]; sumz = sumz+ temp[2];
		}
		cellArrary->InsertNextCell(idlist);
		auto formerData = vtkSmartPointer<vtkPolyData>::New();
		formerData->SetPoints(points);
		formerData->SetLines(cellArrary);
		double centerOfPoints[3];
		centerOfPoints[0] = sumx / numOfPoints; centerOfPoints[1]= sumy / numOfPoints;  centerOfPoints[2] = sumz/ numOfPoints;
		double translateDistance[3];
		for (int i=0;i<3;i++)
		{
			translateDistance[i] =m_PlaneWidgetForArchCurve->GetCenter()[i] - centerOfPoints[i];
		}
		auto transform = vtkSmartPointer<vtkTransform>::New();
		transform->Translate(translateDistance);
		transform->Update();
		auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformFilter->SetTransform(transform);
		transformFilter->SetInputData(formerData);
		transformFilter->Update();
		m_ContourInitialData = transformFilter->GetOutput();

	}
	else
	{
		auto plane = vtkSmartPointer<vtkPlane>::New();
		m_PlaneWidgetForArchCurve->GetPlane(plane);
		int numPts = 5;
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->Initialize();
		auto idlist = vtkSmartPointer<vtkIdList>::New();
		idlist->Initialize();
		auto cellArrary = vtkSmartPointer<vtkCellArray>::New();
		cellArrary->Initialize();
		for (int i = 0; i < numPts; i++)
		{
			// Create numPts points evenly spread around a circumference of radius 10

			const double angle = vtkMath::Pi()*i / numPts;
			double p[3];
			p[0] = 10 * cos(angle) + m_PlaneWidgetForArchCurve->GetCenter()[0];
			p[1] = 10 * sin(angle) + m_PlaneWidgetForArchCurve->GetCenter()[1];
			p[3] = 0;
			plane->ProjectPoint(p, p);
			points->InsertNextPoint(p);
			idlist->InsertNextId(i);
		}
		// Create a cell array to connect the points into meaningful geometry
		cellArrary->InsertNextCell(idlist);
		m_ContourInitialData->SetPoints(points);
		m_ContourInitialData->SetLines(cellArrary);
	}

}
void DentalHelper::GenerateCrossReslice()
{
	//获得曲线上所有的点
	auto splinePoints = vtkSmartPointer<vtkPoints>::New();
	splinePoints = m_ContourSplineFilter->GetOutput()->GetPoints();

	//获得牙弓曲线所在平面
	auto plane = vtkSmartPointer<vtkPlane>::New();
	m_PlaneWidgetForArchCurve->GetPlane(plane);

	//获得当前点
	double point1[3], point2[3];
	splinePoints->GetPoint(ui.LowerRightSlider->value(), point1);
	splinePoints->GetPoint(ui.LowerRightSlider->value()+1, point2);
	//投影到平面上
	plane->ProjectPoint(point1, point1);
	plane->ProjectPoint(point2, point2);
	//计算cross plane的法向量
	double normalOfCrossPlane_[3];
	for (int i=0;i<3;i++)
	{
		normalOfCrossPlane_[i] = point1[i] - point2[i];
	}
	vtkMath::Normalize(normalOfCrossPlane_);
	m_CrossPlane->SetOrigin(point1);
	m_CrossPlane->SetNormal(normalOfCrossPlane_);

	//设置slice in model view
	auto propertySlice = vtkSmartPointer<vtkImageProperty>::New();
	propertySlice->SetColorWindow(ui.WindowSlider->value());
	propertySlice->SetColorLevel(ui.LevelSlider->value());
	propertySlice->SetAmbient(1);
	propertySlice->SetDiffuse(1.0);
	propertySlice->SetOpacity(1.0);
	propertySlice->SetInterpolationTypeToLinear();

	m_ResliceMapperOfCrossInModel->SetInputData(m_ImageData);
	m_ResliceMapperOfCrossInModel->SetSlicePlane(m_CrossPlane);

	m_ReslicePropOfCrossInModel->SetMapper(m_ResliceMapperOfCrossInModel);
	m_ReslicePropOfCrossInModel->SetProperty(propertySlice);
	if (!m_ModelRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfCrossInModel))
	{
		m_ModelRenderer->AddViewProp(m_ReslicePropOfCrossInModel);
	}
	m_ModelRendWin->Render();
	//设置slice in 2D view
	m_ResliceMapperOfCrossIn2D->SetInputData(m_ImageData);
	m_ResliceMapperOfCrossIn2D->SetSlicePlane(m_CrossPlane);
	
	m_ReslicePropOfCrossIn2D->SetMapper(m_ResliceMapperOfCrossIn2D);
	m_ReslicePropOfCrossIn2D->SetProperty(propertySlice);
	if (!m_LowerRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfCrossIn2D))
	{
		m_LowerRightRenderer->AddViewProp(m_ReslicePropOfCrossIn2D);
	}

	//切割种植体
	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInLowerRightView(m_CrossPlane);
	}

	this->UpDateCamera(m_LowerRightRenderer, normalOfCrossPlane_, 90);
	m_LowerRightRendWin->Render();

	this->CutLeftNurveInCrossView();//切割左边神经
	this->CutRightNurveInCrossView();//切割右边神经
	this->CutDownProthesisInCrossView();//切割下颌假牙模型
	this->CutUpProthesisInCrossView();//切割上颌假牙模型

}

void DentalHelper::GenerateOffSetSpline()
{
	//获得牙弓曲线所在平面
	auto planeForArchCurve = vtkSmartPointer<vtkPlane>::New();
	m_PlaneWidgetForArchCurve->GetPlane(planeForArchCurve);
	//获得contour widget的点
	auto polydataOfOffSet = vtkSmartPointer<vtkPolyData>::New();
	polydataOfOffSet->DeepCopy(m_ContourRepForArchCurve->GetContourRepresentationAsPolyData());
	auto pointsOfContour_ = vtkSmartPointer<vtkPoints>::New();
	pointsOfContour_ = polydataOfOffSet->GetPoints();
	int numOfPointsOfContour = pointsOfContour_->GetNumberOfPoints();	

	//offset的点
	auto pointsOfOffSet = vtkSmartPointer<vtkPoints>::New();
	pointsOfOffSet->Initialize();
	for (int i = 0; i < numOfPointsOfContour; i++)
	{
		double firstPoint_[3], nextPoint_[3], normalOfPoints[3], normalOfOffset[3];
		double newPoint[3];
		if (i < numOfPointsOfContour - 1)//如果不是最后一个点
		{
			pointsOfContour_->GetPoint(i, firstPoint_);
			pointsOfContour_->GetPoint(i + 1, nextPoint_);
			planeForArchCurve->ProjectPoint(firstPoint_, firstPoint_);
			planeForArchCurve->ProjectPoint(nextPoint_, nextPoint_);
			vtkMath::Subtract(nextPoint_, firstPoint_, normalOfPoints);
			vtkMath::Normalize(normalOfPoints);
			vtkMath::Cross(m_PlaneWidgetForArchCurve->GetNormal(), normalOfPoints, normalOfOffset);
			for (int j = 0; j < 3; j++)
			{
				newPoint[j] = firstPoint_[j] + normalOfOffset[j] * 0.1*ui.LowerLeftSlider->value();
			}
		}
		else
		{
			pointsOfContour_->GetPoint(i - 1, firstPoint_);
			pointsOfContour_->GetPoint(i, nextPoint_);
			planeForArchCurve->ProjectPoint(firstPoint_, firstPoint_);
			planeForArchCurve->ProjectPoint(nextPoint_, nextPoint_);

			vtkMath::Subtract(nextPoint_, firstPoint_, normalOfPoints);
			vtkMath::Normalize(normalOfPoints);
			vtkMath::Cross(m_PlaneWidgetForArchCurve->GetNormal(), normalOfPoints, normalOfOffset);
			for (int j = 0; j < 3; j++)
			{
				newPoint[j] = nextPoint_[j] + normalOfOffset[j] * 0.1*ui.LowerLeftSlider->value();
			}
		}	
		pointsOfOffSet->InsertNextPoint(newPoint);
	}

	polydataOfOffSet->SetPoints(pointsOfOffSet);

	auto cardinalSplineOffSet = vtkSmartPointer<vtkCardinalSpline>::New();
	cardinalSplineOffSet->SetLeftConstraint(2);
	cardinalSplineOffSet->SetRightConstraint(2);
	cardinalSplineOffSet->SetLeftValue(0);
	cardinalSplineOffSet->SetRightValue(0);
	m_ContourOffSetSplineFilter->SetSpline(cardinalSplineOffSet);
	m_ContourOffSetSplineFilter->SetInputData(polydataOfOffSet);
	m_ContourOffSetSplineFilter->SetSubdivideToSpecified();
	m_ContourOffSetSplineFilter->SetNumberOfSubdivisions(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 1);
	m_ContourOffSetSplineFilter->Update();

	auto offsetMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	offsetMapper->SetInputData(m_ContourOffSetSplineFilter->GetOutput());
	offsetMapper->ScalarVisibilityOff();
	m_ContourOffSetActor->SetMapper(offsetMapper);
	m_ContourOffSetActor->GetProperty()->SetColor(0.2, 0.8, 0.5);
	m_ContourOffSetActor->GetProperty()->SetLineWidth(3);
	if (!m_UpRightRenderer->GetActors()->IsItemPresent(m_ContourOffSetActor))
	{
		m_UpRightRenderer->AddActor(m_ContourOffSetActor);
	}
	m_UpRightRendWin->Render();
}

void DentalHelper::GeneratePanaromicReslice()
{
	//获得widget 的法向量，以此决定移动的方向
	double normalOfPlaneWidget_[3];
	m_PlaneWidgetForArchCurve->GetNormal(normalOfPlaneWidget_);
	double movedDistance = 50;
	double translateDistance[3];
	for (int i=0;i<3;i++)
	{
		translateDistance[i] = movedDistance*normalOfPlaneWidget_[i];
	}
	auto upTransform = vtkSmartPointer<vtkTransform>::New();
	upTransform->Translate(translateDistance);
	upTransform->Update();
	auto upTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	upTransformFilter->SetInputData(m_ContourOffSetSplineFilter->GetOutput());
	upTransformFilter->SetTransform(upTransform);
	upTransformFilter->Update();
	auto upPoints = vtkSmartPointer<vtkPoints>::New();
	upPoints = upTransformFilter->GetOutput()->GetPoints();

	auto downTransform = vtkSmartPointer<vtkTransform>::New();
	downTransform->Translate(-translateDistance[0], -translateDistance[1], -translateDistance[2]);
	downTransform->Update();
	auto downTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	downTransformFilter->SetInputData(m_ContourOffSetSplineFilter->GetOutput());
	downTransformFilter->SetTransform(downTransform);
	downTransformFilter->Update();

	auto downPoints = vtkSmartPointer<vtkPoints>::New();
	downPoints = downTransformFilter->GetOutput()->GetPoints();

	auto ruledSurfaceData = vtkSmartPointer<vtkPolyData>::New();
	this->GenerateRuledSurface(upPoints,downPoints , ruledSurfaceData);

	auto ruledSurfaceFilter = vtkSmartPointer<vtkRuledSurfaceFilter>::New();
	ruledSurfaceFilter->SetInputData(ruledSurfaceData);
	ruledSurfaceFilter->SetResolution(1, 400);
	ruledSurfaceFilter->SetRuledModeToResample();
	ruledSurfaceFilter->Update();

	m_PanaromicProbeFilterInModel->SetInputData(ruledSurfaceFilter->GetOutput());
	m_PanaromicProbeFilterInModel->SetSourceData(m_ImageData);
	m_PanaromicProbeFilterInModel->Update();

	auto windowLevel = vtkSmartPointer<vtkWindowLevelLookupTable>::New();
	windowLevel->SetRampToLinear();
	windowLevel->SetNumberOfColors(1000);
	//windowLevel->SetWindow(5000);
	//windowLevel->SetLevel(200);	
	windowLevel->SetTableRange(-1024, 3096);
	windowLevel->Build();
	auto panaromicMapperInModel = vtkSmartPointer<vtkPolyDataMapper>::New();
	panaromicMapperInModel->SetInputData(m_PanaromicProbeFilterInModel->GetPolyDataOutput());
	panaromicMapperInModel->SetLookupTable(windowLevel);
	panaromicMapperInModel->SetColorModeToMapScalars();
	panaromicMapperInModel->UseLookupTableScalarRangeOn();

	m_PanaromicActorInModel->SetMapper(panaromicMapperInModel);
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_PanaromicActorInModel))
	{
		m_ModelRenderer->AddActor(m_PanaromicActorInModel);
	}
	m_PanaromicActorForDrawingNurve->SetMapper(panaromicMapperInModel);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_PanaromicActorForDrawingNurve))
	{
		m_LowerLeftRenderer->AddActor(m_PanaromicActorForDrawingNurve);
	}
	if (ui.Panaromic2DRadioButton->isChecked())
	{
		m_PanaromicActorForDrawingNurve->VisibilityOff();
	}
	else
	{
		m_PanaromicActorForDrawingNurve->VisibilityOn();
	}
	m_LowerLeftRendWin->Render();
	m_ModelRendWin->Render();

	


}

void DentalHelper::GeneratePanaromicReslice2D()
{
	//获得spline的长度
	auto points = vtkSmartPointer<vtkPoints>::New();
	points = m_ContourOffSetSplineFilter->GetOutput()->GetPoints();
	int num = points->GetNumberOfPoints();
	double distanceOfSpline = 0;
	for (int i = 0; i < num - 1; i++)
	{
		double temp[3];
		double temp1[3];
		points->GetPoint(i, temp);
		points->GetPoint(i + 1, temp1);
		distanceOfSpline = distanceOfSpline + sqrt(vtkMath::Distance2BetweenPoints(temp, temp1));
	}

	double startPoint_[3] = { 0,0,0 };
	double endPoint_[3] = { distanceOfSpline,0,0 };
	auto lineSource = vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(startPoint_);
	lineSource->SetPoint2(endPoint_);
	lineSource->Update();

	auto cardinalSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	cardinalSpline->SetLeftConstraint(2);
	cardinalSpline->SetRightConstraint(2);
	cardinalSpline->SetLeftValue(0);
	cardinalSpline->SetRightValue(0);

	m_ContourOffSetSplineFilter2D->SetSpline(cardinalSpline);
	m_ContourOffSetSplineFilter2D->SetInputData(lineSource->GetOutput());
	m_ContourOffSetSplineFilter2D->SetSubdivideToSpecified();
	m_ContourOffSetSplineFilter2D->SetNumberOfSubdivisions(m_ContourOffSetSplineFilter->GetNumberOfSubdivisions());
	m_ContourOffSetSplineFilter2D->Update();

	auto upTransform = vtkSmartPointer<vtkTransform>::New();
	upTransform->Translate(0, 0, 50);
	upTransform->Update();
	auto upTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	upTransformFilter->SetInputData(m_ContourOffSetSplineFilter2D->GetOutput());
	upTransformFilter->SetTransform(upTransform);
	upTransformFilter->Update();

	auto ruledPoints1 = vtkSmartPointer<vtkPoints>::New();
	ruledPoints1 = upTransformFilter->GetOutput()->GetPoints();

	auto downTranform = vtkSmartPointer<vtkTransform>::New();
	downTranform->Translate(0, 0, -50);
	downTranform->Update();

	auto downTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	downTransformFilter->SetInputData(m_ContourOffSetSplineFilter2D->GetOutput());
	downTransformFilter->SetTransform(downTranform);
	downTransformFilter->Update();

	auto ruledPoints2 = vtkSmartPointer<vtkPoints>::New();
	ruledPoints2 = downTransformFilter->GetOutput()->GetPoints();

	auto ruledSourceData = vtkSmartPointer<vtkPolyData>::New();
	this->GenerateRuledSurface(ruledPoints1, ruledPoints2, ruledSourceData);

	auto ruledSurfaceFilter = vtkSmartPointer<vtkRuledSurfaceFilter>::New();
	ruledSurfaceFilter->SetInputData(ruledSourceData);
	ruledSurfaceFilter->SetResolution(1, 400);
	ruledSurfaceFilter->SetRuledModeToResample();
	ruledSurfaceFilter->Update();

	auto panaromicData = vtkSmartPointer<vtkPolyData>::New();
	panaromicData = ruledSurfaceFilter->GetOutput();
	panaromicData->GetPointData()->SetScalars(m_PanaromicProbeFilterInModel->GetOutput()->GetPointData()->GetScalars());

	auto windowLevel = vtkSmartPointer<vtkWindowLevelLookupTable>::New();
	//windowLevel->SetWindow(ui.WindowSlider->value());
	//windowLevel->SetLevel(ui.LevelSlider->value());
	windowLevel->SetRampToLinear();
	windowLevel->SetNumberOfColors(100);
	windowLevel->SetTableRange(-1024, 3096);
	windowLevel->Build();

	auto panaromicMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	panaromicMapper->SetInputData(panaromicData);
	panaromicMapper->SetColorModeToMapScalars();
	panaromicMapper->SetLookupTable(windowLevel);
	panaromicMapper->UseLookupTableScalarRangeOn();

	m_PanaromicActorIn2D->SetMapper(panaromicMapper);
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_PanaromicActorIn2D))
	{
		m_LowerLeftRenderer->AddActor(m_PanaromicActorIn2D);
	}
	double cameraNormal[3] = { 0,1,0 };
	if (ui.Panaromic2DRadioButton->isChecked())
	{	
		this->UpDateCamera(m_LowerLeftRenderer, cameraNormal, 90);
	}

}
void DentalHelper::GeneratePositionReslice()
{
	//设置移动平面
	double centerOfPosition_[3];
	for (int i=0;i<3;i++)
	{
		centerOfPosition_[i] = m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint[i]- m_ImplantList.at(m_CurrentImplant)->m_NormalOfTube[i] * ui.UpRightSlider->value()*0.5;
	}
	m_PositionPlane->SetNormal(m_ImplantList.at(m_CurrentImplant)->m_NormalOfTube);
	m_PositionPlane->SetOrigin(centerOfPosition_);

	auto propertySlice = vtkSmartPointer<vtkImageProperty>::New();
	propertySlice->SetColorWindow(ui.WindowSlider->value());
	propertySlice->SetColorLevel(ui.LevelSlider->value());
	propertySlice->SetAmbient(1);
	propertySlice->SetDiffuse(1.0);
	propertySlice->SetOpacity(1.0);
	propertySlice->SetInterpolationTypeToLinear();

	m_ResliceMapperOfPositionIn2D->SetInputData(m_ImageData);
	m_ResliceMapperOfPositionIn2D->SetSlicePlane(m_PositionPlane);

	m_ResliceMapperOfPositionIn3D->SetInputData(m_ImageData);
	m_ResliceMapperOfPositionIn3D->SetSlicePlane(m_PositionPlane);

	m_ReslicePropOfPositionIn3D->SetMapper(m_ResliceMapperOfPositionIn3D);
	m_ReslicePropOfPositionIn3D->SetProperty(propertySlice);
	m_ReslicePropOfPositionIn2D->SetMapper(m_ResliceMapperOfPositionIn2D);
	m_ReslicePropOfPositionIn2D->SetProperty(propertySlice);
	if (!m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfPositionIn2D))
	{
		m_UpRightRenderer->AddViewProp(m_ReslicePropOfPositionIn2D);

	}
	if (!m_ModelRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfPositionIn3D))
	{
		m_ModelRenderer->AddViewProp(m_ReslicePropOfPositionIn3D);
	}
	m_ModelRendWin->Render();
	this->UpDateCamera(m_UpRightRenderer, m_PositionPlane->GetNormal(), 90);
}


void DentalHelper::GenerateRotationReslice()
{
	//设置旋转平面
	double normalOfRotation_[3];
	double degreeInRadians = vtkMath::RadiansFromDegrees(double(ui.LowerLeftSlider->value()));
	vtkMath::Perpendiculars(m_ImplantList.at(m_CurrentImplant)->m_NormalOfTube, normalOfRotation_, NULL, degreeInRadians);
	m_RotationPlane->SetOrigin(m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint);
	m_RotationPlane->SetNormal(normalOfRotation_);
	//为各个种植体的基台定义平面
	for each (ImplantItem* var in m_ImplantList)
	{
		var->SetRotatePlane(m_RotationPlane);
	}

	auto propertySlice = vtkSmartPointer<vtkImageProperty>::New();
	propertySlice->SetColorWindow(ui.WindowSlider->value());
	propertySlice->SetColorLevel(ui.LevelSlider->value());
	propertySlice->SetAmbient(1);
	propertySlice->SetDiffuse(1.0);
	propertySlice->SetOpacity(1.0);
	propertySlice->SetInterpolationTypeToLinear();

	m_ResliceMapperOfRotationIn2D->SetInputData(m_ImageData);
	m_ResliceMapperOfRotationIn2D->SetSlicePlane(m_RotationPlane);

	m_ReslicePropOfRotationIn2D->SetMapper(m_ResliceMapperOfRotationIn2D);
	m_ReslicePropOfRotationIn2D->SetProperty(propertySlice);

	m_ResliceMapperOfRotationIn3D->SetInputData(m_ImageData);
	m_ResliceMapperOfRotationIn3D->SetSlicePlane(m_RotationPlane);
	m_ReslicePropOfRotationIn3D->SetMapper(m_ResliceMapperOfRotationIn3D);
	m_ReslicePropOfRotationIn3D->SetProperty(propertySlice);

	if (!m_LowerLeftRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfRotationIn2D))
	{
		m_LowerLeftRenderer->AddViewProp(m_ReslicePropOfRotationIn2D);
	}
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ReslicePropOfRotationIn3D))
	{
		m_ModelRenderer->AddActor(m_ReslicePropOfRotationIn3D);
	}
	m_ModelRendWin->Render();
	if (!m_ImplantList.isEmpty())
	{
		this->UpDateCamera(m_LowerLeftRenderer, normalOfRotation_, 90,m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint);
	}
	else
	{
		this->UpDateCamera(m_LowerLeftRenderer, normalOfRotation_, 90);
	}



}



void DentalHelper::GenerateRuledSurface(vtkPoints* point1, vtkPoints* point2,vtkPolyData* output)
{
	auto points =vtkSmartPointer<vtkPoints>::New();
	points->Initialize();

	int numOflines = point1->GetNumberOfPoints();
	for (int i = 0; i < numOflines / 2.0 - 0.5; i++)
	{
		points->InsertNextPoint(point1->GetPoint(2 * i));
		points->InsertNextPoint(point1->GetPoint(2 * i + 1));
		points->InsertNextPoint(point2->GetPoint(2 * i));
		points->InsertNextPoint(point2->GetPoint(2 * i + 1));
	}

	if (numOflines % 2 == 1)
	{
		points->InsertNextPoint(point1->GetPoint(numOflines - 1));
		points->InsertNextPoint(point2->GetPoint(numOflines - 1));
	}

	auto lines =vtkSmartPointer<vtkCellArray>::New();
	int prePoint1 = 0;
	int prePoint2 = 0;
	for (int i = 0; i < numOflines - 1; i++)
	{
		auto line1 =vtkSmartPointer<vtkLine>::New();
		if (i == 0)
		{
			line1->GetPointIds()->SetId(0, 2 * i);
			line1->GetPointIds()->SetId(1, 2 * i + 1);
			prePoint1 = 2 * i + 1;
		}
		else {
			line1->GetPointIds()->SetId(0, prePoint1);
			if (i % 2 == 1)
			{
				line1->GetPointIds()->SetId(1, prePoint1 + 3);
				prePoint1 = prePoint1 + 3;
			}
			else
			{
				line1->GetPointIds()->SetId(1, prePoint1 + 1);
				prePoint1 = prePoint1 + 1;
			}
		}
		lines->InsertNextCell(line1);

		auto line2 =vtkSmartPointer<vtkLine>::New();
		if (i == 0)
		{
			line2->GetPointIds()->SetId(0, 2 * i + 2);
			line2->GetPointIds()->SetId(1, 2 * i + 3);
			prePoint2 = 2 * i + 3;
		}
		else
		{
			line2->GetPointIds()->SetId(0, prePoint2);
			if (numOflines % 2 == 1 && i == numOflines - 2)
			{
				line2->GetPointIds()->SetId(1, prePoint2 + 2);
			}
			else if (i % 2 == 1)
			{
				line2->GetPointIds()->SetId(1, prePoint2 + 3);
				prePoint2 = prePoint2 + 3;
			}
			else
			{
				line2->GetPointIds()->SetId(1, prePoint2 + 1);
				prePoint2 = prePoint2 + 1;
			}

		}
		lines->InsertNextCell(line2);

	}		
	auto polydata =vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(points);
	polydata->SetLines(lines);
	output->DeepCopy(polydata);
}

void DentalHelper::GetRandomColor(double* rgb, int differ)
{
	QTime time = QTime::currentTime();//获取系统现在的时间
	auto randomNumGenerator = vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();
	randomNumGenerator->SetSeed(time.msec()*differ);

	rgb[0] = randomNumGenerator->GetValue();
	randomNumGenerator->Next();
	rgb[1] = randomNumGenerator->GetValue();
	randomNumGenerator->Next();
	rgb[2] = randomNumGenerator->GetValue();
}
void DentalHelper::InitializeView()
{
	//初始化三维视图
	m_ModelRenderer = vtkSmartPointer<vtkRenderer>::New();
	m_ModelRendWin = vtkSmartPointer<vtkRenderWindow>::New();
	m_ModelInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_ModelInterStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	double color1[3], color2[3];
	this->GetRandomColor(color1, 1);
	this->GetRandomColor(color2, 99);
	//设置为渐变色
	m_ModelRenderer->SetGradientBackground(true);
	m_ModelRenderer->SetBackground(color1);
	m_ModelRenderer->SetBackground2(color2);

	//设置按钮的颜色
	this->SetButtonColor(ui.BackGround1Button, color1);
	this->SetButtonColor(ui.BackGround2Button, color2);

	m_ModelRendWin->AddRenderer(m_ModelRenderer);
	m_ModelInteractor->SetRenderWindow(m_ModelRendWin);
	m_ModelInteractor->SetInteractorStyle(m_ModelInterStyle);

	ui.ModelQvtkWidget->SetRenderWindow(m_ModelRendWin);
	m_ModelInteractor->Initialize();
	double modeltextColor[3];
	this->GetRandomColor(modeltextColor,999);
	this->SetWindowText(m_ModelRenderer, "Model View", modeltextColor);
	//设置方向widget
	AxesActor = vtkSmartPointer<vtkAxesActor>::New();
	CubeActor = vtkSmartPointer<vtkAnnotatedCubeActor>::New();

	auto probAssemble = vtkSmartPointer<vtkPropAssembly>::New();
	probAssemble->AddPart(AxesActor);
	probAssemble->AddPart(CubeActor);

	OrientationWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	OrientationWidget->SetOrientationMarker(probAssemble);
	OrientationWidget->SetInteractor(m_ModelInteractor);
	OrientationWidget->SetViewport(0.7, 0.1, 0.9, 0.4);
	OrientationWidget->SetEnabled(1);
	OrientationWidget->InteractiveOn();
	//初始化右上角视图
	m_UpRightRenderer = vtkSmartPointer<vtkRenderer>::New();
	m_UpRightRendWin = vtkSmartPointer<vtkRenderWindow>::New();
	m_UpRightInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_UpRightInterStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();

	m_UpRightRendWin->AddRenderer(m_UpRightRenderer);
	m_UpRightInteractor->SetRenderWindow(m_UpRightRendWin);
	m_UpRightInteractor->SetInteractorStyle(m_UpRightInterStyle);

	ui.UpRightQvtkWidget->SetRenderWindow(m_UpRightRendWin);
	m_UpRightInteractor->Initialize();

	this->SetWindowText(m_UpRightRenderer, "Axial View", AxialColor);
	//初始化右下角视图
	m_LowerRightRenderer = vtkSmartPointer<vtkRenderer>::New();
	m_LowerRightRendWin = vtkSmartPointer<vtkRenderWindow>::New();
	m_LowerRightInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_LowerRightInterStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();

	m_LowerRightRendWin->AddRenderer(m_LowerRightRenderer);
	m_LowerRightInteractor->SetRenderWindow(m_LowerRightRendWin);
	m_LowerRightInteractor->SetInteractorStyle(m_LowerRightInterStyle);

	ui.LowerRightQvtkWidget->SetRenderWindow(m_LowerRightRendWin);
	m_LowerRightInteractor->Initialize();
	this->SetWindowText(m_LowerRightRenderer, "Sagital View", SagitalColor);
	//初始化左下角视图
	m_LowerLeftRenderer = vtkSmartPointer<vtkRenderer>::New();
	m_LowerLeftRendWin = vtkSmartPointer<vtkRenderWindow>::New();
	m_LowerLeftInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	m_LowerLeftInterStyle = vtkSmartPointer<vtkInteractorStyleImage>::New();
	m_LowerLeftModelStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	m_LowerLeftRendWin->AddRenderer(m_LowerLeftRenderer);
	m_LowerLeftInteractor->SetRenderWindow(m_LowerLeftRendWin);
	m_LowerLeftInteractor->SetInteractorStyle(m_LowerLeftInterStyle);

	ui.LowerLeftQvtkWidget->SetRenderWindow(m_LowerLeftRendWin);
	m_LowerLeftInteractor->Initialize();
	this->SetWindowText(m_LowerLeftRenderer, "Coronal View", CoronalColor);
}
void DentalHelper::OnAddImplant()
{
	if (m_WorkingState!=WorkState::drawimplant)
	{		
		ui.AddImplantButton->setChecked(false);
		return;

	}
	if (ui.AddImplantButton->isChecked())
	{
		m_EventQtConnector->Connect(m_LowerRightInteractor, vtkCommand::RightButtonPressEvent, this, SLOT(PickImplantPoint()));
	}
	else
	{
		m_FirstPointWidget->Off();
		m_DrawImplantState = DrawImplantState::none;
		m_EventQtConnector->Disconnect(m_LowerRightInteractor, vtkCommand::RightButtonPressEvent, this, SLOT(PickImplantPoint()));
	}
	

}


void DentalHelper::OnChange2ArchCurve()
{
	if (!m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropForArchCurve))
	{
		return;
	}
	ui.ChangeUpRightViewButton->setText("Arch Curve");
	m_Change2ArchCurve->setChecked(true);
	m_Change2Axial->setChecked(false);
	m_Change2Position->setChecked(false);
	//设置窗口标识
	this->SetWindowText(m_UpRightRenderer, "Arch Curve", AxialColor);

	//隐藏其他actor
	this->SetActorsVisibilityInAxial(false);
	this->SetActorsVisibilityInPosition(false);
	//显示arch nurve
	this->SetActorsVisibilityInArchCurve(true);

}



void DentalHelper::OnChange2AxialView()
{

	ui.ChangeUpRightViewButton->setText("Axial");
	m_Change2Axial->setChecked(true);
	m_Change2ArchCurve->setChecked(false);
	m_Change2Position->setChecked(false);
	//设置窗口标识
	this->SetWindowText(m_UpRightRenderer, "Axial", AxialColor);

	//重设slider 的范围	
	isAxial = true;
	isArchCurve = false;
	isPosition = false;
	int axial_middle = (m_Extend[5] - m_Extend[4]) / 2;
	ui.UpRightSlider->setMaximum(m_Extend[5] - m_Extend[4]);
	ui.UpRightSpinBox->setMaximum(m_Extend[5] - m_Extend[4]);
	ui.UpRightSlider->setValue(axial_middle);
	//显示image 
	this->SetActorsVisibilityInAxial(true);
	//隐藏其他
	this->SetActorsVisibilityInArchCurve(false);
	this->SetActorsVisibilityInPosition(false);

	//切割Implant
	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInUpRightView(m_AxialPlane);
	}
	this->CutLeftNurveInAxialView();
	this->CutRightNurveInAxialView();
	this->CutUpProthesisInAxialView();
	double normalOfAxialCamera[3] = { 0,0,1 };
	this->UpDateCamera(m_UpRightRenderer, normalOfAxialCamera, 0);
}
void DentalHelper::OnChange2Coronal()
{
	ui.ChangeLowerLeftButton->setText("Coronal");
	m_Change2Coronal->setChecked(true);
	m_Change2Panaromic->setChecked(false);
	m_Change2Rotation->setChecked(false);
	//设置窗口显示
	this->SetWindowText(m_LowerLeftRenderer, "Coronal", CoronalColor);
	isCoronal = true;
	isPanaromic = false;
	isRotation = false;
	//重设slider范围
	int coronal_middle = (m_Extend[3] - m_Extend[2]) / 2;
	ui.LowerLeftSlider->setMaximum(m_Extend[3] - m_Extend[2]);
	ui.LowerLeftSpinBox->setMaximum(m_Extend[3] - m_Extend[2]);
	ui.LowerLeftSlider->setValue(coronal_middle);

	//隐藏其他actor
	this->SetActorsVisibilityInPanaromic(false);
	this->SetActorsVisibilityInRotation(false);

	//切割implant
	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInLowerLeftView(m_CoronalPlane);
	}
	//显示image
	this->SetActorsVisibilityInCoronal(true);

	this->CutDownProthesisInCoronalView();
	this->CutUpProthesisInCoronalView();
	this->CutLeftNurveInCoronalView();
	this->CutRightNurveInCoronalView();
	double normalOfcoronalCamera[3] = { 0,-1,0 };
	m_LowerLeftRenderer->ResetCamera();
	this->UpDateCamera(m_LowerLeftRenderer, normalOfcoronalCamera, 90);
}
void DentalHelper::OnChange2Cross()
{
	ui.ChangeLowerRightButton->setText("Cross");
	m_Change2Cross->setChecked(true);
	m_Change2Sagital->setChecked(false);

	//设置窗口显示
	this->SetWindowText(m_LowerRightRenderer, "Cross", SagitalColor);	
	isCross = true;
	isSagital = false;

	ui.LowerRightSlider->setMaximum(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 2);
	ui.LowerRightSpinBox->setMaximum(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 2);
	ui.LowerRightSlider->setValue(0);
	//隐藏其他actor
	this->SetActorsVisibilityInSagital(false);

	//切割implant
	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInLowerRightView(m_CrossPlane);
	}
	//显示cross的actor
	this->SetActorsVisibilityInCross(true);



}

void DentalHelper::OnChange2Panaromic()
{
	ui.ChangeLowerLeftButton->setText("Panaromic");
	m_Change2Coronal->setChecked(false);
	m_Change2Rotation->setChecked(false);
	m_Change2Panaromic->setChecked(true);
	//设置窗口显示
	this->SetWindowText(m_LowerLeftRenderer, "Panaromic", CoronalColor);
	isPanaromic = true;
	isCoronal = false;
	isRotation = false;
	//设置panaromic slider的值和范围
	ui.LowerLeftSlider->setMinimum(-100);
	ui.LowerLeftSpinBox->setMinimum(-100);
	ui.LowerLeftSlider->setMaximum(100);
	ui.LowerLeftSpinBox->setMaximum(100);
	ui.LowerLeftSlider->setValue(0);
	ui.LowerLeftSpinBox->setValue(0);

	//隐藏其他actor
	this->SetActorsVisibilityInCoronal(false);
	this->SetActorsVisibilityInRotation(false);
	//显示panaromic的东西
	this->SetActorsVisibilityInPanaromic(true);

}
void DentalHelper::OnChange2Position()
{
	if (m_ImplantList.isEmpty())
	{
		m_Change2Position->setChecked(false);
		return;
	}
	ui.ChangeUpRightViewButton->setText("Position");
	m_Change2Axial->setChecked(false);
	m_Change2ArchCurve->setChecked(false);
	//设置窗口显示
	this->SetWindowText(m_UpRightRenderer, "Position", AxialColor);
	isPosition = true;
	isArchCurve = false;
	isAxial = false;
	//设置position slider的值和范围
	double maxValue = m_ImplantList.at(m_CurrentImplant)->m_ImplantLength / 0.5;
	ui.UpRightSlider->setMaximum(maxValue);
	ui.UpRightSpinBox->setMaximum(maxValue);
	ui.UpRightSlider->setValue(0);
	ui.UpRightSpinBox->setValue(0);

	//隐藏其他actor
	this->SetActorsVisibilityInArchCurve(false);
	this->SetActorsVisibilityInAxial(false);

	//切割implant
	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInUpRightView(m_PositionPlane);
	}
	//显示position的东西
	this->SetActorsVisibilityInPosition(true);

	this->CUtLeftNurveInPositionView();
	this->CutRightNurveInPositionView();
	this->CutUpProthesisInPositionView();
	this->CutDownProthesisInPositionView();
}

void DentalHelper::OnChange2Rotation()
{
	if (m_ImplantList.isEmpty())
	{
		m_Change2Rotation->setChecked(false);
		return;
	}
	ui.ChangeLowerLeftButton->setText("Rotation");
	m_Change2Coronal->setChecked(false);
	m_Change2Panaromic->setChecked(false);
	m_Change2Rotation->setChecked(true);
	//设置窗口显示
	this->SetWindowText(m_LowerLeftRenderer, "Rotation", CoronalColor);
	isRotation = true;
	isPanaromic = false;
	isCoronal = false;
	//设置rotation slider的值和范围
	ui.LowerLeftSlider->setMinimum(0);
	ui.LowerLeftSpinBox->setMinimum(0);
	ui.LowerLeftSlider->setMaximum(360);
	ui.LowerLeftSpinBox->setMaximum(360);
	ui.LowerLeftSlider->setValue(0);
	ui.LowerLeftSpinBox->setValue(0);

	this->SetActorsVisibilityInPanaromic(false);
	this->SetActorsVisibilityInCoronal(false);


	//切割Implant
	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInLowerLeftView(m_RotationPlane);
	}
	this->SetActorsVisibilityInRotation(true);
	//切割神经和模型
	this->CutLeftNurveInRotationView();
	this->CutRightNurveInRotationView();
	this->CutUpProthesisInRotationView();
	this->CutDownProthesisInRotationView();

}

void DentalHelper::OnChange2Sagital()
{
	ui.ChangeLowerRightButton->setText("Sagital");
	m_Change2Sagital->setChecked(true);
	m_Change2Cross->setChecked(false);
	//设置窗口显示
	this->SetWindowText(m_LowerRightRenderer, "Sagital", SagitalColor);
	isSagital = true;
	isCross = false;
	int sagital_middle = (m_Extend[1] - m_Extend[0]) / 2;
	ui.LowerRightSlider->setMaximum(m_Extend[1] - m_Extend[0]);
	ui.LowerRightSpinBox->setMaximum(m_Extend[1] - m_Extend[0]);
	ui.LowerRightSlider->setValue(sagital_middle);
	//隐藏其他actor
	this->SetActorsVisibilityInCross(false);

	for each (ImplantItem* var in m_ImplantList)
	{
		var->CutInLowerRightView(m_SagitalPlane);
	}
	//显示sagital的actor
	this->SetActorsVisibilityInSagital(true);

	m_LowerRightRenderer->ResetCamera();
	double normalOfSagitalCamera[3] = { 1,0,0 };
	this->UpDateCamera(m_LowerRightRenderer, normalOfSagitalCamera, 90);

}



void DentalHelper::OnChangeDownProthesisOpacity(int value)
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		ui.DownProthesisOpacitySlider->setValue(100);
		return;
	}
	m_DownProthesisActor->GetProperty()->SetOpacity(value / 100.0);
	m_ModelRendWin->Render();
}


void DentalHelper::OnChangePanaromicInteractionStyle()
{
	if (ui.Panaromic2DRadioButton->isChecked())
	{
		m_LowerLeftInteractor->SetInteractorStyle(m_LowerLeftInterStyle);
		m_PanaromicActorIn2D->VisibilityOn();
		m_PanaromicActorForDrawingNurve->VisibilityOff();
		double normalOfCamera[3] = { 0,1,0 };
		m_LowerLeftRenderer->ResetCamera();
		this->UpDateCamera(m_LowerLeftRenderer, normalOfCamera,90);
	}
	else
	{
		m_LowerLeftInteractor->SetInteractorStyle(m_LowerLeftModelStyle);
		m_PanaromicActorIn2D->VisibilityOff();
		m_PanaromicActorForDrawingNurve->VisibilityOn();
		m_LowerLeftRenderer->ResetCamera();
	}
	m_LowerLeftRendWin->Render();
}
void DentalHelper::OnChangeUpProthesisOpacity(int value)
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		ui.UpProthesisOpacitySlider->setValue(100);
		return;
	}
	m_UpProthesisActor->GetProperty()->SetOpacity(value / 100.0);
	m_ModelRendWin->Render();
}


void DentalHelper::OnContourForLeftCurveInterAction()
{
	this->CutLeftNurveInCrossView();
	m_ModelRendWin->Render();
}

void DentalHelper::OnContourForRightCurveInterAction()
{
	this->CutRightNurveInCrossView();
	m_ModelRendWin->Render();
}



void DentalHelper::OnContourWidgetForArchCurveInteraction()
{
	auto contourData_ = vtkSmartPointer<vtkPolyData>::New();
	m_ContourRepForArchCurve->GetNodePolyData(contourData_);
	auto points_ = vtkSmartPointer<vtkPoints>::New();
	points_=contourData_->GetPoints();
	//将特定点存在注册表中
	int numOfPoints = points_->GetNumberOfPoints();
	QSettings widgetPoints;
	widgetPoints.setValue("numOfPoints", numOfPoints);
	for (int i = 0; i < numOfPoints; i++)
	{
		double temp[3];
		points_->GetPoint(i, temp);
		QString nameOfPoint = "Point";
		QString nameOfIndex;
		widgetPoints.setValue(nameOfPoint + nameOfIndex.setNum(i) + "x",temp[0]);
		widgetPoints.setValue(nameOfPoint + nameOfIndex.setNum(i) + "y", temp[1]);
		widgetPoints.setValue(nameOfPoint + nameOfIndex.setNum(i) + "z", temp[2]);
	}
	//更新曲线插值
	m_ContourSplineFilter->SetInputData(m_ContourRepForArchCurve->GetContourRepresentationAsPolyData());
	m_ContourSplineFilter->Update();
	//更新slider的最大值
	ui.LowerRightSlider->setMaximum(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 2);
	ui.LowerRightSpinBox->setMaximum(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 2);
	//更新cross slice
	this->GenerateCrossReslice();
	//更新offset spline
	this->GenerateOffSetSpline();
	this->GeneratePanaromicReslice();
	this->GeneratePanaromicReslice2D();
}

void DentalHelper::OnCrossViewInModelVisibility()
{
	//如果没有生成，则返回
	if (!m_ModelRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfCrossInModel))
	{
		return;
	}
	if (m_ReslicePropOfCrossInModel->GetVisibility())
	{
		m_ReslicePropOfCrossInModel->VisibilityOff();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.CrossViewVisibilityButton->setIcon(icon);
	}
	else
	{
		m_ReslicePropOfCrossInModel->VisibilityOn();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.CrossViewVisibilityButton->setIcon(icon);
	}
	m_ModelRendWin->Render();
}
void DentalHelper::OnDeleteLastNodeForLeftNurve()
{
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		return;
	}
	if (!ui.DrawLeftNurveButton->isChecked())
	{
		return;
	}
	m_ContourRepForLeftNurve->DeleteLastNode();
	m_ModelRendWin->Render();
	m_LowerLeftRendWin->Render();
}
void DentalHelper::OnDeleteLastNodeForRightNurve()
{
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		return;
	}
	if (!ui.DrawRightNurveButton->isChecked())
	{
		return;
	}
	m_ContourRepForRightNurve->DeleteLastNode();
	m_ModelRendWin->Render();
	m_LowerLeftRendWin->Render();
}

void DentalHelper::OnDeleteUpProthesis()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}
	if (QMessageBox::information(this, "Remove Up Prothesis", "Are you sure to remove the up prothesis?", QMessageBox::Ok | QMessageBox::Cancel)==QMessageBox::Cancel)
	{
		return;
	}
	m_ModelRenderer->RemoveActor(m_UpProthesisActor);
	m_ModelRendWin->Render();
	//还应该要删除在其他视图中的轮廓线
	//axial
	if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInAxial))
	{
		m_UpRightRenderer->RemoveActor(m_CutActorForUpProthesisInAxial);
		m_UpRightRendWin->Render();
	}
	//coronal
	if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCoronal))
	{
		m_LowerLeftRenderer->RemoveActor(m_CutActorForUpProthesisInCoronal);
		m_LowerLeftRendWin->Render();
	}
	//sagital
	if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInSagital))
	{
		m_LowerRightRenderer->RemoveActor(m_CutActorForUpProthesisInSagital);
		m_LowerRightRendWin->Render();
	}
	//cross
	if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCross))
	{
		m_LowerRightRenderer->RemoveActor(m_CutActorForUpProthesisInCross);
		m_LowerRightRendWin->Render();
	}
	//rotation
	if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInRotation))
	{
		m_LowerLeftRenderer->RemoveActor(m_CutActorForUpProthesisInRotation);
		m_LowerLeftRendWin->Render();
	}
	//position
	if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInPosition))
	{
		m_UpRightRenderer->RemoveActor(m_CutActorForUpProthesisInPosition);
		m_UpRightRendWin->Render();
	}
}

void DentalHelper::OnDownProthesisVisibility()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}
	if (m_DownProthesisActor->GetVisibility())
	{
		m_DownProthesisActor->VisibilityOff();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DownProthesisVisibilityButton->setIcon(icon);
	}
	else
	{
		m_DownProthesisActor->VisibilityOn();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DownProthesisVisibilityButton->setIcon(icon);
	}
	m_ModelRendWin->Render();
}



void DentalHelper::OnDrawLeftNurve()
{
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_PanaromicActorForDrawingNurve))
	{
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawLeftNurveButton->setIcon(icon);
		ui.DrawLeftNurveButton->setChecked(false);
		return;
	}
	if (!m_PanaromicActorForDrawingNurve->GetVisibility())
	{
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawLeftNurveButton->setIcon(icon);
		ui.DrawLeftNurveButton->setChecked(false);
		return;
	}
	if (!ui.DrawLeftNurveButton->isChecked())
	{
		ui.DrawRightNurveButton->setDisabled(false);
		ui.DeleteLastRightNurvePointButton->setDisabled(false);

		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawLeftNurveButton->setIcon(icon);
		ui.LowerLeftQvtkWidget->setCursor(Qt::ArrowCursor);
		m_ContourForLeftNurve->Off();
		m_LowerLeftRendWin->Render();
		return;
	}


	if (m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		QIcon msgicon;
		msgicon.addFile(QStringLiteral(":/DentalHelper/Resources/DentalHelper.ico"), QSize(), QIcon::Normal, QIcon::Off);
		QMessageBox choose;
		choose.setText("Do you want to change the left nurve?");
		choose.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		choose.setButtonText(2048, "Redraw");
		choose.setButtonText(8388608, "Modify");
		choose.setWindowIcon(msgicon);
		QString msgStyle = "font: 90 9pt \"Microsoft YaHei\";";

		QList<QAbstractButton*> msgButtons = choose.buttons();
		for each (QAbstractButton* var in msgButtons)
		{
			var->setStyleSheet("border:2px solid gray;border-radius:10px;padding:2px 4px;"+msgStyle);
		}
		int choice = choose.exec();

		if (choice ==QMessageBox::Discard)//如果是修改
		{

			m_ContourForLeftNurve->On();
			m_ContourForLeftNurve->SetWidgetState(vtkContourWidget::Define);

			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw_Checked.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.DrawLeftNurveButton->setIcon(icon);
			//画右边神经的按钮禁用
			ui.DrawRightNurveButton->setDisabled(true);
			ui.DeleteLastRightNurvePointButton->setDisabled(true);
			ui.LowerLeftQvtkWidget->setCursor(Qt::PointingHandCursor);
			return;
		}
		else if (choice==QMessageBox::Save)//如果是重画
		{
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw_Checked.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.DrawLeftNurveButton->setIcon(icon);

			m_ContourRepForLeftNurve->ClearAllNodes();
			m_ContourForLeftNurve->SetWidgetState(vtkContourWidget::Define);
			ui.LowerLeftQvtkWidget->setCursor(Qt::PointingHandCursor);
		}
		else
		{
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.DrawLeftNurveButton->setIcon(icon);
			ui.DrawLeftNurveButton->setChecked(false);
			return;
		}
		m_LowerLeftRendWin->Render();
	}
	QIcon icon;
	icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw_Checked.png"), QSize(), QIcon::Normal, QIcon::Off);
	ui.DrawLeftNurveButton->setIcon(icon);
	//画右边神经的按钮禁用
	ui.DrawRightNurveButton->setDisabled(true);
	ui.DeleteLastRightNurvePointButton->setDisabled(true);

	m_ContourForLeftNurve->SetInteractor(m_LowerLeftInteractor);
	m_ContourForLeftNurve->SetFollowCursor(0);

	m_ContourLineForLeftNurve->SetMaximumCurveLineSegments(200);
	m_ContourPointPlacerForLeftNurve->AddProp(m_PanaromicActorForDrawingNurve);

	m_ContourForLeftNurve->SetRepresentation(m_ContourRepForLeftNurve);
	m_ContourRepForLeftNurve->SetLineInterpolator(m_ContourLineForLeftNurve);
	m_ContourRepForLeftNurve->SetPointPlacer(m_ContourPointPlacerForLeftNurve);

	m_ContourRepForLeftNurve->GetLinesProperty()->SetLineWidth(3);
	m_ContourRepForLeftNurve->GetLinesProperty()->SetColor(0.7, 0.1, 0.3);
	

	m_ContourForLeftNurve->On();
	m_EventQtConnector->Connect(m_ContourForLeftNurve, vtkCommand::InteractionEvent, this, SLOT(OnContourForLeftCurveInterAction()));

	ui.LowerLeftQvtkWidget->setCursor(Qt::PointingHandCursor);

	m_TubeForLeftNurve->SetCapping(1);
	m_TubeForLeftNurve->SetRadius(2);
	m_TubeForLeftNurve->SetNumberOfSides(20);
	m_TubeForLeftNurve->SetInputData(m_ContourRepForLeftNurve->GetContourRepresentationAsPolyData());
	m_TubeForLeftNurve->Update();

	auto leftNurveMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	leftNurveMapper->SetInputConnection(m_TubeForLeftNurve->GetOutputPort());
	m_ActorForLeftNurve->SetMapper(leftNurveMapper);
	m_ActorForLeftNurve->GetProperty()->SetColor(1, 0, 0);
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve))
	{
		m_ModelRenderer->AddActor(m_ActorForLeftNurve);
	}
	m_ModelRendWin->Render();
}

void DentalHelper::OnDrawRightNurve()
{
	if (!m_LowerLeftRenderer->GetActors()->IsItemPresent(m_PanaromicActorForDrawingNurve))
	{
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawRightNurveButton->setIcon(icon);
		ui.DrawRightNurveButton->setChecked(false);
		return;
	}
	if (!m_PanaromicActorForDrawingNurve->GetVisibility())
	{
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawRightNurveButton->setIcon(icon);
		ui.DrawRightNurveButton->setChecked(false);
		return;
	}
	if (!ui.DrawRightNurveButton->isChecked())
	{
		ui.DrawLeftNurveButton->setDisabled(false);
		ui.DeleteLastLeftNurvePointButton->setDisabled(false);

		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawRightNurveButton->setIcon(icon);
		ui.LowerLeftQvtkWidget->setCursor(Qt::ArrowCursor);
		m_ContourForRightNurve->Off();
		m_LowerLeftRendWin->Render();
		return;
	}


	if (m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		QIcon msgicon;
		msgicon.addFile(QStringLiteral(":/DentalHelper/Resources/DentalHelper.ico"), QSize(), QIcon::Normal, QIcon::Off);
		QMessageBox choose;
		choose.setText("Do you want to change the left nurve?");
		choose.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		choose.setButtonText(2048, "Redraw");
		choose.setButtonText(8388608, "Modify");
		choose.setWindowIcon(msgicon);
		QString msgStyle = "font: 90 9pt \"Microsoft YaHei\";";

		QList<QAbstractButton*> msgButtons = choose.buttons();
		for each (QAbstractButton* var in msgButtons)
		{
			var->setStyleSheet("border:2px solid gray;border-radius:10px;padding:2px 4px;" + msgStyle);
		}
		int choice = choose.exec();

		if (choice == QMessageBox::Discard)//如果是修改
		{

			m_ContourForRightNurve->On();
			m_ContourForRightNurve->SetWidgetState(vtkContourWidget::Define);

			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw_Checked.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.DrawRightNurveButton->setIcon(icon);
			//画左边神经的按钮禁用
			ui.DrawLeftNurveButton->setDisabled(true);
			ui.DeleteLastLeftNurvePointButton->setDisabled(true);
			ui.LowerLeftQvtkWidget->setCursor(Qt::PointingHandCursor);
			return;
		}
		else if (choice == QMessageBox::Save)//如果是重画
		{
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw_Checked.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.DrawRightNurveButton->setIcon(icon);

			m_ContourRepForRightNurve->ClearAllNodes();
			m_ContourForRightNurve->SetWidgetState(vtkContourWidget::Define);
			ui.LowerLeftQvtkWidget->setCursor(Qt::PointingHandCursor);
		}
		else
		{
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.DrawRightNurveButton->setIcon(icon);
			ui.DrawRightNurveButton->setChecked(false);
			return;
		}
		m_LowerLeftRendWin->Render();
	}
	QIcon icon;
	icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw_Checked.png"), QSize(), QIcon::Normal, QIcon::Off);
	ui.DrawRightNurveButton->setIcon(icon);
	//画左边神经的按钮禁用
	ui.DrawLeftNurveButton->setDisabled(true);
	ui.DeleteLastLeftNurvePointButton->setDisabled(true);

	m_ContourForRightNurve->SetInteractor(m_LowerLeftInteractor);
	m_ContourForRightNurve->SetFollowCursor(0);

	m_ContourLineForRightNurve->SetMaximumCurveLineSegments(200);
	m_ContourPointPlacerForRightNurve->AddProp(m_PanaromicActorForDrawingNurve);

	m_ContourForRightNurve->SetRepresentation(m_ContourRepForRightNurve);
	m_ContourRepForRightNurve->SetLineInterpolator(m_ContourLineForRightNurve);
	m_ContourRepForRightNurve->SetPointPlacer(m_ContourPointPlacerForRightNurve);

	m_ContourRepForRightNurve->GetLinesProperty()->SetLineWidth(3);
	m_ContourRepForRightNurve->GetLinesProperty()->SetColor(0.7, 0.1, 0.3);


	m_ContourForRightNurve->On();
	m_EventQtConnector->Connect(m_ContourForRightNurve, vtkCommand::InteractionEvent, this, SLOT(OnContourForRightCurveInterAction()));

	ui.LowerLeftQvtkWidget->setCursor(Qt::PointingHandCursor);

	m_TubeForRightNurve->SetCapping(1);
	m_TubeForRightNurve->SetRadius(2);
	m_TubeForRightNurve->SetNumberOfSides(20);
	m_TubeForRightNurve->SetInputData(m_ContourRepForRightNurve->GetContourRepresentationAsPolyData());
	m_TubeForRightNurve->Update();

	auto rightNurveMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	rightNurveMapper->SetInputConnection(m_TubeForRightNurve->GetOutputPort());
	m_ActorForRightNurve->SetMapper(rightNurveMapper);
	m_ActorForRightNurve->GetProperty()->SetColor(1, 0, 0);
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve))
	{
		m_ModelRenderer->AddActor(m_ActorForRightNurve);
	}
	m_ModelRendWin->Render();
}
void DentalHelper::OnFirstPointWidgetInteraction()
{
	double centerOfFirstWiget_[3];
	m_FirstPointWidget->GetCenter(centerOfFirstWiget_);
	double projcetedPoint[3];
	m_CrossPlane->ProjectPoint(centerOfFirstWiget_, projcetedPoint);

	m_FirstPointWidget->SetCenter(projcetedPoint);
}



void DentalHelper::OnFirstRotateWidgetInteraction()
{
	double centerOfFirstWidget_[3];
	m_FirstRotateWidget->GetCenter(centerOfFirstWidget_);
	//投影到平面上
	double projectedPosition[3];
	m_RotationPlane->ProjectPoint(centerOfFirstWidget_, projectedPosition);
	m_FirstRotateWidget->SetCenter(projectedPosition);

	//计算新的位置
	double newNormal[3];
	for (int i=0;i<3;i++)
	{
		newNormal[i]= projectedPosition[i] - m_ImplantList.at(m_CurrentImplant)->m_ImplantSecondPoint[i];
	}
	vtkMath::Normalize(newNormal);
	for (int i=0;i<3;i++)
	{	
		m_ImplantList.at(m_CurrentImplant)->m_NormalOfTube[i] = newNormal[i];
		m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint[i] = m_ImplantList.at(m_CurrentImplant)->m_ImplantSecondPoint[i] + m_ImplantList.at(m_CurrentImplant)->m_ImplantLength*newNormal[i];
	}
	
	m_ImplantList.at(m_CurrentImplant)->UpDateImplant();
	//计算平行度
	this->CalculateParallelAngle();
	//this->GeneratePositionReslice();
}



void DentalHelper::OnImageTableCellClicked(int row, int columm)
{
	if (columm==0)
	{
		//重置当前选中的item
		m_CurrentIndex = row;
	}
	if (columm==5)
	{
		//显示item的详细信息
	}
}
void DentalHelper::OnImplantChanged(ImplantItem* item)
{
		if (isAxial)
	{	
		item->CutInUpRightView(m_AxialPlane);
	}
	if (isCoronal)
	{	
		item->CutInLowerLeftView(m_CoronalPlane);
	}
	if (isSagital)
	{
		item->CutInLowerRightView(m_SagitalPlane);
	}
	if (isRotation)
	{
		item->CutInLowerLeftView(m_RotationPlane);
	}
	if (isPosition)
	{
		item->CutInUpRightView(m_PositionPlane);
	}
	if (isCross)
	{
		item->CutInLowerRightView(m_CrossPlane);
	}

	if (m_CurrentImplant==m_ImplantList.indexOf(item))
	{	
		this->SetImplantInteractionWidget(item->m_ImplantFirstPoint, item->m_NormalOfTube, item->m_ImplantLength);

		for (int i = 0; i < 3; i++)
		{
			centerOfPreviousMoveWiget[i] = (item->m_ImplantFirstPoint[i] + item->m_ImplantSecondPoint[i]) / 2;
		}
		//设置position slider的值和范围
		double maxValue = m_ImplantList.at(m_CurrentImplant)->m_ImplantLength / 0.5;
		ui.UpRightSlider->setMaximum(maxValue);
		ui.UpRightSpinBox->setMaximum(maxValue);
	}


	m_LowerLeftRendWin->Render();
	m_LowerRightRendWin->Render();
	m_UpRightRendWin->Render();
}


void DentalHelper::OnLoadDownProthesis()
{
	QString path = QFileDialog::getOpenFileName(this, "Choose the Lower Prothesis model", nullptr, "(*.stl)");
	if (path.isEmpty())
	{
		return;
	}
	auto stlReader = vtkSmartPointer<vtkSTLReader>::New();
	stlReader->SetFileName(qPrintable(path));
	stlReader->Update();

	m_DownProthesisData = stlReader->GetOutput();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_DownProthesisData);
	m_DownProthesisActor->SetMapper(mapper);
	m_DownProthesisActor->GetProperty()->SetColor(0, 1, 0);
	m_DownProthesisActor->GetProperty()->SetAmbient(0);
	m_DownProthesisActor->GetProperty()->SetDiffuse(1);
	m_DownProthesisActor->GetProperty()->SetSpecular(0);
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		m_ModelRenderer->AddActor(m_DownProthesisActor);
	}
	m_ModelRendWin->Render();
	if (isAxial)
	{	
		this->CutDownProthesisInAxialView();
	}
	if (isCoronal)
	{	
		this->CutDownProthesisInCoronalView();
	}
	if (isSagital)
	{
		this->CutDownProthesisInSagitalView();
	}
	if (isRotation)
	{
		this->CutDownProthesisInRotationView();
	}
	if (isPosition)
	{
		this->CutDownProthesisInPositionView();
	}
	if (isCross)
	{
		this->CutDownProthesisInCrossView();
	}
}


void DentalHelper::OnLoadUpProthesis()
{
	QString path = QFileDialog::getOpenFileName(this, "Choose the Up Prothesis model", nullptr, "(*.stl)");
	if (path.isEmpty())
	{
		return;
	}
	auto stlReader = vtkSmartPointer<vtkSTLReader>::New();
	stlReader->SetFileName(qPrintable(path));
	stlReader->Update();

	m_UpProthesisData = stlReader->GetOutput();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_UpProthesisData);
	m_UpProthesisActor->SetMapper(mapper);
	m_UpProthesisActor->GetProperty()->SetColor(0, 1, 0);
	m_UpProthesisActor->GetProperty()->SetAmbient(0);
	m_UpProthesisActor->GetProperty()->SetDiffuse(1);
	m_UpProthesisActor->GetProperty()->SetSpecular(0);
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		m_ModelRenderer->AddActor(m_UpProthesisActor);
	}
	m_ModelRendWin->Render();
	if (isAxial)
	{
		this->CutUpProthesisInAxialView();
	}
	if (isCoronal)
	{	
		this->CutUpProthesisInCoronalView();
	}
	if (isSagital)
	{	
		this->CutUpProthesisInSagitalView();
	}
	if (isPosition)
	{
		this->CutUpProthesisInPositionView();
	}
	if (isRotation)
	{
		this->CutUpProthesisInRotationView();
	}
	if (isCross)
	{
		this->CutUpProthesisInCrossView();
	}
}


void DentalHelper::OnLowerLeftViewButton()
{
	if (isShowLowerLeftView)
	{
		isShowLowerLeftView = false;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/LowerLeftOff.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.LowerLeftViewButton->setIcon(icon);
		ui.LowerLeftLayout->hide();
	}
	else
	{
		isShowLowerLeftView = true;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/LowerLeftOn.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.LowerLeftViewButton->setIcon(icon);
		ui.LowerLeftLayout->show();
	}
	this->SetSuitableLayout();
}
void DentalHelper::OnLowerRightViewButton()
{
	if (isShowLowerRightView)
	{
		isShowLowerRightView = false;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/LowerRightOff.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.LowerRightViewButton->setIcon(icon);
		ui.LowerRightLayout->hide();
	}
	else
	{
		isShowLowerRightView = true;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/LowerRightOn.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.LowerRightViewButton->setIcon(icon);
		ui.LowerRightLayout->show();
	}
	this->SetSuitableLayout();
}
void DentalHelper::OnMoveWidgetInteraction()
{
	double centerOfMoveWidget_[3];
	m_MoveWidget->GetCenter(centerOfMoveWidget_);
	double projectPosition[3];
	m_RotationPlane->ProjectPoint(centerOfMoveWidget_, projectPosition);
	double moveDistance[3];
	vtkMath::Subtract(projectPosition, centerOfPreviousMoveWiget,moveDistance);

	for (int i=0;i<3;i++)
	{
		m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint[i] += moveDistance[i];
		centerOfPreviousMoveWiget[i] = projectPosition[i];
	}
	m_ImplantList.at(m_CurrentImplant)->UpDateImplant();

	//计算平行度
	this->CalculateParallelAngle();
	//this->GeneratePositionReslice();
}
void DentalHelper::OnOpenProject()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open Project File"), "",
		tr("*.lx"));

	if (fileName.isEmpty())
		return;
	m_SaveProjectPath = fileName.left(fileName.length() - 3);
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_5_7);
	in >> m_WorkingState;
	if (m_WorkingState == WorkState::ImageLoading)
	{
		this->OpenImages(m_SaveProjectPath);
	}
	if (m_WorkingState==WorkState::preplanning)
	{
		this->OpenImages(m_SaveProjectPath);
		this->OnPreviewData();
		this->OnPreplanSetting();
		this->OpenSetting(m_SaveProjectPath);
	}
}



void DentalHelper::OnPanaromicInModelVisibility()
{
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_PanaromicActorInModel))
	{
		return;//如果没有全景片，返回
	}
	if (m_PanaromicActorInModel->GetVisibility())//如果可见，则设置为不可见
	{
		m_PanaromicActorInModel->VisibilityOff();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.PanaromicInModelVisivilityButton->setIcon(icon);
	}
	else
	{
		m_PanaromicActorInModel->VisibilityOn();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.PanaromicInModelVisivilityButton->setIcon(icon);
	}
	m_ModelRendWin->Render();
}

void DentalHelper::OnParallelRadioButton(ImplantItem* item)
{
	int indexOfItem = m_ImplantList.indexOf(item);
	m_CurrentParallel = indexOfItem;
	this->CalculateParallelAngle();
}


void DentalHelper::OnPlaneWidgetForArchCurveInteraction()
{
	auto planeForArchCurve = vtkSmartPointer<vtkPlane>::New();
	m_PlaneWidgetForArchCurve->GetPlane(planeForArchCurve);

	m_ResliceMapperForArchCurve->SetSlicePlane(planeForArchCurve);

	this->UpDateCamera(m_UpRightRenderer, planeForArchCurve->GetNormal(), 90);
	m_UpRightRendWin->Render();

	this->GenerateCrossReslice();
	this->GenerateOffSetSpline();
	this->GeneratePanaromicReslice();
	this->GeneratePanaromicReslice2D();
}
void DentalHelper::OnPlaneWidgetForArchCurveVisibility()
{
	if (!m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropForArchCurve))
	{
		return;
	}
	if (m_PlaneWidgetForArchCurve->GetEnabled())//如果可见，设置为不可见
	{
		m_PlaneWidgetForArchCurve->Off();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.PlaneWidgetVisibilityButton->setIcon(icon);
	}
	else
	{
		m_PlaneWidgetForArchCurve->On();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.PlaneWidgetVisibilityButton->setIcon(icon);
	}
	m_ModelRendWin->Render();
}

void DentalHelper::OnPositionVisibilityIn3DView()
{
	//如果没有种植体
	if (m_ImplantList.isEmpty())
	{
		return;
	}
	if (m_ModelRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfPositionIn3D))
	{
		if (m_ReslicePropOfPositionIn3D->GetVisibility())
		{
			m_ReslicePropOfPositionIn3D->VisibilityOff();
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.PositionIn3DVisibilityButton->setIcon(icon);
		}
		else
		{
			m_ReslicePropOfPositionIn3D->VisibilityOn();
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.PositionIn3DVisibilityButton->setIcon(icon);
		}
		m_ModelRendWin->Render();
	}
}



void DentalHelper::OnPreplanSetting()
{ 
	//如果没有导入imagedata return
	if (!m_ModelRenderer->GetVolumes()->IsItemPresent(m_VolumeActor))
	{
		QMessageBox::information(this, "Warning", "Please load image data first!");
		return;
	}
	ui.tabWidget->setCurrentIndex(1);//显示规划菜单栏
	this->SetViewModel2Preplan();
	ui.ImageNextButton->setDisabled(true);
}


void DentalHelper::OnPreviewData()
{
	if (m_DicomList.isEmpty())
	{
		return;
	}
	if (m_CurrentIndex==-1)
	{
		m_ImageData = m_DicomList.last()->GetImageData();
	}
	else
	{
		m_ImageData = m_DicomList.at(m_CurrentIndex)->GetImageData();
	}
	m_ImageData->GetExtent(m_Extend);
	m_ImageData->GetSpacing(m_Spacing);
	m_ImageData->GetOrigin(m_Origin);

	this->SetViewModel2Conventional();
	this->SetVolumeRendering();
}
void DentalHelper::OnReadDICOM()
{
	DlgSelectDirectory *dialog = new DlgSelectDirectory;//选中的文件夹
	dialog->setParent(this, Qt::Dialog);
	if (dialog->exec() == QDialog::Accepted)
	{	

		this->ReadImageFile(dialog->m_sDirectory);
	}
	delete dialog;
}

void DentalHelper::OnRemoveImage()
{
	//如果image list为空 return
	if (m_DicomList.isEmpty())
	{
		return;
	}
	//如果当前没有选中任何image 设定为最后一个
	if (m_CurrentIndex==-1)
	{
		m_CurrentIndex = m_DicomList.size() - 1;
	}
	QString warningText = "Serial Num: "+m_DicomList.at(m_CurrentIndex)->GetMainList().at(0) + "  Name:  " + m_DicomList.at(m_CurrentIndex)->GetMainList().at(1);
	int choose = QMessageBox::information(this, "Remove image", "Are you sure to remove  \n" + warningText, QMessageBox::Cancel | QMessageBox::Ok);
	if (choose==QMessageBox::Cancel)
	{
		return;
	}
	m_DicomList.removeAt(m_CurrentIndex);
	ui.ImageTabelWidget->removeRow(m_CurrentIndex);
}

void DentalHelper::OnDeleteDownProthesis()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
	{
		return;
	}
	if (QMessageBox::information(this, "Remove Up Prothesis", "Are you sure to remove the lower prothesis?", QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
	{
		return;
	}
	m_ModelRenderer->RemoveActor(m_DownProthesisActor);
	m_ModelRendWin->Render();
	//还应该要删除在其他视图中的轮廓线

	//axial
	if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInAxial))
	{
		m_UpRightRenderer->RemoveActor(m_CutActorForDownProthesisInAxial);
		m_UpRightRendWin->Render();
	}
	//coronal
	if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCoronal))
	{
		m_LowerLeftRenderer->RemoveActor(m_CutActorForDownProthesisInCoronal);
		m_LowerLeftRendWin->Render();
	}
	//sagital
	if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInSagital))
	{
		m_LowerRightRenderer->RemoveActor(m_CutActorForDownProthesisInSagital);
		m_LowerRightRendWin->Render();
	}
	//cross
	if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCross))
	{
		m_LowerRightRenderer->RemoveActor(m_CutActorForDownProthesisInCross);
		m_LowerRightRendWin->Render();
	}
	//rotation
	if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInRotation))
	{
		m_LowerLeftRenderer->RemoveActor(m_CutActorForDownProthesisInRotation);
		m_LowerLeftRendWin->Render();
	}
	//position
	if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInPosition))
	{
		m_UpRightRenderer->RemoveActor(m_CutActorForDownProthesisInPosition);
		m_UpRightRendWin->Render();
	}
}


void DentalHelper::OnDeleteImplant(ImplantItem* item)
{


	int removedIndex = m_ImplantList.indexOf(item);
	m_ImplantList.removeAt(removedIndex);
	ui.ImplantTableWidget->removeColumn(removedIndex);
	ui.BaseTableWidget->removeColumn(removedIndex);
	ui.ParallelTableWidget->removeColumn(removedIndex);
	for each (ImplantItem* var in m_ImplantList)
	{
		int newIndex = m_ImplantList.indexOf(var);
		var->SetIndex(newIndex+1);
	}
	m_CurrentParallel = m_ImplantList.size() - 1;
	if (m_CurrentParallel>-1)
	{
		m_ImplantList.at(m_CurrentParallel)->SetParallelRadioButton(true);
		this->CalculateParallelAngle();
	}

	//判断是否是当前的种植体
	if (m_CurrentImplant==removedIndex)
	{
		//如果删除后还有种植体
		if (!m_ImplantList.isEmpty())
		{
			this->OnUpdateCurrentImplant(m_ImplantList.size());
		}
		else//如果没有种植体了
		{
			m_CurrentImplant = -1;
			this->OnChange2Panaromic();
			this->OnChange2ArchCurve();
			//删除两个在三维的actor
			m_ModelRenderer->RemoveViewProp(m_ReslicePropOfPositionIn3D);
			m_ModelRenderer->RemoveViewProp(m_ReslicePropOfRotationIn3D);
			m_ModelRendWin->Render();
			//调整种植体的widget关闭
			m_FirstRotateWidget->Off();
			m_SecondRotateWidget->Off();
			m_MoveWidget->Off();
			m_LowerLeftRendWin->Render();
		}
	}
	else if (m_CurrentImplant>removedIndex)
	{
		this->OnUpdateCurrentImplant(m_CurrentImplant);
		this->OnUpDateLowerLeftView(ui.LowerLeftSlider->value());
		this->OnUpDateLowerRightView(ui.LowerRightSlider->value());
		this->OnUpDateUpRightView(ui.UpRightSlider->value());
	}

}


void DentalHelper::OnDeleteLastContourNode()
{
	if (m_ContourRepForArchCurve->GetNumberOfNodes()<3)
	{
		return;
	}
	m_ContourRepForArchCurve->DeleteLastNode();
	m_ContourRepForArchCurve->BuildRepresentation();
	this->OnContourWidgetForArchCurveInteraction();
	m_UpRightRendWin->Render();

}

void DentalHelper::OnRotationVisibilityIn3DView()
{
	//如果没有种植体
	if (m_ImplantList.isEmpty())
	{
		return;
	}
	if (m_ModelRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfRotationIn3D))
	{
		if (m_ReslicePropOfRotationIn3D->GetVisibility())
		{
			m_ReslicePropOfRotationIn3D->VisibilityOff();
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.RotationIn3DVisibilityButton->setIcon(icon);
		}
		else
		{
			m_ReslicePropOfRotationIn3D->VisibilityOn();
			QIcon icon;
			icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
			ui.RotationIn3DVisibilityButton->setIcon(icon);
		}
		m_ModelRendWin->Render();
	}
}
void DentalHelper::OnSaveProject()
{
	if (m_SaveProjectPath.isEmpty())
	{
		m_SaveProjectPath = QFileDialog::getSaveFileName(this, "Save Project", 0, "*.lx");
	}
	if (m_SaveProjectPath.isEmpty())
	{
		return;
	}
	m_SaveProjectPath = m_SaveProjectPath.left(m_SaveProjectPath.length() - 3);
	QString fileName = m_SaveProjectPath + ".lx";
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_7);

	//输出当前状态
	out << m_WorkingState;
	//如果还没有开始规划
	if (m_WorkingState==WorkState::ImageLoading)
	{
		this->SaveImages(m_SaveProjectPath);
	}
	//如果开始规划了
	if (m_WorkingState==WorkState::preplanning)
	{
		this->SaveImages(m_SaveProjectPath);
		this->SavePrePlanning(m_SaveProjectPath);
	}
	if (m_WorkingState==WorkState::drawimplant)
	{
		this->SaveImages(m_SaveProjectPath);
		this->SavePrePlanning(m_SaveProjectPath);
		this->SaveImplants(m_SaveProjectPath);
	}
	QMessageBox::information(this, "Save Project", "Done!");
}


void DentalHelper::OnSecondRotateWidgetInteraction()
{
	double centerOfSecondWidget_[3];
	m_SecondRotateWidget->GetCenter(centerOfSecondWidget_);
	//投影到平面上
	double projectedPosition[3];
	m_RotationPlane->ProjectPoint(centerOfSecondWidget_, projectedPosition);
	m_SecondRotateWidget->SetCenter(projectedPosition);

	//计算新的位置
	double newNormal[3];
	for (int i = 0; i < 3; i++)
	{
		newNormal[i] = m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint[i]-projectedPosition[i] ;
	}
	vtkMath::Normalize(newNormal);
	for (int i = 0; i < 3; i++)
	{
		m_ImplantList.at(m_CurrentImplant)->m_NormalOfTube[i] = newNormal[i];
		m_ImplantList.at(m_CurrentImplant)->m_ImplantSecondPoint[i] = m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint[i]- m_ImplantList.at(m_CurrentImplant)->m_ImplantLength*newNormal[i];
	}

	m_ImplantList.at(m_CurrentImplant)->UpDateImplant();

	//计算平行度
	this->CalculateParallelAngle();
	//this->GeneratePositionReslice();
}


void DentalHelper::OnSetBackGroundColor1()
{
	//初始化颜色
	QColor initialColor;
	initialColor.setRed(m_ModelRenderer->GetBackground()[0] * 255);
	initialColor.setGreen(m_ModelRenderer->GetBackground()[1] * 255);
	initialColor.setBlue(m_ModelRenderer->GetBackground()[2] * 255);
	QColor resultColor;
	resultColor=QColorDialog::getColor(initialColor, this, "Choose a color for background 1", QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
	if (!resultColor.isValid())
	{
		return;
	}
	double changedColor[3];
	changedColor[0] = resultColor.red() / 255.0;
	changedColor[1] = resultColor.green() / 255.0;
	changedColor[2] = resultColor.blue() / 255.0;

	this->SetButtonColor(ui.BackGround1Button, changedColor);
	m_ModelRenderer->SetBackground(changedColor); 
	m_ModelRendWin->Render();
}

void DentalHelper::OnSetBackGroundColor2()
{
	//初始化颜色
	QColor initialColor;
	initialColor.setRed(m_ModelRenderer->GetBackground2()[0] * 255);
	initialColor.setGreen(m_ModelRenderer->GetBackground2()[1] * 255);
	initialColor.setBlue(m_ModelRenderer->GetBackground2()[2] * 255);
	QColor resultColor;
	resultColor = QColorDialog::getColor(initialColor, this, "Choose a color for background 2", QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);
	if (!resultColor.isValid())
	{
		return;
	}
	double changedColor[3];
	changedColor[0] = resultColor.red() / 255.0;
	changedColor[1] = resultColor.green() / 255.0;
	changedColor[2] = resultColor.blue() / 255.0;

	this->SetButtonColor(ui.BackGround2Button, changedColor);
	m_ModelRenderer->SetBackground2(changedColor);
	m_ModelRendWin->Render();
}

void DentalHelper::OnSetProgressBar(int p)
{
	m_ProgressDialog->show();
	m_ProgressDialog->SetValue(p);
	QCoreApplication::processEvents();
	if (p == 100)
	{
		m_ProgressDialog->hide();
	}
}

void DentalHelper::OnStartDrawImplant()
{

	//如果没有进行pre plan,就返回
	if (!m_ModelRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfCrossInModel))
	{
		return;
	}
	ui.tabWidget->setCurrentIndex(2);//显示放种植体菜单栏

	m_WorkingState = WorkState::drawimplant;
	ui.StartDrawImplantButton->setDisabled(true);
}
void DentalHelper::OnUpdateCurrentImplant(int index)
{

	//将其他的radiobutton设置为unchecked
	for each (ImplantItem* var in m_ImplantList)
	{
		var->SetRadioButtonUnchecked(index);
	}	

	m_CurrentImplant = index - 1;
	//下面处理关于rotation和position视图的事
	if (isRotation)
	{

		GenerateRotationReslice();
		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInLowerLeftView(m_RotationPlane);
		}
		m_LowerLeftRendWin->Render();
	}
	if (isPosition)
	{		
		GeneratePositionReslice();
		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInUpRightView(m_PositionPlane);
		}
		m_UpRightRendWin->Render();
	}
	this->SetImplantInteractionWidget(m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint, m_ImplantList.at(m_CurrentImplant)->m_NormalOfTube, m_ImplantList.at(m_CurrentImplant)->m_ImplantLength);
	for (int i = 0; i < 3; i++)
	{
		centerOfPreviousMoveWiget[i] = (m_ImplantList.at(m_CurrentImplant)->m_ImplantFirstPoint[i] + m_ImplantList.at(m_CurrentImplant)->m_ImplantSecondPoint[i]) / 2;
	}
}



void DentalHelper::OnUpDateLowerLeftView(int index)
{
	//如果是传统视图
	if (isCoronal)
	{
		//判断视图中是否存在图像
		if (m_LowerLeftRenderer->GetViewProps()->IsItemPresent(m_CoronalViewer->GetImageActor()))
		{
			m_CoronalViewer->SetSlice(index);
			m_CoronalViewer->Render();
			this->UpDateCoronalLine();
			m_LowerLeftRendWin->Render();

			double normalOfCut[3] = { 0,1,0 };
			double centerOfCut[3];
			centerOfCut[0] = m_Origin[0] + m_SagitalViewer->GetSlice()*m_Spacing[0];
			centerOfCut[1] = m_Origin[1] + m_CoronalViewer->GetSlice()*m_Spacing[1];
			centerOfCut[2] = m_Origin[2] + m_AxialViewer->GetSlice()*m_Spacing[2];

			m_CoronalPlane->SetOrigin(centerOfCut);
			m_CoronalPlane->SetNormal(normalOfCut);

			//切割种植体
			for each (ImplantItem* var in m_ImplantList)
			{
				var->CutInLowerLeftView(m_CoronalPlane);
			}
			m_LowerLeftRendWin->Render();

			this->CutLeftNurveInCoronalView();
			this->CutRightNurveInCoronalView();

			this->CutUpProthesisInCoronalView();
			this->CutDownProthesisInCoronalView();
		}
	}
	else if(isPanaromic)
		{
		this->GenerateOffSetSpline();
		this->GeneratePanaromicReslice();
		this->GeneratePanaromicReslice2D();
		}
	if (isRotation)
	{
		this->GenerateRotationReslice();
		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInLowerLeftView(m_RotationPlane);
		}
		this->CutLeftNurveInRotationView();
		this->CutRightNurveInRotationView();

		this->CutUpProthesisInRotationView();
		this->CutDownProthesisInRotationView();
		m_LowerLeftRendWin->Render();
	}
}


void DentalHelper::OnUpDateLowerRightView(int index)
{
	//如果是传统视图
	if (isSagital)
	{
		//判断视图中是否存在图像
		if (m_LowerRightRenderer->GetViewProps()->IsItemPresent(m_SagitalViewer->GetImageActor()))
		{
			m_SagitalViewer->SetSlice(index);
			m_SagitalViewer->Render();
			this->UpDateSagitalLine();
			m_LowerRightRendWin->Render();

			double normalOfCut[3] = { 1,0,0 };
			double centerOfCut[3];
			centerOfCut[0] = m_Origin[0] + m_SagitalViewer->GetSlice()*m_Spacing[0];
			centerOfCut[1] = m_Origin[1] + m_CoronalViewer->GetSlice()*m_Spacing[1];
			centerOfCut[2] = m_Origin[2] + m_AxialViewer->GetSlice()*m_Spacing[2];

			m_SagitalPlane->SetOrigin(centerOfCut);
			m_SagitalPlane->SetNormal(normalOfCut);

			for each (ImplantItem* var in m_ImplantList)
			{
				var->CutInLowerRightView(m_SagitalPlane);
				m_LowerRightRendWin->Render();
			}
			this->CutLeftNurveInSagitalView();
			this->CutRightNurveInSagitalView();

			this->CutUpProthesisInSagitalView();
			this->CutDownProthesisInSagitalView();
		}
	}
	else
	{
		this->GenerateCrossReslice();		
		

		//判断是否显示第一个种植点widget
		if (m_DrawImplantState==first)
		{
			double centerOfFirstWiget_[3];
			m_FirstPointWidget->GetCenter(centerOfFirstWiget_);
			double projcetedPoint[3];
			m_CrossPlane->ProjectPoint(centerOfFirstWiget_, projcetedPoint);
			double distance_ = sqrt(vtkMath::Distance2BetweenPoints(projcetedPoint, centerOfFirstWiget_));
			if (distance_<0.01)
			{
				m_FirstPointWidget->On();
			}
			else
			{
				m_FirstPointWidget->Off();
			}
			m_LowerRightRendWin->Render();
		}
		

	}
}



void DentalHelper::OnUpDateUpRightView(int index)
{

	//如果是传统视图
	if (isAxial)
	{
		//判断视图中是否存在图像
		if (m_UpRightRenderer->GetViewProps()->IsItemPresent(m_AxialViewer->GetImageActor()))
		{
			m_AxialViewer->SetSlice(index);

			double normalOfCut[3] = { 0,0,1 };
			double centerOfCut[3];
			centerOfCut[0] = m_Origin[0] + m_SagitalViewer->GetSlice()*m_Spacing[0];
			centerOfCut[1] = m_Origin[1] + m_CoronalViewer->GetSlice()*m_Spacing[1];
			centerOfCut[2] = m_Origin[2] + m_AxialViewer->GetSlice()*m_Spacing[2];

			m_AxialPlane->SetOrigin(centerOfCut);
			m_AxialPlane->SetNormal(normalOfCut);

			this->UpDateAxialLine();
			this->CutLeftNurveInAxialView();
			this->CutRightNurveInAxialView();
			this->CutUpProthesisInAxialView();
			this->CutDownProthesisInAxialView();
			//切割Implant
			for each (ImplantItem* var in m_ImplantList)
			{
				var->CutInUpRightView(m_AxialPlane);
			}
			m_UpRightRendWin->Render();
		}
	}
	if (isPosition)
	{

		this->GeneratePositionReslice();		
		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInUpRightView(m_PositionPlane);
		}
		this->CUtLeftNurveInPositionView();
		this->CutRightNurveInPositionView();
		this->CutUpProthesisInPositionView();
		this->CutDownProthesisInPositionView();
		m_UpRightRendWin->Render();
	}

}
void DentalHelper::OnUpDateVolumeRendering()
{
	if (!m_ModelRenderer->GetVolumes()->IsItemPresent(m_VolumeActor))
	{
		ui.ModelOpacitySlider->setValue(100);
		return;
	}
	double opacity = ui.ModelOpacitySlider->value() / 100.0;
	double min_ = ui.MinThresholdSlider->value();
	double max_ = ui.MaxThresholdSlider->value();
	double middle_ = min_ + (max_ - min_) / 2;
	m_VolumeFunction->RemoveAllPoints();
	m_VolumeFunction->AddPoint(-1024, 0);
	m_VolumeFunction->AddPoint(min_ - 1, 0);
	m_VolumeFunction->AddPoint(min_, 0.5*opacity);
	m_VolumeFunction->AddPoint(middle_, 0.7*opacity);
	m_VolumeFunction->AddPoint(max_, 0.8*opacity);
	m_VolumeFunction->AddPoint(max_ + 1, 0);
	m_VolumeProperty->SetScalarOpacity(m_VolumeFunction);

	m_VolumeColor->RemoveAllPoints();
	m_VolumeColor->AddRGBPoint(-1024, 0.00, 0.00, 0.00);
	m_VolumeColor->AddRGBPoint(min_ - 1, 0.00, 0.00, 0.00);
	m_VolumeColor->AddRGBPoint(min_, 230 / 255.0, 232 / 255.0, 82 / 255.0);
	m_VolumeColor->AddRGBPoint(middle_, 150 / 255.0, 80 / 255.0, 180 / 255.0);
	m_VolumeColor->AddRGBPoint(max_, 114 / 255.0, 115 / 255.0, 40 / 255.0);
	m_VolumeColor->AddRGBPoint(max_ + 1, 0, 0, 0);	
	m_VolumeProperty->SetColor(m_VolumeColor);
	m_ModelRendWin->Render();
}





void DentalHelper::OnUpLeftViewButton()
{
	if (isShowUpLeftView)
	{
		isShowUpLeftView = false;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/UpLeftOff.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.UpLeftViewButton->setIcon(icon);
		ui.UpLeftLayout->hide();
	}
	else
	{
		isShowUpLeftView = true;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/UpLeftOn.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.UpLeftViewButton->setIcon(icon);
		ui.UpLeftLayout->show();
	}
	this->SetSuitableLayout();
}

void DentalHelper::OnUpProthesisVisibility()
{
	//如果没有模型，返回
	if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
	{
		return;
	}
	if (m_UpProthesisActor->GetVisibility())
	{
		m_UpProthesisActor->VisibilityOff();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_off_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.UpProthesisVisibilityButton->setIcon(icon);
	}
	else
	{
		m_UpProthesisActor->VisibilityOn();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/visibility_on_64.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.UpProthesisVisibilityButton->setIcon(icon);
	}
	m_ModelRendWin->Render();
}


void DentalHelper::OnUpRightViewButton()
{
	if (isShowUpRightView)
	{
		isShowUpRightView = false;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/UpRightOff.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.UpRightViewButton->setIcon(icon);
		ui.UpRightLayout->hide();
	}
	else
	{
		isShowUpRightView = true;
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/UpRightOn.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.UpRightViewButton->setIcon(icon);
		ui.UpRightLayout->show();
	}
	this->SetSuitableLayout();
}
void DentalHelper::OpenDicomInformation(QDataStream& d, XDicomItem& data)
{
	d >> data.patientName >> data.patientGender >> data.patientAge >> data.SerialNum >> data.patientID >> data.date >> data.spacingX >> data.spacingY >> data.spacingZ >> data.manufacturer >> data.hospital;

}


void DentalHelper::OpenImages(QString filepath)
{
	QString fileName = filepath + ".manager";
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_5_7);
	int numOfitem;
	in >> numOfitem;
	in >> m_CurrentIndex;
	for (int i = 0; i < numOfitem; i++)
	{
		XDicomItem *item = new XDicomItem(0);
		this->OpenDicomInformation(in, *item);
		//read image
		QString temp;
		QString vtiName = filepath + "_" + temp.setNum(i);
		auto imageReader = vtkSmartPointer<vtkXMLImageDataReader>::New();
		imageReader->SetFileName(qPrintable(vtiName.append(".vti")));
		imageReader->Update();
		item->SetImageData(imageReader->GetOutput());
		item->BuildTagList();
		this->AddImageItem(item);
	}

}

void DentalHelper::OpenSetting(QString filepath)
{
	QString fileName = filepath + ".setting";
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_5_7);
	double centerOfPlane[3];
	double normalOfPlane[3];
	for (int i = 0; i < 3; i++)
	{
		in >> centerOfPlane[i];
		in >> normalOfPlane[i];
	}
	m_PlaneWidgetForArchCurve->SetCenter(centerOfPlane);
	m_PlaneWidgetForArchCurve->SetNormal(normalOfPlane);
	this->OnPlaneWidgetForArchCurveInteraction();
	//读取arch curve
	auto contourPointsForArchCurve= vtkSmartPointer<vtkPoints>::New();
	contourPointsForArchCurve->Initialize();
	int numOfPointsFforArchCurve;
	in >> numOfPointsFforArchCurve;
	for (int i = 0; i < numOfPointsFforArchCurve; i++)
	{
		double tempPoint[3];
		for (int j = 0; j < 3; j++)
		{
			in >> tempPoint[j];
		}
		contourPointsForArchCurve->InsertNextPoint(tempPoint);
	}
	auto initialDataForArchCurve = vtkSmartPointer<vtkPolyData>::New();
	this->GenerateContourInitialData(contourPointsForArchCurve, initialDataForArchCurve);
	m_ContourWidgetForArchCurve->Initialize(initialDataForArchCurve);
	this->OnContourWidgetForArchCurveInteraction();
	//读取左边神经
	bool isLeftNurveExist;
	in >> isLeftNurveExist;
	if (isLeftNurveExist)
	{
		auto contourPointsOfLeftNurve = vtkSmartPointer<vtkPoints>::New();
		contourPointsOfLeftNurve->Initialize();
		int numOfPointsForLeftNurve; 
		in>> numOfPointsForLeftNurve;
		for (int i = 0; i < numOfPointsForLeftNurve; i++)
		{
			double tempPoints[3];
			for (int j = 0; j < 3; j++)
			{
				in >>tempPoints[j];
			}
			contourPointsOfLeftNurve->InsertNextPoint(tempPoints);
		}
		auto initialDataForLeftNurve = vtkSmartPointer<vtkPolyData>::New();
		this->GenerateContourInitialData(contourPointsOfLeftNurve, initialDataForLeftNurve);
		ui.DrawLeftNurveButton->setChecked(true);
		ui.Panaromic3DRadioButton->setChecked(true);
		ui.Panaromic2DRadioButton->setChecked(false);
		this->OnChangePanaromicInteractionStyle();
		this->OnDrawLeftNurve();
		m_ContourForLeftNurve->Initialize(initialDataForLeftNurve);
		this->OnContourForLeftCurveInterAction();
		m_ContourForLeftNurve->Off();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawLeftNurveButton->setIcon(icon);
		ui.DrawLeftNurveButton->setChecked(false);
		//画右边神经的按钮禁用
		ui.DrawRightNurveButton->setDisabled(false);
		ui.DeleteLastRightNurvePointButton->setDisabled(false);
		ui.LowerLeftQvtkWidget->setCursor(Qt::ArrowCursor);
		m_LowerLeftRenderer->GetRenderWindow()->Render();
	}
	//读取右边神经
	bool isRightNurveExist;
	in >> isRightNurveExist;
	if (isRightNurveExist)
	{
		auto contourPointsOfRightNurve = vtkSmartPointer<vtkPoints>::New();
		contourPointsOfRightNurve->Initialize();
		int numOfPointsForLeftNurve;
		in >> numOfPointsForLeftNurve;
		for (int i = 0; i < numOfPointsForLeftNurve; i++)
		{
			double tempPoints[3];
			for (int j = 0; j < 3; j++)
			{
				in >> tempPoints[j];
			}
			contourPointsOfRightNurve->InsertNextPoint(tempPoints);
		}
		auto initialDataForRightNurve = vtkSmartPointer<vtkPolyData>::New();
		this->GenerateContourInitialData(contourPointsOfRightNurve, initialDataForRightNurve);
		ui.DrawRightNurveButton->setChecked(true);
		ui.Panaromic3DRadioButton->setChecked(true);
		ui.Panaromic2DRadioButton->setChecked(false);
		this->OnChangePanaromicInteractionStyle();
		this->OnDrawRightNurve();
		m_ContourForRightNurve->EnabledOn();
		m_ContourForRightNurve->Initialize(initialDataForRightNurve);
		this->OnContourForRightCurveInterAction();
		m_ContourForRightNurve->Off();
		QIcon icon;
		icon.addFile(QStringLiteral(":/DentalHelper/Resources/Draw.png"), QSize(), QIcon::Normal, QIcon::Off);
		ui.DrawRightNurveButton->setIcon(icon);
		ui.DrawRightNurveButton->setChecked(false);
		//画左边神经的按钮禁用
		ui.DrawLeftNurveButton->setDisabled(false);
		ui.DeleteLastLeftNurvePointButton->setDisabled(false);
		ui.LowerLeftQvtkWidget->setCursor(Qt::ArrowCursor);
		m_LowerLeftRenderer->GetRenderWindow()->Render();
	}
	//读取上颌模型
	bool isUpProthesisExist;
	in >> isUpProthesisExist;
	if (isUpProthesisExist)
	{
		QString temp = "UpProthesis";
		QString stlName = fileName + "_" + temp;
		auto stlReader = vtkSmartPointer<vtkSTLReader>::New();
		stlReader->SetFileName(qPrintable(stlName));
		stlReader->Update();

		m_UpProthesisData = stlReader->GetOutput();
		auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(m_UpProthesisData);
		m_UpProthesisActor->SetMapper(mapper);
		m_UpProthesisActor->GetProperty()->SetColor(0, 1, 0);
		m_UpProthesisActor->GetProperty()->SetAmbient(0);
		m_UpProthesisActor->GetProperty()->SetDiffuse(1);
		m_UpProthesisActor->GetProperty()->SetSpecular(0);
		if (!m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor))
		{
			m_ModelRenderer->AddActor(m_UpProthesisActor);
		}
		m_ModelRendWin->Render();
		if (isAxial)
		{
			this->CutUpProthesisInAxialView();
		}
		if (isCoronal)
		{
			this->CutUpProthesisInCoronalView();
		}
		if (isSagital)
		{
			this->CutUpProthesisInSagitalView();
		}
		if (isPosition)
		{
			this->CutUpProthesisInPositionView();
		}
		if (isRotation)
		{
			this->CutUpProthesisInRotationView();
		}
		if (isCross)
		{
			this->CutUpProthesisInCrossView();
		}
	}
	//读取下颌模型
	bool isLowerProthesisExist;
	in >> isLowerProthesisExist;
	if (isLowerProthesisExist)
	{
		QString temp = "LowerProthesis";
		QString stlName = fileName + "_" + temp;

		auto stlReader = vtkSmartPointer<vtkSTLReader>::New();
		stlReader->SetFileName(qPrintable(stlName));
		stlReader->Update();

		m_DownProthesisData = stlReader->GetOutput();
		auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(m_DownProthesisData);
		m_DownProthesisActor->SetMapper(mapper);
		m_DownProthesisActor->GetProperty()->SetColor(0, 1, 0);
		m_DownProthesisActor->GetProperty()->SetAmbient(0);
		m_DownProthesisActor->GetProperty()->SetDiffuse(1);
		m_DownProthesisActor->GetProperty()->SetSpecular(0);
		if (!m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor))
		{
			m_ModelRenderer->AddActor(m_DownProthesisActor);
		}
		m_ModelRendWin->Render();
		if (isAxial)
		{
			this->CutDownProthesisInAxialView();
		}
		if (isCoronal)
		{
			this->CutDownProthesisInCoronalView();
		}
		if (isSagital)
		{
			this->CutDownProthesisInSagitalView();
		}
		if (isRotation)
		{
			this->CutDownProthesisInRotationView();
		}
		if (isPosition)
		{
			this->CutDownProthesisInPositionView();
		}
		if (isCross)
		{
			this->CutDownProthesisInCrossView();
		}
	}
}


void DentalHelper::PickImplantPoint()
{
	double screenPosition[3];
	screenPosition[0] = m_LowerRightInteractor->GetEventPosition()[0];
	screenPosition[1] = m_LowerRightInteractor->GetEventPosition()[1];
	screenPosition[2] = 0;

	double pickedPoint[3];
	auto picker = vtkSmartPointer<vtkWorldPointPicker>::New();
	m_LowerRightInteractor->SetPicker(picker);
	picker->SetPickFromList(1);
	picker->AddPickList(m_ReslicePropOfCrossIn2D);
	picker->Pick(screenPosition[0], screenPosition[1], screenPosition[2], m_LowerRightRenderer);
	picker->GetPickPosition(pickedPoint);

	if (m_DrawImplantState==DrawImplantState::none)
	{	

		m_FirstPointWidget->SetCenter(pickedPoint);
		m_FirstPointWidget->On();
		m_DrawImplantState = first;
		m_EventQtConnector->Connect(m_FirstPointWidget, vtkCommand::InteractionEvent, this, SLOT(OnFirstPointWidgetInteraction()));
		return;
	}
	if (m_DrawImplantState== DrawImplantState::first)
	{
		m_FirstPointWidget->Off();
		m_EventQtConnector->Disconnect(m_FirstPointWidget, vtkCommand::InteractionEvent, this, SLOT(OnFirstPointWidgetInteraction()));

		ImplantItem* item = new ImplantItem(this,m_FirstPointWidget->GetCenter(),pickedPoint,ui.ImplantDiameterSpinBox->value(),ui.ImplantLengthSpinBox->value());
		item->SetRenderers(m_ModelRenderer, m_UpRightRenderer, m_LowerLeftRenderer, m_LowerRightRenderer);

		m_ImplantList.append(item);
		int numOfImplant = m_ImplantList.size();
		item->SetIndex(numOfImplant);
	//更新当前显示的种植体
		m_CurrentImplant = numOfImplant - 1;	
		m_CurrentParallel= numOfImplant - 1;
		this->CalculateParallelAngle();

		this->OnUpdateCurrentImplant(m_CurrentImplant + 1);

		connect(item, SIGNAL(radioButtonChecked(int)), this, SLOT(OnUpdateCurrentImplant(int)));
		connect(item, SIGNAL(ImplantChanged(ImplantItem*)), this, SLOT(OnImplantChanged(ImplantItem*)));
		connect(item, SIGNAL(deleteImplant(ImplantItem*)), this, SLOT(OnDeleteImplant(ImplantItem*)));
		connect(item, SIGNAL(parallelRadioButtonClicked(ImplantItem*)), this, SLOT(OnParallelRadioButton(ImplantItem*)));

		//设置交互widget的中心
		this->SetImplantInteractionWidget(item->m_ImplantFirstPoint, item->m_NormalOfTube, item->m_ImplantLength);
		for (int i = 0; i < 3; i++)
		{
			centerOfPreviousMoveWiget[i] = (item->m_ImplantFirstPoint[i]+item->m_ImplantSecondPoint[i])/2;
		}
		ui.ImplantTableWidget->setRowCount(1);
		ui.ImplantTableWidget->setRowHeight(0,75);

		ui.ImplantTableWidget->setColumnCount(numOfImplant);
		ui.ImplantTableWidget->setColumnWidth(numOfImplant - 1, 145);
		 
		ui.ImplantTableWidget->setCellWidget(0, numOfImplant - 1, item->GetImplantWidget());

		ui.BaseTableWidget->setRowCount(1);
		ui.BaseTableWidget->setRowHeight(0,75);
		ui.BaseTableWidget->setColumnCount(numOfImplant);
		ui.BaseTableWidget->setColumnWidth(numOfImplant - 1, 130);
		ui.BaseTableWidget->setCellWidget(0, numOfImplant - 1, item->GetBaseWidget());


		ui.ParallelTableWidget->setRowCount(1);
		ui.ParallelTableWidget->setRowHeight(0, 75);
		ui.ParallelTableWidget->setColumnCount(numOfImplant);
		ui.ParallelTableWidget->setColumnWidth(numOfImplant - 1, 130);
		ui.ParallelTableWidget->setCellWidget(0, numOfImplant - 1, item->GetParalleWiget());

		m_EventQtConnector->Disconnect(m_LowerRightInteractor, vtkCommand::RightButtonPressEvent, this, SLOT(PickImplantPoint()));

		m_DrawImplantState = DrawImplantState::none;
		ui.AddImplantButton->setChecked(false);

		this->OnChange2Position();
		this->GeneratePositionReslice();
		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInUpRightView(m_PositionPlane);
		}
		m_UpRightRenderer->ResetCamera();
		m_UpRightRendWin->Render();

		this->GenerateRotationReslice();
		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInLowerLeftView(m_RotationPlane);
		}
		this->OnChange2Rotation();
		for (int i=0;i<3;i++)
		{
			item->normalForGeneratingBase[i] = m_RotationPlane->GetNormal()[i];
		}
		item->GenerateBase();
		item->CutInLowerLeftView(m_RotationPlane);

		m_LowerLeftRenderer->ResetCamera();
		m_LowerLeftRendWin->Render();

		for each (ImplantItem* var in m_ImplantList)
		{
			var->CutInLowerRightView(m_CrossPlane);
		}
		m_LowerRightRendWin->Render();
	}

}


void DentalHelper::ReadImageFile(QString dir)
{
	auto nameGenerator = itk::GDCMSeriesFileNames::New();
	nameGenerator->SetUseSeriesDetails(false);
	nameGenerator->SetDirectory(qPrintable(dir));
	std::vector<std::string> seriesUID;
	try
	{
		nameGenerator->Update();
	}
	catch (itk::ExceptionObject & exp)
	{
		QString msg;
		msg.fromStdString(exp.GetDescription());
		QMessageBox::information(this, "Error", msg + '\n' + "please select the right dir");
		return;
	}
	seriesUID = nameGenerator->GetSeriesUIDs();
	if (seriesUID.empty()) return;


	for (auto seriesItr = seriesUID.begin(); seriesItr != seriesUID.end(); ++seriesItr)
	{

		XDicomItem* item = new XDicomItem(this);
		connect(item, SIGNAL(ProgressUpdate(int)), this, SLOT(OnSetProgressBar(int)));
		item->SetFileName(nameGenerator->GetFileNames(seriesItr->c_str()));
		item->ReadDicomFile();

		m_DicomList.append(item);
		int numOfItem = m_DicomList.size();
		ui.ImageTabelWidget->setRowCount(numOfItem - 1);
		ui.ImageTabelWidget->insertRow(numOfItem - 1);

		for each(QString text in item->GetMainList())
		{
			int indexOfItem = item->GetMainList().indexOf(text);
			QTableWidgetItem *itemText = new QTableWidgetItem(text);
			itemText->setTextAlignment(Qt::AlignCenter);
			ui.ImageTabelWidget->setItem(numOfItem - 1, indexOfItem, itemText);
		}
		QTableWidgetItem *LastText = new QTableWidgetItem("...");
		LastText->setTextAlignment(Qt::AlignCenter);
		ui.ImageTabelWidget->setItem(numOfItem - 1, item->GetMainList().size(), LastText);
	}
}
void DentalHelper::SaveDicomInformation(QDataStream& out, XDicomItem &data)
{
	out << data.patientName << data.patientGender << data.patientAge << data.SerialNum << data.patientID << data.date;
	out << data.spacingX << data.spacingY << data.spacingZ << data.manufacturer << data.hospital;
}

void DentalHelper::SaveImages(QString filepath)
{
	QString fileName = filepath + ".manager";
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_7);
	out << m_DicomList.size();
	out << m_CurrentIndex;
	for each (XDicomItem* var in m_DicomList)
	{
		this->SaveDicomInformation(out, *var);
		QString temp;
		QString vtiName = filepath + "_" + temp.setNum(m_DicomList.indexOf(var));
		auto imageWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
		imageWriter->SetFileName(qPrintable(vtiName.append(".vti")));
		imageWriter->SetDataModeToBinary();
		imageWriter->SetInputData(var->GetImageData());
		imageWriter->Write();
	}

}


void DentalHelper::SaveImplants(QString filepath)
{
	QString fileName = filepath + ".implant";
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_7);
	out << m_ImplantList.size();
	for each (ImplantItem* var in m_ImplantList)
	{
		out << 1;
		for (int i = 0; i < 3; i++)
		{
			out << var->m_ImplantFirstPoint[i];
			out << var->m_ImplantSecondPoint[i];
			out << var->m_ImplantColor[i];
			out << var->normalOfBase[i];
		}
		out << var->m_ImplantDiameter;
		out << var->m_ImplantLength;
		out << var->GetBaseAngle();
		out << var->GetBaseLength();
	}
}


void DentalHelper::SavePrePlanning(QString filepath)
{
	QString fileName = filepath + ".setting";
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_7);
	double centerOfPlane[3]; m_PlaneWidgetForArchCurve->GetCenter(centerOfPlane);
	double normalOfPlane[3]; m_PlaneWidgetForArchCurve->GetNormal(normalOfPlane);
	for (int i = 0; i < 3; i++)
	{
		out << centerOfPlane[i];
		out << normalOfPlane[i];
	}

	//保存牙弓弧线
	auto contourNodesOfArchCurve = vtkSmartPointer<vtkPolyData>::New();
	m_ContourRepForArchCurve->GetNodePolyData(contourNodesOfArchCurve);
	auto contourPointsOfArchCurve = vtkSmartPointer<vtkPoints>::New();
	contourPointsOfArchCurve = contourNodesOfArchCurve->GetPoints();
	int numOfPointsForArchCurve = contourPointsOfArchCurve->GetNumberOfPoints();
	out << numOfPointsForArchCurve;
	for (int i = 0; i < numOfPointsForArchCurve; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			out << contourPointsOfArchCurve->GetPoint(i)[j];
		}
	}
	//保存左边神经
	bool isLeftNurveExist = m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForLeftNurve);
	out << isLeftNurveExist;
	if (isLeftNurveExist)
	{
		auto contourNodesOfLeftNurve = vtkSmartPointer<vtkPolyData>::New();
		m_ContourRepForLeftNurve->GetNodePolyData(contourNodesOfLeftNurve);
		auto contourPointsOfLeftNurve = vtkSmartPointer<vtkPoints>::New();
		contourPointsOfLeftNurve = contourNodesOfLeftNurve->GetPoints();
		int numOfPointsForLeftNurve = contourPointsOfLeftNurve->GetNumberOfPoints();
		out << numOfPointsForLeftNurve;
		for (int i = 0; i < numOfPointsForLeftNurve; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				out << contourPointsOfLeftNurve->GetPoint(i)[j];
			}
		}
	}
	//保存右边神经
	bool isRightNurveExist = m_ModelRenderer->GetActors()->IsItemPresent(m_ActorForRightNurve);
	out << isRightNurveExist;
	if (isRightNurveExist)
	{
		auto contourNodesOfRightNurve = vtkSmartPointer<vtkPolyData>::New();
		m_ContourRepForRightNurve->GetNodePolyData(contourNodesOfRightNurve);
		auto contourPointsOfRightNurve = vtkSmartPointer<vtkPoints>::New();
		contourPointsOfRightNurve = contourNodesOfRightNurve->GetPoints();
		int numOfPointsForLeftNurve = contourPointsOfRightNurve->GetNumberOfPoints();
		out << numOfPointsForLeftNurve;
		for (int i = 0; i < numOfPointsForLeftNurve; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				out << contourPointsOfRightNurve->GetPoint(i)[j];
			}
		}
	}
	//保存上颌模型
	bool isUpProthesisExist= m_ModelRenderer->GetActors()->IsItemPresent(m_UpProthesisActor);
	out << isUpProthesisExist;
	if (isUpProthesisExist)
	{
		QString temp = "UpProthesis";
		QString stlName = fileName + "_" + temp;
		auto stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
		stlWriter->SetInputData(m_UpProthesisData);
		stlWriter->SetFileTypeToBinary();
		stlWriter->SetFileName(qPrintable(stlName));
		stlWriter->Update();
	}
	//保存下颌模型
	bool isLowerProthesisExist = m_ModelRenderer->GetActors()->IsItemPresent(m_DownProthesisActor);
	out << isLowerProthesisExist;
	if (isLowerProthesisExist)
	{
		QString temp = "LowerProthesis";
		QString stlName = fileName + "_" + temp;
		auto stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
		stlWriter->SetInputData(m_DownProthesisData);
		stlWriter->SetFileTypeToBinary();
		stlWriter->SetFileName(qPrintable(stlName));
		stlWriter->Update();
	}
}

void DentalHelper::SetActorsVisibilityInArchCurve(bool vis)
{
	if (!vis)
	{
		isArchCurve = false;
		if (m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropForArchCurve))
		{
			m_ReslicePropForArchCurve->VisibilityOff();
			m_ContourWidgetForArchCurve->Off();
			m_ContourOffSetActor->VisibilityOff();
		}
	}
	else
	{
		isArchCurve = true;
		if (m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropForArchCurve))
		{
			m_ReslicePropForArchCurve->VisibilityOn();
			m_ContourWidgetForArchCurve->On();
			m_ContourOffSetActor->VisibilityOn();
			m_UpRightRenderer->ResetCamera();
			this->UpDateCamera(m_UpRightRenderer, m_PlaneWidgetForArchCurve->GetNormal(), 90);
		}

	}
	m_UpRightRendWin->Render();
}



void DentalHelper::SetActorsVisibilityInAxial(bool vis)
{
	if (!vis)
	{
		isAxial = false;
		//隐藏图像
		m_AxialViewer->GetImageActor()->VisibilityOff();
		//隐藏标线
		m_Sagital_AxialLineActor->VisibilityOff();
		m_Coronal_AxialLineActor->VisibilityOff();
		//应该还有其他的东西需要隐藏，比如轮廓线，标号等...
		//显示神经轮廓
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInAxial))
		{
			m_CutActorForLeftNurveInAxial->VisibilityOff();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInAxial))
		{
			m_CutActorForRightNurveInAxial->VisibilityOff();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInAxial))
		{
			m_CutActorForUpProthesisInAxial->VisibilityOff();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInAxial))
		{
			m_CutActorForDownProthesisInAxial->VisibilityOff();
		}
	}
	else
	{
		isAxial = true;
		//显示图像
		m_AxialViewer->GetImageActor()->VisibilityOn();
		//显示标线
		m_Sagital_AxialLineActor->VisibilityOn();
		m_Coronal_AxialLineActor->VisibilityOn();
		//根据其他条件，决定是否需要显示其他东西...
		//显示神经轮廓
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInAxial))
		{
			m_CutActorForLeftNurveInAxial->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInAxial))
		{
			m_CutActorForRightNurveInAxial->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInAxial))
		{
			m_CutActorForUpProthesisInAxial->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInAxial))
		{
			m_CutActorForDownProthesisInAxial->VisibilityOn();
		}
	}
	m_UpRightRendWin->Render();
}


void DentalHelper::SetActorsVisibilityInCoronal(bool vis)
{

	if (!vis)
	{
		isCoronal = false;
		//隐藏图像
		m_CoronalViewer->GetImageActor()->VisibilityOff();
		//隐藏标线
		m_Sagital_CoronalLineActor->VisibilityOff();
		m_Axial_CoronalLineActor->VisibilityOff();
		//应该还有其他的东西需要隐藏，比如轮廓线，标号等...
		//显示神经轮廓
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInCoronal))
		{
			m_CutActorForLeftNurveInCoronal->VisibilityOff();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInCoronal))
		{
			m_CutActorForRightNurveInCoronal->VisibilityOff();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCoronal))
		{
			m_CutActorForUpProthesisInCoronal->VisibilityOff();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCoronal))
		{
			m_CutActorForDownProthesisInCoronal->VisibilityOff();
		}
	}
	else
	{
		isCoronal = true;
		//显示图像
		m_CoronalViewer->GetImageActor()->VisibilityOn();
		//显示标线
		m_Sagital_CoronalLineActor->VisibilityOn();
		m_Axial_CoronalLineActor->VisibilityOn();
		//根据其他条件，决定是否需要显示其他东西...
		//显示神经轮廓
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInCoronal))
		{
			m_CutActorForLeftNurveInCoronal->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInCoronal))
		{
			m_CutActorForRightNurveInCoronal->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCoronal))
		{
			m_CutActorForUpProthesisInCoronal->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCoronal))
		{
			m_CutActorForDownProthesisInCoronal->VisibilityOn();
		}

	}
	m_LowerLeftRendWin->Render();
}
void DentalHelper::SetActorsVisibilityInCross(bool vis)
{
	if (!vis)//如果是隐藏
	{
		isPanaromic = false;
		m_ReslicePropOfCrossIn2D->VisibilityOff();
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInCross))
		{
			m_CutActorForLeftNurveInCross->VisibilityOff();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInCross))
		{
			m_CutActorForRightNurveInCross->VisibilityOff();
		}

		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCross))
		{
			m_CutActorForUpProthesisInCross->VisibilityOff();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCross))
		{
			m_CutActorForDownProthesisInCross->VisibilityOff();
		}
	}
	else
	{
		isCross = true;
		m_ReslicePropOfCrossIn2D->VisibilityOn();
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInCross))
		{
			m_CutActorForLeftNurveInCross->VisibilityOn();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInCross))
		{
			m_CutActorForRightNurveInCross->VisibilityOn();
		}

		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInCross))
		{
			m_CutActorForUpProthesisInCross->VisibilityOn();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInCross))
		{
			m_CutActorForDownProthesisInCross->VisibilityOn();
		}
	}
	m_LowerRightRendWin->Render();
}


void DentalHelper::SetActorsVisibilityInPanaromic(bool vis)
{
	if (!vis)//如果隐藏panaromic里的actor
	{
		isPanaromic = false;
		m_PanaromicActorForDrawingNurve->VisibilityOff();
		m_PanaromicActorIn2D->VisibilityOff();
		//应该还有其他的东西需要隐藏，比如轮廓线，标号等...
	}
	else
	{
		if (ui.Panaromic2DRadioButton->isChecked())
		{
			m_LowerLeftInteractor->SetInteractorStyle(m_LowerLeftInterStyle);
			m_PanaromicActorIn2D->VisibilityOn();
			m_PanaromicActorForDrawingNurve->VisibilityOff();
			double normalOfCamera[3] = { 0,1,0 };
			m_LowerLeftRenderer->ResetCamera();
			this->UpDateCamera(m_LowerLeftRenderer, normalOfCamera, 90);
		}
		else
		{
			isPanaromic = true;
			m_LowerLeftInteractor->SetInteractorStyle(m_LowerLeftModelStyle);
			m_PanaromicActorIn2D->VisibilityOff();
			m_PanaromicActorForDrawingNurve->VisibilityOn();
			m_LowerLeftRenderer->ResetCamera();
		}
	}
	m_LowerLeftRendWin->Render();
}
void DentalHelper::SetActorsVisibilityInPosition(bool vis)
{
	if (!vis)
	{
		isPosition = false;
		if (m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfPositionIn2D))
		{
			m_ReslicePropOfPositionIn2D->VisibilityOff();
		}
		//应该还有其他的东西需要隐藏，比如轮廓线，标号等...
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInPosition))
		{
			m_CutActorForLeftNurveInPosition->VisibilityOff();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInPosition))
		{
			m_CutActorForRightNurveInPosition->VisibilityOff();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInPosition))
		{
			m_CutActorForUpProthesisInPosition->VisibilityOff();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInPosition))
		{
			m_CutActorForDownProthesisInPosition->VisibilityOff();
		}
	}
	else
	{
		isPosition = true;
		if (m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfPositionIn2D))
		{
			m_ReslicePropOfPositionIn2D->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInPosition))
		{
			m_CutActorForLeftNurveInPosition->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInPosition))
		{
			m_CutActorForRightNurveInPosition->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInPosition))
		{
			m_CutActorForUpProthesisInPosition->VisibilityOn();
		}
		if (m_UpRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInPosition))
		{
			m_CutActorForDownProthesisInPosition->VisibilityOn();
		}
		this->UpDateCamera(m_UpRightRenderer, m_PositionPlane->GetNormal(), 90);
		m_UpRightRenderer->ResetCamera();
	}
	m_UpRightRendWin->Render();
}

void DentalHelper::SetActorsVisibilityInRotation(bool vis)
{
	if (!vis)
	{
		isRotation = false;
		if (m_LowerLeftRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfRotationIn2D))
		{
			m_ReslicePropOfRotationIn2D->VisibilityOff();
		}
		//应该还有其他的东西需要隐藏，比如轮廓线，标号等...
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInRotation))
		{
			m_CutActorForLeftNurveInRotation->VisibilityOff();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CUtActorForRightNurveInRotation))
		{
			m_CUtActorForRightNurveInRotation->VisibilityOff();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInRotation))
		{
			m_CutActorForUpProthesisInRotation->VisibilityOff();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInRotation))
		{
			m_CutActorForDownProthesisInRotation->VisibilityOff();
		}

		m_FirstRotateWidget->Off();
		m_SecondRotateWidget->Off();
		m_MoveWidget->Off();
	}
	else
	{
		isRotation = true;
		if (m_LowerLeftRenderer->GetViewProps()->IsItemPresent(m_ReslicePropOfRotationIn2D))
		{
			m_ReslicePropOfRotationIn2D->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInRotation))
		{
			m_CutActorForLeftNurveInRotation->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CUtActorForRightNurveInRotation))
		{
			m_CUtActorForRightNurveInRotation->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInRotation))
		{
			m_CutActorForUpProthesisInRotation->VisibilityOn();
		}
		if (m_LowerLeftRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInRotation))
		{
			m_CutActorForDownProthesisInRotation->VisibilityOn();
		}
		this->UpDateCamera(m_LowerLeftRenderer, m_RotationPlane->GetNormal(), 90);
		m_LowerLeftRenderer->ResetCamera();
		m_FirstRotateWidget->On();
		m_SecondRotateWidget->On();
		m_MoveWidget->On();
	}

	m_LowerLeftRendWin->Render();
}



void DentalHelper::SetActorsVisibilityInSagital(bool vis)
{
	if (!vis)
	{
		isSagital = false;
		//隐藏图像
		m_SagitalViewer->GetImageActor()->VisibilityOff();
		//隐藏标线
		m_Coronal_SagitalLineActor->VisibilityOff();
		m_Axial_SagitalLineActor->VisibilityOff();
		//应该还有其他的东西需要隐藏，比如轮廓线，标号等...
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInSagital))
		{
			m_CutActorForDownProthesisInSagital->VisibilityOff();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInSagital))
		{
			m_CutActorForLeftNurveInSagital->VisibilityOff();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInSagital))
		{
			m_CutActorForRightNurveInSagital->VisibilityOff();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInSagital))
		{
			m_CutActorForUpProthesisInSagital->VisibilityOff();
		}
	}
	else
	{
		isSagital = true;
		//显示图像
		m_SagitalViewer->GetImageActor()->VisibilityOn();
		//显示标线
		m_Coronal_SagitalLineActor->VisibilityOn();
		m_Axial_SagitalLineActor->VisibilityOn();
		//根据其他条件，决定是否需要显示其他东西...
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForDownProthesisInSagital))
		{
			m_CutActorForDownProthesisInSagital->VisibilityOn();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForLeftNurveInSagital))
		{
			m_CutActorForLeftNurveInSagital->VisibilityOn();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForRightNurveInSagital))
		{
			m_CutActorForRightNurveInSagital->VisibilityOn();
		}
		if (m_LowerRightRenderer->GetActors()->IsItemPresent(m_CutActorForUpProthesisInSagital))
		{
			m_CutActorForUpProthesisInSagital->VisibilityOn();
		}
	}
	m_LowerRightRendWin->Render();
}

void DentalHelper::SetButtonColor(QPushButton* button, double* color)
{
	QString style_pannel = "border:2px solid gray;border-radius:10px;padding:2px 4px;";
	int color_int[3];
	for (int i = 0; i < 3; i++)
	{
		color_int[i] = color[i] * 255;
	}
	QString temp;
	QString background_color = "background-color: rgb(" + temp.setNum(color_int[0]) + "," + temp.setNum(color_int[1]) + "," + temp.setNum(color_int[2]) + ");";

	button->setStyleSheet(style_pannel + background_color);
}


void DentalHelper::SetImplantInteractionWidget(double* first, double* normal,double length)
{
	//设置绕第一个widget的中心
	double centerOfFirstWidget_[3];
	for (int i=0;i<3;i++)
	{
		centerOfFirstWidget_[i] = first[i] - length / 4.0*normal[i];
	}
	m_FirstRotateWidget->SetCenter(centerOfFirstWidget_);
	if (!m_FirstRotateWidget->GetEnabled())
	{	
		m_FirstRotateWidget->On();
		//设置交互函数
		m_EventQtConnector->Connect(m_FirstRotateWidget, vtkCommand::EndInteractionEvent, this, SLOT(OnFirstRotateWidgetInteraction()));
	}


	//设置第二个widget的中心
	double centerOfSecondWidget_[3];
	for (int i=0;i<3;i++)
	{
		centerOfSecondWidget_[i] = first[i] - length * 3 / 4.0*normal[i];
	}
	m_SecondRotateWidget->SetCenter(centerOfSecondWidget_);
	if (!m_SecondRotateWidget->GetEnabled())
	{	
		m_SecondRotateWidget->On();
		//设置交互函数
		m_EventQtConnector->Connect(m_SecondRotateWidget, vtkCommand::EndInteractionEvent, this, SLOT(OnSecondRotateWidgetInteraction()));
	}




	//设置移动widget的中心
	double centerOfMoveWidget_[3];
	for (int i=0;i<3;i++)
	{
		centerOfMoveWidget_[i] = first[i] - length / 2.0*normal[i];
	}
	m_MoveWidget->SetCenter(centerOfMoveWidget_);
	if (!m_MoveWidget->GetEnabled())
	{	
		m_MoveWidget->On();
		//设置交互函数
		m_EventQtConnector->Connect(m_MoveWidget, vtkCommand::EndInteractionEvent, this, SLOT(OnMoveWidgetInteraction()));
	}



	m_LowerLeftRendWin->Render();
}


void DentalHelper::SetSingleViewer(vtkRenderer* ren, vtkImageViewer2* viewer, int sliceIndex, int mark)
{
	
	viewer->SetRenderWindow(ren->GetRenderWindow());
	viewer->SetRenderer(ren);
	viewer->SetupInteractor(ren->GetRenderWindow()->GetInteractor());
	viewer->SetInputData(m_ImageData);
	switch (mark)
	{
	case  0:
				{	viewer->SetSliceOrientationToXY(); break;}//Axial viewer
	case 1:
				{	viewer->SetSliceOrientationToXZ(); break; }//Coronal viewer
	case 2:
				{	viewer->SetSliceOrientationToYZ(); break; }//Sagital viewer
	}
	viewer->SetSlice(sliceIndex);
	viewer->SetColorWindow(ui.WindowSlider->value());
	viewer->SetColorLevel(ui.LevelSlider->value());
	viewer->Render();
	ren->ResetCamera();
	ren->GetRenderWindow()->Render();
}


void DentalHelper::SetSuitableLayout()
{
	ui.ViewGridLayout->removeWidget(ui.UpLeftLayout);
	ui.ViewGridLayout->removeWidget(ui.UpRightLayout);
	ui.ViewGridLayout->removeWidget(ui.LowerLeftLayout);
	ui.ViewGridLayout->removeWidget(ui.LowerRightLayout);
	//全显示 
	//11
	//11
	if (isShowLowerLeftView&&isShowLowerRightView&&isShowUpLeftView&&isShowUpRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout,0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 1, 0);
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 1);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 1, 1);
	}
	if (!isShowLowerLeftView&&!isShowLowerRightView&&!isShowUpLeftView&&!isShowUpRightView)
	{
		this->OnUpLeftViewButton();
		this->OnUpRightViewButton();
		this->OnLowerLeftViewButton();
		this->OnLowerRightViewButton();
		ui.UpLeftLayout->show();
		ui.UpRightLayout->show();
		ui.LowerLeftLayout->show();
		ui.LowerRightLayout->show();
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 1, 0);
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 1);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 1, 1);
	}
	//01
	//11
	if (!isShowUpLeftView&&isShowUpRightView&&isShowLowerLeftView&&isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 0, 0, 2, 1);
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 1);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 1, 1);
	}
	//00
	//11
	if (!isShowUpLeftView&&!isShowUpRightView&&isShowLowerLeftView&&isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 0, 1);
	}
	//00
	//01
	if (!isShowUpLeftView && !isShowUpRightView&&!isShowLowerLeftView&&isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 1, 1);
	}
	//10
	//11
	if (isShowUpLeftView&&!isShowUpRightView&&isShowLowerLeftView&&isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 1, 0);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 0, 1,2,1);
	}
	//10
	//01
	if (!isShowLowerLeftView && isShowLowerRightView&&isShowUpLeftView&&!isShowUpRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 0,1);
	}
	//10
	//00
	if (isShowUpLeftView && !isShowUpRightView&&!isShowLowerLeftView&&!isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
	}
	//11
	//01
	if (isShowUpLeftView && isShowUpRightView && !isShowLowerLeftView && isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0,2,1);
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 1);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 1, 1);
	}
	//11
	//00
	if (isShowUpLeftView && isShowUpRightView && !isShowLowerLeftView &&! isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 1);
	}
	//11
	//10
	if (isShowUpLeftView && isShowUpRightView && isShowLowerLeftView && !isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 1,2,1);
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 1, 0);
	}
	//10
	//10
	if (isShowUpLeftView && !isShowUpRightView && isShowLowerLeftView && !isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpLeftLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 0,1);
	}
	//01
	//10
	if (!isShowUpLeftView && isShowUpRightView && isShowLowerLeftView && !isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 0,1);
	}
	//01
	//01
	if (!isShowUpLeftView && isShowUpRightView && !isShowLowerLeftView && isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 0);
		ui.ViewGridLayout->addWidget(ui.LowerRightLayout, 0, 1);
	}
	//01
	//00
	if (!isShowUpLeftView && isShowUpRightView && !isShowLowerLeftView && !isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.UpRightLayout, 0, 0);
	}
	//00
	//10
	if (!isShowUpLeftView && !isShowUpRightView && isShowLowerLeftView && !isShowLowerRightView)
	{
		ui.ViewGridLayout->addWidget(ui.LowerLeftLayout, 0, 0);
	}

}
void DentalHelper::SetViewModel2Conventional()
{
	//设置axial 视图
	int axial_middle = (m_Extend[5] - m_Extend[4]) / 2;	
	ui.UpRightSlider->setMaximum(m_Extend[5] - m_Extend[4]);
	ui.UpRightSpinBox->setMaximum(m_Extend[5] - m_Extend[4]);
	ui.UpRightSlider->setValue(axial_middle);
	this->SetSingleViewer(m_UpRightRenderer, m_AxialViewer, axial_middle, 0);
	ui.ChangeUpRightViewButton->setText("Axial");
	m_Change2ArchCurve->setChecked(false);
	m_Change2Axial->setChecked(true);
	

	//设置coronal视图
	int coronal_middle = (m_Extend[3] - m_Extend[2]) / 2;	
	ui.LowerLeftSlider->setMaximum(m_Extend[3] - m_Extend[2]);
	ui.LowerLeftSpinBox->setMaximum(m_Extend[3] - m_Extend[2]);
	ui.LowerLeftSlider->setValue(coronal_middle);
	this->SetSingleViewer(m_LowerLeftRenderer, m_CoronalViewer, coronal_middle, 1);
	ui.ChangeLowerLeftButton->setText("Coronal");
	m_Change2Coronal->setChecked(true);
	m_Change2Panaromic->setChecked(false);
	
	//设置sagital视图
	int sagital_middle = (m_Extend[1] - m_Extend[0]) / 2;	
	ui.LowerRightSlider->setMaximum(m_Extend[1] - m_Extend[0]);
	ui.LowerRightSpinBox->setMaximum(m_Extend[1] - m_Extend[0]);
	ui.LowerRightSlider->setValue(sagital_middle);
	this->SetSingleViewer(m_LowerRightRenderer, m_SagitalViewer, sagital_middle, 2);;
	ui.ChangeLowerRightButton->setText("Sagital");
	m_Change2Sagital->setChecked(true);
	m_Change2Cross->setChecked(false);

	this->DrawAxialLine();
	this->DrawCoronalLine();
	this->DrawSagitalLine();
}

void DentalHelper::SetViewModel2Preplan()
{
	m_WorkingState = WorkState::preplanning;
	ui.ChangeUpRightViewButton->setText("Arch Curve");
	m_Change2ArchCurve->setChecked(true);
	m_Change2Axial->setChecked(false);
	ui.ChangeLowerLeftButton->setText("Panaromic");
	m_Change2Panaromic->setChecked(true);
	m_Change2Coronal->setChecked(false);

	ui.ChangeLowerRightButton->setText("Cross");
	m_Change2Sagital->setChecked(false);
	m_Change2Cross->setChecked(true);
	//设定planewidget的point1和point2
	double point1ForPlaneWidget[3], point2ForPlaneWidget[3], boundsOfProp[6];
	m_VolumeActor->GetBounds(boundsOfProp);
	point1ForPlaneWidget[0] = boundsOfProp[0]; 
	point1ForPlaneWidget[1] = boundsOfProp[2];
	point1ForPlaneWidget[2] = (boundsOfProp[4] + boundsOfProp[5]) / 2;

	point2ForPlaneWidget[0] = boundsOfProp[1];
	point2ForPlaneWidget[1] = boundsOfProp[3];
	point2ForPlaneWidget[2] = (boundsOfProp[4] + boundsOfProp[5]) / 2;
	//计算planewidget的中心
	double OriginOfPlaneWidget_[3];
	OriginOfPlaneWidget_[0] = boundsOfProp[0];
	OriginOfPlaneWidget_[1] = boundsOfProp[3];
	OriginOfPlaneWidget_[2] = (boundsOfProp[4] + boundsOfProp[5]) / 2;
	//初始化planewidget
	m_PlaneWidgetForArchCurve->SetCenter(OriginOfPlaneWidget_);
	m_PlaneWidgetForArchCurve->SetPoint1(point1ForPlaneWidget);
	m_PlaneWidgetForArchCurve->SetPoint2(point2ForPlaneWidget);
	m_PlaneWidgetForArchCurve->SetInteractor(m_ModelInteractor);
	m_PlaneWidgetForArchCurve->SetResolution(200);
	m_PlaneWidgetForArchCurve->SetHandleSize(0.01);
	m_PlaneWidgetForArchCurve->GetPlaneProperty()->SetColor(0.5, 0.8, 0.3);
	m_PlaneWidgetForArchCurve->GetHandleProperty()->SetColor(1, 0, 0);
	m_PlaneWidgetForArchCurve->GetHandleProperty()->SetAmbientColor(1, 0, 0);
	m_PlaneWidgetForArchCurve->GetHandleProperty()->SetAmbient(0.5);
	m_PlaneWidgetForArchCurve->SetRepresentationToSurface();
	m_PlaneWidgetForArchCurve->PlaceWidget();
	m_PlaneWidgetForArchCurve->EnabledOn();

	m_EventQtConnector->Connect(m_PlaneWidgetForArchCurve, vtkCommand::InteractionEvent, this, SLOT(OnPlaneWidgetForArchCurveInteraction()));
	//隐藏右上方视图中，axial的actor
	this->SetActorsVisibilityInAxial(false);
	this->SetWindowText(m_UpRightRenderer, "Arch Curve", AxialColor);
	//显示画牙弓曲线的平面
	auto planeForArchCurve = vtkSmartPointer<vtkPlane>::New();
	m_PlaneWidgetForArchCurve->GetPlane(planeForArchCurve);

	m_ResliceMapperForArchCurve->SetInputData(m_ImageData);
	m_ResliceMapperForArchCurve->SetSlicePlane(planeForArchCurve);

	auto propertySlice = vtkSmartPointer<vtkImageProperty>::New();
	propertySlice->SetColorWindow(ui.WindowSlider->value());
	propertySlice->SetColorLevel(ui.LevelSlider->value());
	propertySlice->SetAmbient(1.0);
	propertySlice->SetDiffuse(1.0);
	propertySlice->SetOpacity(1.0);
	propertySlice->SetInterpolationTypeToLinear();
	propertySlice->SetUseLookupTableScalarRange(1);

	m_ReslicePropForArchCurve->SetMapper(m_ResliceMapperForArchCurve);
	m_ReslicePropForArchCurve->SetProperty(propertySlice);

	if (!m_UpRightRenderer->GetViewProps()->IsItemPresent(m_ReslicePropForArchCurve))
	{
		m_UpRightRenderer->AddViewProp(m_ReslicePropForArchCurve);
	}
	m_UpRightRendWin->Render();

	//初始化contour widget
	this->GenerateContourWidgetPolydata();//初始化contour widget 的点
	m_ContourRepForArchCurve->SetPointPlacer(m_ContourFocalPlanePlacer);
	m_ContourRepForArchCurve->SetLineInterpolator(m_ContourLineInterpolator);
	m_ContourRepForArchCurve->AlwaysOnTopOn();
	m_ContourRepForArchCurve->GetLinesProperty()->SetLineWidth(5);
	m_ContourRepForArchCurve->GetLinesProperty()->SetColor(0.6, 0.4, 0.6);
	m_ContourRepForArchCurve->GetProperty()->SetColor(0.4, 0.2, 0.5);
	m_ContourRepForArchCurve->GetProperty()->SetPointSize(4);
	m_ContourWidgetForArchCurve->SetInteractor(m_UpRightInteractor);
	m_ContourWidgetForArchCurve->FollowCursorOff();
	m_ContourWidgetForArchCurve->SetRepresentation(m_ContourRepForArchCurve);
	m_ContourWidgetForArchCurve->On();
	m_ContourWidgetForArchCurve->Initialize(m_ContourInitialData);
	m_UpRightRenderer->ResetCamera();
	m_UpRightRendWin->Render();
	m_EventQtConnector->Connect(m_ContourWidgetForArchCurve, vtkCommand::InteractionEvent, this, SLOT(OnContourWidgetForArchCurveInteraction()));
	//样条曲线
	auto cardinalSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	cardinalSpline->SetLeftConstraint(2);
	cardinalSpline->SetRightConstraint(2);
	cardinalSpline->SetLeftValue(0);
	cardinalSpline->SetRightValue(0);
	//插值样条曲线
	m_ContourSplineFilter->SetSpline(cardinalSpline);
	m_ContourSplineFilter->SetInputData(m_ContourRepForArchCurve->GetContourRepresentationAsPolyData());
	m_ContourSplineFilter->SetSubdivideToLength();
	m_ContourSplineFilter->SetLength(0.5);
	m_ContourSplineFilter->Update();
	ui.LowerRightSlider->setMaximum(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 2);
	ui.LowerRightSpinBox->setMaximum(m_ContourSplineFilter->GetOutput()->GetNumberOfPoints() - 2);
	//隐藏右下角视图,sagital的actor
	this->SetActorsVisibilityInSagital(false);
	this->SetWindowText(m_LowerRightRenderer, "Cross View", SagitalColor);	
	//生成cross reslice
	this->GenerateCrossReslice();
	//设置panaromic slider的值和范围
	ui.LowerLeftSlider->setMinimum(-100);
	ui.LowerLeftSpinBox->setMinimum(-100);
	ui.LowerLeftSlider->setMaximum(100);
	ui.LowerLeftSpinBox->setMaximum(100);
	ui.LowerLeftSlider->setValue(0);
	ui.LowerLeftSpinBox->setValue(0);
	isPanaromic = true;
	isCross = true;
	//隐藏左下角视图，coronal的actor
	this->SetActorsVisibilityInCoronal(false);
	this->SetWindowText(m_LowerLeftRenderer, "Panaromic View", CoronalColor);
	//显示全镜面
	/*更新offset spline的位置*/
	this->GenerateOffSetSpline();
	this->GeneratePanaromicReslice();
	this->GeneratePanaromicReslice2D();
	m_LowerLeftRenderer->ResetCamera();
	m_LowerLeftRendWin->Render();
}

void DentalHelper::SetVolumeRendering()
{
	if (m_ModelRenderer->GetVolumes()->IsItemPresent(m_VolumeActor))
	{
		m_ModelRenderer->RemoveVolume(m_VolumeActor);
	}
	ui.ModelOpacitySlider->setValue(100);

	m_VolumeMapper->SetInputData(m_ImageData);
	m_VolumeMapper->AutoAdjustSampleDistancesOn();
	m_VolumeMapper->SetMaxMemoryFraction(0.90);
	m_VolumeMapper->SetBlendModeToComposite();

	m_VolumeProperty->SetInterpolationTypeToLinear();
	m_VolumeProperty->ShadeOn();
	m_VolumeProperty->SetAmbient(0.4);
	m_VolumeProperty->SetDiffuse(0.6);
	m_VolumeProperty->SetSpecular(0.2);

	
	double opacity = ui.ModelOpacitySlider->value() / 100.0;
	double min_ = ui.MinThresholdSlider->value();
	double max_ = ui.MaxThresholdSlider->value();
	double middle_ = min_ + (max_ - min_) / 2;
	m_VolumeFunction->RemoveAllPoints();
	m_VolumeFunction->AddPoint(-1024, 0);
	m_VolumeFunction->AddPoint(min_ - 1, 0);
	m_VolumeFunction->AddPoint(min_, 0.5*opacity);
	m_VolumeFunction->AddPoint(middle_, 0.7*opacity);
	m_VolumeFunction->AddPoint(max_, 0.8*opacity);
	m_VolumeFunction->AddPoint(max_ + 1, 0);
	m_VolumeProperty->SetScalarOpacity(m_VolumeFunction);

	m_VolumeColor->RemoveAllPoints();
	m_VolumeColor->AddRGBPoint(-1024, 0.00, 0.00, 0.00);
	m_VolumeColor->AddRGBPoint(min_ - 1, 0.00, 0.00, 0.00);
	m_VolumeColor->AddRGBPoint(min_, 230 / 255.0, 232 / 255.0, 82 / 255.0);
	m_VolumeColor->AddRGBPoint(middle_, 150 / 255.0, 80 / 255.0, 180 / 255.0);
	m_VolumeColor->AddRGBPoint(max_, 114 / 255.0, 115 / 255.0, 40 / 255.0);
	m_VolumeColor->AddRGBPoint(max_ + 1, 0, 0, 0);
	m_VolumeProperty->SetColor(m_VolumeColor);


	m_VolumeActor->SetMapper(m_VolumeMapper);
	m_VolumeActor->SetProperty(m_VolumeProperty);
	m_ModelRenderer->AddVolume(m_VolumeActor);
	m_ModelRenderer->ResetCamera();
	m_ModelRendWin->Render();
}


void DentalHelper::SetWindowText(vtkRenderer* ren, QString text, double* color)
{
	//删除视图当前标识
	auto propCollection = vtkSmartPointer<vtkActor2DCollection>::New();
	propCollection = ren->GetActors2D();
	propCollection->InitTraversal();
	for (int i=0;i<propCollection->GetNumberOfItems();i++)
	{
		ren->RemoveActor2D(propCollection->GetNextProp());
	}

	auto textActor = vtkSmartPointer<vtkTextActor>::New();
	textActor->SetInput(qPrintable(text));
	textActor->SetTextScaleModeToViewport();
	textActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	textActor->GetPositionCoordinate()->SetValue(0.05, 0.07);
	textActor->GetTextProperty()->SetFontFamilyToArial();
	textActor->GetTextProperty()->SetFontSize(15);
	textActor->GetTextProperty()->BoldOn();
	textActor->GetTextProperty()->ShadowOff();
	textActor->GetTextProperty()->ItalicOff();
	textActor->GetTextProperty()->SetColor(color);
	ren->AddActor2D(textActor);//添加新的标识
	ren->GetRenderWindow()->Render();
}


void DentalHelper::UpDateAxialLine()
{
	double Axial_CoronalPoint1[3];
	double Axial_CoronalPoint2[3];


	Axial_CoronalPoint1[0] = m_Spacing[0] * m_Extend[0] + m_Origin[0];
	Axial_CoronalPoint1[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Axial_CoronalPoint1[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];

	Axial_CoronalPoint2[0] = m_Spacing[0] * m_Extend[1] + m_Origin[0];
	Axial_CoronalPoint2[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Axial_CoronalPoint2[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];

	m_Axial_CoronalLine->GetPoints()->Initialize();
	m_Axial_CoronalLine->GetPoints()->InsertNextPoint(Axial_CoronalPoint1);
	m_Axial_CoronalLine->GetPoints()->InsertNextPoint(Axial_CoronalPoint2);
	m_LowerLeftRendWin->Render();

	double Axial_SagitalPoint1[3];
	double Axial_SagitalPoint2[3];

	Axial_SagitalPoint1[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Axial_SagitalPoint1[1] = m_Spacing[1] * m_Extend[2] + m_Origin[1];
	Axial_SagitalPoint1[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];

	Axial_SagitalPoint2[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Axial_SagitalPoint2[1] = m_Spacing[1] * m_Extend[3] + m_Origin[1];
	Axial_SagitalPoint2[2] = m_Spacing[2] * ui.UpRightSlider->value() + m_Origin[2];

	m_Axial_SagitalLine->GetPoints()->Initialize();
	m_Axial_SagitalLine->GetPoints()->InsertNextPoint(Axial_SagitalPoint1);
	m_Axial_SagitalLine->GetPoints()->InsertNextPoint(Axial_SagitalPoint2);

	m_LowerRightRendWin->Render();
}

void DentalHelper::UpDateCamera(vtkRenderer* ren, double* referenceNormal, double angle,double* foculPoint)
{
	double normalOfProjection[3];
	for (int i=0;i<3;i++)
	{
		normalOfProjection[i] = referenceNormal[i];
	}
	//获得相机
	auto camera = vtkSmartPointer<vtkCamera>::New();
	camera = ren->GetActiveCamera();

	double viewRight[3] = { 0,0,1 };
	double newViewRight[3];
	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->RotateWXYZ(angle, referenceNormal);
	transform->Update();
	for (int i=0;i<3;i++)
	{
		newViewRight[i] = transform->TransformDoubleNormal(viewRight)[i];
	}


	double viewUp[3];
	vtkMath::Cross(newViewRight, normalOfProjection, viewUp);
	double focusDistance, focusPoint[3];
	focusDistance=camera->GetDistance();
	camera->GetFocalPoint(focusPoint);
	if (foculPoint!=NULL)
	{
		for (int i=0;i<3;i++)
		{
			focusPoint[i] = foculPoint[i];
		}
	}
	double newPosition[3];
	for (int i=0;i<3;i++)
	{
		newPosition[i] = focusPoint[i] + focusDistance*referenceNormal[i];
	}
	camera->SetFocalPoint(focusPoint);
	camera->SetPosition(newPosition);
	camera->SetViewUp(viewUp);
	camera->OrthogonalizeViewUp();
	ren->GetRenderWindow()->Render();
}


void DentalHelper::UpDateCoronalLine()
{
	double Coronal_AxialPoint1[3];
	double Coronal_AxialPoint2[3];

	Coronal_AxialPoint1[0] = m_Spacing[0] * m_Extend[0] + m_Origin[0];
	Coronal_AxialPoint1[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_AxialPoint1[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	Coronal_AxialPoint2[0] = m_Spacing[0] * m_Extend[1] + m_Origin[0];
	Coronal_AxialPoint2[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_AxialPoint2[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	m_Coronal_AxialLine->GetPoints()->Initialize();
	m_Coronal_AxialLine->GetPoints()->InsertNextPoint(Coronal_AxialPoint1);
	m_Coronal_AxialLine->GetPoints()->InsertNextPoint(Coronal_AxialPoint2);
	m_UpRightRendWin->Render();
	


	double Coronal_SagitalPoint1[3];
	double Coronal_SagitalPoint2[3];

	Coronal_SagitalPoint1[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Coronal_SagitalPoint1[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_SagitalPoint1[2] = m_Spacing[2] * m_Extend[4] + m_Origin[2];

	Coronal_SagitalPoint2[0] = m_Spacing[0] * (m_Extend[1] + 1) + m_Origin[0];
	Coronal_SagitalPoint2[1] = m_Spacing[1] * ui.LowerLeftSlider->value() + m_Origin[1];
	Coronal_SagitalPoint2[2] = m_Spacing[2] * m_Extend[5] + m_Origin[2];

	m_Coronal_SagitalLine->GetPoints()->Initialize();
	m_Coronal_SagitalLine->GetPoints()->InsertNextPoint(Coronal_SagitalPoint1);
	m_Coronal_SagitalLine->GetPoints()->InsertNextPoint(Coronal_SagitalPoint2);
	
	m_LowerRightRendWin->Render();
}

void DentalHelper::UpDateSagitalLine()
{
	double Sagital_AxialPoint1[3];
	double Sagital_AxialPoint2[3];

	Sagital_AxialPoint1[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_AxialPoint1[1] = m_Spacing[1] * m_Extend[2] + m_Origin[1];
	Sagital_AxialPoint1[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	Sagital_AxialPoint2[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_AxialPoint2[1] = m_Spacing[1] * m_Extend[3] + m_Origin[1];
	Sagital_AxialPoint2[2] = m_Spacing[2] * (m_Extend[5] + 1) + m_Origin[2];

	m_Sagital_AxialLine->GetPoints()->Initialize();
	m_Sagital_AxialLine->GetPoints()->InsertNextPoint(Sagital_AxialPoint1);
	m_Sagital_AxialLine->GetPoints()->InsertNextPoint(Sagital_AxialPoint2);
	m_UpRightRendWin->Render();

	double Sagital_CoronalPoint1[3];
	double Sagital_CoronalPoint2[3];
	Sagital_CoronalPoint1[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_CoronalPoint1[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Sagital_CoronalPoint1[2] = m_Spacing[2] * m_Extend[4] + m_Origin[2];

	Sagital_CoronalPoint2[0] = m_Spacing[0] * ui.LowerRightSlider->value() + m_Origin[0];
	Sagital_CoronalPoint2[1] = m_Spacing[1] * (m_Extend[2] - 1) + m_Origin[1];
	Sagital_CoronalPoint2[2] = m_Spacing[2] * m_Extend[5] + m_Origin[2];

	m_Sagital_CoronalLine->GetPoints()->Initialize();
	m_Sagital_CoronalLine->GetPoints()->InsertNextPoint(Sagital_CoronalPoint1);
	m_Sagital_CoronalLine->GetPoints()->InsertNextPoint(Sagital_CoronalPoint2);

	m_LowerLeftRendWin->Render();
}







