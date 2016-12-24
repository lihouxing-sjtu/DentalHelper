#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_DentalHelper.h"
#include <vtkAutoInit.h>

#include "DlgSelectDirectory.h"
#include "XDicomItem.h"
#include "qglobal.h"
#include "QProgressWidget.h"
#include "ImplantItem.h"
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
	int m_WorkingState;
	int m_DrawImplantState;
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
	bool isPanaromic;
	bool isArchCurve;
	bool isCross;
	bool isRotation;
	bool isPosition;
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
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_LowerLeftModelStyle;
	//当前处理的image data
	vtkSmartPointer<vtkImageData> m_ImageData;
	double m_Spacing[3],  m_Origin[3];
	int m_Extend[6];

	//三个传统视图的显示
	vtkSmartPointer<vtkImageViewer2> m_AxialViewer;
	vtkSmartPointer<vtkPlane> m_AxialPlane;
	vtkSmartPointer<vtkImageViewer2> m_CoronalViewer;
	vtkSmartPointer<vtkPlane> m_CoronalPlane;
	vtkSmartPointer<vtkImageViewer2> m_SagitalViewer;
	vtkSmartPointer<vtkPlane> m_SagitalPlane;

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
	//牙弓曲线的contourwidget
	vtkSmartPointer<vtkContourWidget> m_ContourWidgetForArchCurve;
	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> m_ContourRepForArchCurve;
	vtkSmartPointer<vtkBezierContourLineInterpolator> m_ContourLineInterpolator;
	vtkSmartPointer<vtkFocalPlanePointPlacer> m_ContourFocalPlanePlacer;

	vtkSmartPointer<vtkPolyData> m_ContourInitialData;
	//cross imageslice
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperOfCrossInModel;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropOfCrossInModel;
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperOfCrossIn2D;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropOfCrossIn2D;
	//rotation imageslice
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperOfRotationIn2D;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropOfRotationIn2D;

	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperOfRotationIn3D;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropOfRotationIn3D;

	vtkSmartPointer<vtkPlane> m_RotationPlane;

	//position imageslice
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperOfPositionIn2D;
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperOfPositionIn3D;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropOfPositionIn2D;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropOfPositionIn3D;
	vtkSmartPointer<vtkPlane> m_PositionPlane;

	//保存contour widget 上的点，并进行插值
	vtkSmartPointer<vtkSplineFilter> m_ContourSplineFilter;
	//偏移的曲线及actor
	vtkSmartPointer<vtkSplineFilter> m_ContourOffSetSplineFilter;
	vtkSmartPointer<vtkActor> m_ContourOffSetActor;
	vtkSmartPointer<vtkSplineFilter> m_ContourOffSetSplineFilter2D;

	//cross plane
	vtkSmartPointer<vtkPlane> m_CrossPlane;
	//panaromic in model
	vtkSmartPointer<vtkProbeFilter> m_PanaromicProbeFilterInModel;
	vtkSmartPointer<vtkActor> m_PanaromicActorInModel;

	//panaromic in 2D
	vtkSmartPointer<vtkActor> m_PanaromicActorIn2D;
	vtkSmartPointer<vtkActor> m_PanaromicActorForDrawingNurve;


	//画左边神经
	vtkSmartPointer<vtkContourWidget> m_ContourForLeftNurve;
	vtkSmartPointer<vtkBezierContourLineInterpolator> m_ContourLineForLeftNurve;
	vtkSmartPointer<vtkPolygonalSurfacePointPlacer> m_ContourPointPlacerForLeftNurve;
	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> m_ContourRepForLeftNurve;
	vtkSmartPointer<vtkTubeFilter> m_TubeForLeftNurve;
	vtkSmartPointer<vtkActor> m_ActorForLeftNurve;


	//画右边神经
	vtkSmartPointer<vtkContourWidget> m_ContourForRightNurve;
	vtkSmartPointer<vtkBezierContourLineInterpolator> m_ContourLineForRightNurve;
	vtkSmartPointer<vtkPolygonalSurfacePointPlacer> m_ContourPointPlacerForRightNurve;
	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> m_ContourRepForRightNurve;
	vtkSmartPointer<vtkTubeFilter> m_TubeForRightNurve;
	vtkSmartPointer<vtkActor> m_ActorForRightNurve;
	//切割左边神经
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInCross;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInSagital;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInRotation;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInPosition;

	//切割右边神经
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInCross;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInSagital;
	vtkSmartPointer<vtkActor> m_CUtActorForRightNurveInRotation;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInPosition;


	//上颌假牙模型
	vtkSmartPointer<vtkPolyData> m_UpProthesisData;
	vtkSmartPointer<vtkActor> m_UpProthesisActor;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInSagital;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInCross;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInRotation;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInPosition;

	//下颌假牙模型
	vtkSmartPointer<vtkPolyData> m_DownProthesisData;
	vtkSmartPointer<vtkActor> m_DownProthesisActor;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInSagital;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInCross;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInRotation;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInPosition;

	//切换axial,arch curve和position的action
	QMenu* m_MenuForUpRight;
	QAction* m_Change2ArchCurve;
	QAction* m_Change2Axial;
	QAction* m_Change2Position;
	//切换coronal,panaromic和rotation的action
	QMenu* m_MenuForLowerLeft;
	QAction* m_Change2Panaromic;
	QAction* m_Change2Coronal;
	QAction* m_Change2Rotation;
	//切换cross和sagital的action
	QMenu* m_MenuForLowerRight;
	QAction* m_Change2Cross;
	QAction* m_Change2Sagital;

	//种植体的第一个点
	vtkSmartPointer<vtkSphereWidget> m_FirstPointWidget;

	//种植体list
	QList<ImplantItem*> m_ImplantList;
	//记录当前显示的种植体
	int m_CurrentImplant;

	//种植体交互widget
	vtkSmartPointer<vtkSphereWidget> m_FirstRotateWidget;
	vtkSmartPointer<vtkSphereWidget> m_SecondRotateWidget;
	vtkSmartPointer<vtkSphereWidget> m_MoveWidget;

	//记录move widget的前一个坐标
	double centerOfPreviousMoveWiget[3];
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

	void SetActorsVisibilityInPanaromic(bool vis);
	void SetActorsVisibilityInCross(bool vis);
	void SetActorsVisibilityInArchCurve(bool vis);

	void SetActorsVisibilityInPosition(bool vis);
	void SetActorsVisibilityInRotation(bool vis);

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
	void UpDateCamera(vtkRenderer* ren, double* referenceNormal, double angle,double* foculPoint=NULL);
	//生成contour的点
	void GenerateContourWidgetPolydata();
	//生成cross reslice
	void GenerateCrossReslice();
	//生成offset spline
	void GenerateOffSetSpline();
	//生成panaromic slice
	void GeneratePanaromicReslice();
	//生成rotation slice
	void GenerateRotationReslice();
	//生成position slice
	void GeneratePositionReslice();
	//生成直纹面
	void GenerateRuledSurface(vtkPoints* p1, vtkPoints* p2,vtkPolyData*);
	
	//生成二维panaromic slice
	void GeneratePanaromicReslice2D();
	//在cross view中切割left nurve
	void CutLeftNurveInCrossView();
	//在cross view中切割right nurve
	void CutRightNurveInCrossView();
	//在cross view中切割 up prothesis
	void CutUpProthesisInCrossView();
	//在cross view中切割down prothesis
	void CutDownProthesisInCrossView();
	//在axial view中切割left nurve
	void CutLeftNurveInAxialView();
	//在axial view中切割right nurve
	void CutRightNurveInAxialView();
	//在coronal view中切割 left nurve
	void CutLeftNurveInCoronalView();
	//在coronal view中切割right nurve
	void CutRightNurveInCoronalView();
	//在sagital view中切割left nurve
	void CutLeftNurveInSagitalView();
	//在sagital view中切割right nurve
	void CutRightNurveInSagitalView();

	//在rotation中切割左边神经
	void CutLeftNurveInRotationView();
	//在position 中切割左边神经
	void CUtLeftNurveInPositionView();

	//在rotation view中切割右边神经
	void CutRightNurveInRotationView();
	//在position view中切割右边神经
	void CutRightNurveInPositionView();



	//在axial view中切割up prothesis
	void CutUpProthesisInAxialView();


	//在coronal view中切割up prothesis
	void CutUpProthesisInCoronalView();
	//在sagital view中切割up prothesis
	void CutUpProthesisInSagitalView();
	//在rotation view中切割up prothesis
	void CutUpProthesisInRotationView();
	//在position view中切割up prothesis
	void CutUpProthesisInPositionView();
	//在axial view中切割down prothesis
	void CutDownProthesisInAxialView();
	//在coronal view中切割down prothesis
	void CutDownProthesisInCoronalView();
	//在sagital view中切割sagital prothesis
	void CutDownProthesisInSagitalView();
	//在rotation 中切割down prothesis
	void CutDownProthesisInRotationView();
	//在position view中切割down prothesis
	void CutDownProthesisInPositionView();
	//设置种植体交互widget的位置
	void SetImplantInteractionWidget(double* first, double* normal,double length);
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
	//牙弓曲线contour widget交互响应函数
	void OnContourWidgetForArchCurveInteraction();
	//plane widget的可见性
	void OnPlaneWidgetForArchCurveVisibility();
	//panaromic in model的可见性
	void OnPanaromicInModelVisibility();
	//cross view in model的可见性
	void OnCrossViewInModelVisibility();
	//初始化contour widget，如果对自动记录上一次的点不满意
	void OnDeleteLastContourNode();
	//切换交互方式
	void OnChangePanaromicInteractionStyle();
	//画左边神经
	void OnDrawLeftNurve();
	//左边神经contour的交互
	void OnContourForLeftCurveInterAction();
	//画左边神经时撤销最后一个选择的点
	void OnDeleteLastNodeForLeftNurve();
	//画右边神经
	void OnDrawRightNurve();
	//右边神经contour的交互
	void OnContourForRightCurveInterAction();
	//画右边神经时撤销最后一个选择的点
	void OnDeleteLastNodeForRightNurve();
	//up right切换到axial view
	void OnChange2AxialView();
	//up right切换到arch curve
	void OnChange2ArchCurve();
	//up right 切换到position view
	void OnChange2Position();
	//lower left切换到coronal
	void OnChange2Coronal();
	//lower left 切换到panaromic
	void OnChange2Panaromic();
	//lower left切换到rotation
	void OnChange2Rotation();
	//lower right切换到sagital
	void OnChange2Sagital();
	//lower right切换到 cross
	void OnChange2Cross();

	//添加上颌假体模型
	void OnLoadUpProthesis();
	//调整上颌假体的透明度
	void OnChangeUpProthesisOpacity(int);
	//删除上颌假体
	void OnDeleteUpProthesis();
	//上颌假体的可见性
	void OnUpProthesisVisibility();
	//添加下颌假体模型
	void OnLoadDownProthesis();
	//调整下颌假体的透明度
	void OnChangeDownProthesisOpacity(int);
	//删除下颌假体
	void OnDeleteDownProthesis();
	//下颌假体的可见性
	void OnDownProthesisVisibility();
	//开始放种植体
	void OnStartDrawImplant();
	//添加种植体
	void OnAddImplant();
	//在cross上选择种植点
	void PickImplantPoint();
	//第一个种植点交互，投影到cross平面设置投影点为其中心
	void OnFirstPointWidgetInteraction();
	//更新当前显示的种植体
	void OnUpdateCurrentImplant(int);
	//控制rotation在3D视图中的可见性
	void OnRotationVisibilityIn3DView();
	//控制position在3D视图中的可见性
	void OnPositionVisibilityIn3DView();
	//当种植体参数改变的时候，切割
	void OnImplantChanged(ImplantItem*);
	//删除种植体
	void OnDeleteImplant(ImplantItem*);
	//种植体第一个widget的交互函数
	void OnFirstRotateWidgetInteraction();
	//种植体的第二个widget的交互函数
	void OnSecondRotateWidgetInteraction();
	//种植体的移动widget交互函数
	void OnMoveWidgetInteraction();
};
