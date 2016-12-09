#include "stdafx.h"
#include "DentalHelper.h"

enum ViewState
{
	conventional,

};

DentalHelper::DentalHelper(QWidget *parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);
	ui.ImageTabelWidget->setColumnWidth(1, 150);
	ui.ImageTabelWidget->setColumnWidth(2, 70);
	ui.ImageTabelWidget->setColumnWidth(3, 70);
	ui.ImageTabelWidget->setColumnWidth(4, 130);
	m_ViewState = ViewState::conventional;
	m_CurrentIndex = -1;

	isShowUpLeftView = true;
	isShowLowerLeftView = true;
	isShowUpRightView = true;
	isShowLowerRightView = true;

	isAxial = true;
	isCoronal = true;
	isSagital = true;

	m_ProgressDialog = new QProgressWidget(0);
	m_ProgressDialog->hide();
	m_ImageData = vtkSmartPointer<vtkImageData>::New();

	m_AxialViewer = vtkSmartPointer<vtkImageViewer2>::New();
	m_CoronalViewer = vtkSmartPointer<vtkImageViewer2>::New();
	m_SagitalViewer = vtkSmartPointer<vtkImageViewer2>::New();



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



	m_EventQtConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();


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

}

DentalHelper::~DentalHelper()
{
	delete m_ProgressDialog;
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
		}
		cellArrary->InsertNextCell(idlist);
		m_ContourInitialData->SetPoints(points);
		m_ContourInitialData->SetLines(cellArrary);
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

	m_LowerLeftRendWin->AddRenderer(m_LowerLeftRenderer);
	m_LowerLeftInteractor->SetRenderWindow(m_LowerLeftRendWin);
	m_LowerLeftInteractor->SetInteractorStyle(m_LowerLeftInterStyle);

	ui.LowerLeftQvtkWidget->SetRenderWindow(m_LowerLeftRendWin);
	m_LowerLeftInteractor->Initialize();
	this->SetWindowText(m_LowerLeftRenderer, "Coronal View", CoronalColor);
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

void DentalHelper::OnPlaneWidgetForArchCurveInteraction()
{
	auto planeForArchCurve = vtkSmartPointer<vtkPlane>::New();
	m_PlaneWidgetForArchCurve->GetPlane(planeForArchCurve);

	m_ResliceMapperForArchCurve->SetSlicePlane(planeForArchCurve);

	this->UpDateCamera(m_UpRightRenderer, planeForArchCurve->GetNormal(), 90);
	m_UpRightRendWin->Render();
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


void DentalHelper::OnUpDateLowerLeftView(int index)
{
	//如果是传统视图
	if (m_ViewState == ViewState::conventional)
	{
		//判断视图中是否存在图像
		if (m_LowerLeftRenderer->GetViewProps()->IsItemPresent(m_CoronalViewer->GetImageActor()))
		{
			m_CoronalViewer->SetSlice(index);
			m_CoronalViewer->Render();
			this->UpDateCoronalLine();
			m_LowerLeftRendWin->Render();
		}
	}
}


void DentalHelper::OnUpDateLowerRightView(int index)
{
	//如果是传统视图
	if (m_ViewState == ViewState::conventional)
	{
		//判断视图中是否存在图像
		if (m_LowerRightRenderer->GetViewProps()->IsItemPresent(m_SagitalViewer->GetImageActor()))
		{
			m_SagitalViewer->SetSlice(index);
			m_SagitalViewer->Render();
			this->UpDateSagitalLine();
			m_LowerRightRendWin->Render();
		}
	}
}



void DentalHelper::OnUpDateUpRightView(int index)
{

	//如果是传统视图
	if (m_ViewState==ViewState::conventional)
	{
		//判断视图中是否存在图像
		if (m_UpRightRenderer->GetViewProps()->IsItemPresent(m_AxialViewer->GetImageActor()))
		{
			m_AxialViewer->SetSlice(index);
			m_AxialViewer->Render();
			this->UpDateAxialLine();
			m_UpRightRendWin->Render();
		}
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

		foreach(QString text, item->GetMainList())
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

	

	//设置coronal视图
	int coronal_middle = (m_Extend[3] - m_Extend[2]) / 2;	
	ui.LowerLeftSlider->setMaximum(m_Extend[3] - m_Extend[2]);
	ui.LowerLeftSpinBox->setMaximum(m_Extend[3] - m_Extend[2]);
	ui.LowerLeftSlider->setValue(coronal_middle);
	this->SetSingleViewer(m_LowerLeftRenderer, m_CoronalViewer, coronal_middle, 1);

	
	//设置sagital视图
	int sagital_middle = (m_Extend[1] - m_Extend[0]) / 2;	
	ui.LowerRightSlider->setMaximum(m_Extend[1] - m_Extend[0]);
	ui.LowerRightSpinBox->setMaximum(m_Extend[1] - m_Extend[0]);
	ui.LowerRightSlider->setValue(sagital_middle);
	this->SetSingleViewer(m_LowerRightRenderer, m_SagitalViewer, sagital_middle, 2);;

	this->DrawAxialLine();
	this->DrawCoronalLine();
	this->DrawSagitalLine();
}

void DentalHelper::SetViewModel2Preplan()
{
	

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

	//隐藏左下角视图，coronal的actor
	this->SetActorsVisibilityInCoronal(false);
	this->SetWindowText(m_LowerLeftRenderer, "Panaromic View", CoronalColor);
	//显示全镜面

	this->SetActorsVisibilityInSagital(false);


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
	m_VolumeMapper->SetMinimumImageSampleDistance(0.01);
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

void DentalHelper::UpDateCamera(vtkRenderer* ren, double* referenceNormal, double angle)
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







