#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DentalHelper.h"
#include <vtkAutoInit.h>

#include "DlgSelectDirectory.h"
#include "XDicomItem.h"
#include "qglobal.h"
#include "QProgressWidget.h"
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingOpenGL2);

class DentalHelper : public QMainWindow
{
    Q_OBJECT

public:
    DentalHelper(QWidget *parent = Q_NULLPTR);
	~DentalHelper();
private:
    Ui::DentalHelperClass ui;
	int m_CurrentIndex;//当前选中的image item
	int m_ViewState;

	QList<XDicomItem*> m_DicomList;//存DICOM的list

	double AxialColor[3], CoronalColor[3], SagitalColor[3];
	QProgressWidget* m_ProgressDialog;
	//控制显示视图的布尔量
	bool isShowUpLeftView;
	bool isShowLowerLeftView;
	bool isShowUpRightView;
	bool isShowLowerRightView;
	//控制当前视图显示的内容
	bool isAxial;
	bool isCoronal;
	bool isSagital;

	//三维模型视图
	vtkSmartPointer<vtkRenderer> m_ModelRenderer;
	vtkSmartPointer<vtkRenderWindow> m_ModelRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_ModelInteractor;
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_ModelInterStyle;

	vtkSmartPointer<vtkAxesActor> AxesActor;
	vtkSmartPointer<vtkAnnotatedCubeActor> CubeActor;
	vtkSmartPointer<vtkOrientationMarkerWidget> OrientationWidget;
	//右上角视图
	vtkSmartPointer<vtkRenderer> m_UpRightRenderer;
	vtkSmartPointer<vtkRenderWindow> m_UpRightRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_UpRightInteractor;
	vtkSmartPointer<vtkInteractorStyleImage> m_UpRightInterStyle;

	//右下角视图
	vtkSmartPointer<vtkRenderer> m_LowerRightRenderer;
	vtkSmartPointer<vtkRenderWindow> m_LowerRightRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_LowerRightInteractor;
	vtkSmartPointer<vtkInteractorStyleImage> m_LowerRightInterStyle;

	//左下角视图
	vtkSmartPointer<vtkRenderer> m_LowerLeftRenderer;
	vtkSmartPointer<vtkRenderWindow> m_LowerLeftRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_LowerLeftInteractor;
	vtkSmartPointer<vtkInteractorStyleImage> m_LowerLeftInterStyle;

	//当前处理的image data
	vtkSmartPointer<vtkImageData> m_ImageData;
	double m_Spacing[3],  m_Origin[3];
	int m_Extend[6];

	//三个传统视图的显示
	vtkSmartPointer<vtkImageViewer2> m_AxialViewer;
	vtkSmartPointer<vtkImageViewer2> m_CoronalViewer;
	vtkSmartPointer<vtkImageViewer2> m_SagitalViewer;

	vtkSmartPointer<vtkLine> m_Axial_CoronalLine;
	vtkSmartPointer<vtkLine> m_Axial_SagitalLine;
	vtkSmartPointer<vtkPolyData> m_Axial_CoronalLinePolyData;
	vtkSmartPointer<vtkPolyData> m_Axial_SagitalLinePolyData;
	vtkSmartPointer<vtkActor> m_Axial_CoronalLineActor;
	vtkSmartPointer<vtkActor> m_Axial_SagitalLineActor;

	vtkSmartPointer<vtkLine> m_Coronal_AxialLine;
	vtkSmartPointer<vtkPolyData> m_Coronal_AxialLinePolyData;
	vtkSmartPointer<vtkActor> m_Coronal_AxialLineActor;

	vtkSmartPointer<vtkLine> m_Coronal_SagitalLine;
	vtkSmartPointer<vtkPolyData> m_Coronal_SagitalLinePolyData;
	vtkSmartPointer<vtkActor> m_Coronal_SagitalLineActor;

	vtkSmartPointer<vtkLine> m_Sagital_AxialLine;
	vtkSmartPointer<vtkPolyData> m_Sagital_AxialLinePolyData;
	vtkSmartPointer<vtkActor> m_Sagital_AxialLineActor;

	vtkSmartPointer<vtkLine> m_Sagital_CoronalLine;
	vtkSmartPointer<vtkPolyData> m_Sagital_CoronalLinePolyData;
	vtkSmartPointer<vtkActor> m_Sagital_CoronalLineActor;

	//volume rendering
	vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> m_VolumeMapper;
	vtkSmartPointer<vtkPiecewiseFunction> m_VolumeFunction;
	vtkSmartPointer<vtkColorTransferFunction> m_VolumeColor;
	vtkSmartPointer<vtkVolumeProperty> m_VolumeProperty;
	vtkSmartPointer<vtkVolume> m_VolumeActor;

	//vtk-qt event connector
	vtkSmartPointer<vtkEventQtSlotConnect> m_EventQtConnector;

	//用于定义牙弓弧线的面
	vtkSmartPointer<vtkPlaneWidget> m_PlaneWidgetForArchCurve;

	//画牙弓曲线的面
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperForArchCurve;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropForArchCurve;

	//读取DICOM
	void ReadImageFile(QString dir);

	//获得随机颜色
	void GetRandomColor(double* rgb, int differ);

	//初始化视图
	void InitializeView();

	//设置视图标识
	void SetWindowText(vtkRenderer* ren, QString text, double* color);

	//设置显示传统视图
	void SetViewModel2Conventional();

	//设置显示规划视图
	void SetViewModel2Preplan();

	//隐藏或显示在传统视图中的各类actor
	void SetActorsVisibilityInAxial(bool vis);
	void SetActorsVisibilityInCoronal(bool vis);
	void SetActorsVisibilityInSagital(bool vis);

	//设置单个视图参数
	void SetSingleViewer(vtkRenderer* ren, vtkImageViewer2* viewer,int sliceIndex,int mark);

	void SetVolumeRendering();

	//中心线
	void DrawAxialLine();
	void UpDateAxialLine();
	void DrawCoronalLine();
	void UpDateCoronalLine();
	void DrawSagitalLine();
	void UpDateSagitalLine();
	//根据各个视图的可见性，设置合适的layout
	void SetSuitableLayout();
	//给按钮设定颜色
	void SetButtonColor(QPushButton* button, double* color);

	//设置相机参数，正对图像
	void UpDateCamera(vtkRenderer* ren, double* referenceNormal, double angle);
protected slots:

	//对Load Button的响应函数
	void OnReadDICOM();
	//显示进度
	void OnSetProgressBar(int);
	//点击别的item
	void OnImageTableCellClicked(int row, int columm);
	//导入视图显示图像
	void OnPreviewData();
	//更新视图
	void OnUpDateUpRightView(int);
	void OnUpDateLowerLeftView(int);
	void OnUpDateLowerRightView(int);
	//更新volume rendering
	void OnUpDateVolumeRendering();
	//控制左上角视图
	void OnUpLeftViewButton();
	//控制左下角视图
	void OnLowerLeftViewButton();
	//控制右上角视图
	void OnUpRightViewButton();
	//控制右下角视图
	void OnLowerRightViewButton();
	//删除选定image
	void OnRemoveImage();
	//改变background color1
	void OnSetBackGroundColor1();
	//改变background color2
	void OnSetBackGroundColor2();
	//开始规划
	void OnPreplanSetting();

	//planewidget交互时，更新画牙弓曲线的面
	void OnPlaneWidgetForArchCurveInteraction();
};
