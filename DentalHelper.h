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
	int m_CurrentIndex;//��ǰѡ�е�image item
	int m_ViewState;

	QList<XDicomItem*> m_DicomList;//��DICOM��list

	double AxialColor[3], CoronalColor[3], SagitalColor[3];
	QProgressWidget* m_ProgressDialog;
	//������ʾ��ͼ�Ĳ�����
	bool isShowUpLeftView;
	bool isShowLowerLeftView;
	bool isShowUpRightView;
	bool isShowLowerRightView;
	//���Ƶ�ǰ��ͼ��ʾ������
	bool isAxial;
	bool isCoronal;
	bool isSagital;

	//��άģ����ͼ
	vtkSmartPointer<vtkRenderer> m_ModelRenderer;
	vtkSmartPointer<vtkRenderWindow> m_ModelRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_ModelInteractor;
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_ModelInterStyle;

	vtkSmartPointer<vtkAxesActor> AxesActor;
	vtkSmartPointer<vtkAnnotatedCubeActor> CubeActor;
	vtkSmartPointer<vtkOrientationMarkerWidget> OrientationWidget;
	//���Ͻ���ͼ
	vtkSmartPointer<vtkRenderer> m_UpRightRenderer;
	vtkSmartPointer<vtkRenderWindow> m_UpRightRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_UpRightInteractor;
	vtkSmartPointer<vtkInteractorStyleImage> m_UpRightInterStyle;

	//���½���ͼ
	vtkSmartPointer<vtkRenderer> m_LowerRightRenderer;
	vtkSmartPointer<vtkRenderWindow> m_LowerRightRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_LowerRightInteractor;
	vtkSmartPointer<vtkInteractorStyleImage> m_LowerRightInterStyle;

	//���½���ͼ
	vtkSmartPointer<vtkRenderer> m_LowerLeftRenderer;
	vtkSmartPointer<vtkRenderWindow> m_LowerLeftRendWin;
	vtkSmartPointer<vtkRenderWindowInteractor> m_LowerLeftInteractor;
	vtkSmartPointer<vtkInteractorStyleImage> m_LowerLeftInterStyle;
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_LowerLeftModelStyle;
	//��ǰ�����image data
	vtkSmartPointer<vtkImageData> m_ImageData;
	double m_Spacing[3],  m_Origin[3];
	int m_Extend[6];

	//������ͳ��ͼ����ʾ
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

	//���ڶ����������ߵ���
	vtkSmartPointer<vtkPlaneWidget> m_PlaneWidgetForArchCurve;

	//���������ߵ���
	vtkSmartPointer<vtkImageResliceMapper> m_ResliceMapperForArchCurve;
	vtkSmartPointer<vtkImageSlice> m_ReslicePropForArchCurve;
	//�������ߵ�contourwidget
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
	//����contour widget �ϵĵ㣬�����в�ֵ
	vtkSmartPointer<vtkSplineFilter> m_ContourSplineFilter;
	//ƫ�Ƶ����߼�actor
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


	//�������
	vtkSmartPointer<vtkContourWidget> m_ContourForLeftNurve;
	vtkSmartPointer<vtkBezierContourLineInterpolator> m_ContourLineForLeftNurve;
	vtkSmartPointer<vtkPolygonalSurfacePointPlacer> m_ContourPointPlacerForLeftNurve;
	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> m_ContourRepForLeftNurve;
	vtkSmartPointer<vtkTubeFilter> m_TubeForLeftNurve;
	vtkSmartPointer<vtkActor> m_ActorForLeftNurve;


	//���ұ���
	vtkSmartPointer<vtkContourWidget> m_ContourForRightNurve;
	vtkSmartPointer<vtkBezierContourLineInterpolator> m_ContourLineForRightNurve;
	vtkSmartPointer<vtkPolygonalSurfacePointPlacer> m_ContourPointPlacerForRightNurve;
	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> m_ContourRepForRightNurve;
	vtkSmartPointer<vtkTubeFilter> m_TubeForRightNurve;
	vtkSmartPointer<vtkActor> m_ActorForRightNurve;
	//�и������
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInCross;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForLeftNurveInSagital;
	//�и��ұ���
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInCross;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForRightNurveInSagital;


	//������ģ��
	vtkSmartPointer<vtkPolyData> m_UpProthesisData;
	vtkSmartPointer<vtkActor> m_UpProthesisActor;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForUpProthesisInSagital;


	//������ģ��
	vtkSmartPointer<vtkPolyData> m_DownProthesisData;
	vtkSmartPointer<vtkActor> m_DownProthesisActor;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInAxial;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInCoronal;
	vtkSmartPointer<vtkActor> m_CutActorForDownProthesisInSagital;

	//�л�axial��arch curve��action
	QMenu* m_MenuForUpRight;
	QAction* m_Change2ArchCurve;
	QAction* m_Change2Axial;

	//��ȡDICOM
	void ReadImageFile(QString dir);

	//��������ɫ
	void GetRandomColor(double* rgb, int differ);

	//��ʼ����ͼ
	void InitializeView();

	//������ͼ��ʶ
	void SetWindowText(vtkRenderer* ren, QString text, double* color);

	//������ʾ��ͳ��ͼ
	void SetViewModel2Conventional();

	//������ʾ�滮��ͼ
	void SetViewModel2Preplan();

	//���ػ���ʾ�ڴ�ͳ��ͼ�еĸ���actor
	void SetActorsVisibilityInAxial(bool vis);
	void SetActorsVisibilityInCoronal(bool vis);
	void SetActorsVisibilityInSagital(bool vis);

	//���õ�����ͼ����
	void SetSingleViewer(vtkRenderer* ren, vtkImageViewer2* viewer,int sliceIndex,int mark);

	void SetVolumeRendering();

	//������
	void DrawAxialLine();
	void UpDateAxialLine();
	void DrawCoronalLine();
	void UpDateCoronalLine();
	void DrawSagitalLine();
	void UpDateSagitalLine();
	//���ݸ�����ͼ�Ŀɼ��ԣ����ú��ʵ�layout
	void SetSuitableLayout();
	//����ť�趨��ɫ
	void SetButtonColor(QPushButton* button, double* color);

	//�����������������ͼ��
	void UpDateCamera(vtkRenderer* ren, double* referenceNormal, double angle);
	//����contour�ĵ�
	void GenerateContourWidgetPolydata();
	//����cross reslice
	void GenerateCrossReslice();
	//����offset spline
	void GenerateOffSetSpline();
	//����panaromic slice
	void GeneratePanaromicReslice();
	//����ֱ����
	void GenerateRuledSurface(vtkPoints* p1, vtkPoints* p2,vtkPolyData*);
	
	//���ɶ�άpanaromic slice
	void GeneratePanaromicReslice2D();
	//��cross view���и�left nurve
	void CutLeftNurveInCrossView();
	//��cross view���и�right nurve
	void CutRightNurveInCrossView();
	//��axial view���и�left nurve
	void CutLeftNurveInAxialView();
	//��axial view���и�right nurve
	void CutRightNurveInAxialView();
	//��coronal view���и� left nurve
	void CutLeftNurveInCoronalView();
	//��coronal view���и�right nurve
	void CutRightNurveInCoronalView();
	//��sagital view���и�left nurve
	void CutLeftNurveInSagitalView();
	//��sagital view���и�right nurve
	void CutRightNurveInSagitalView();




	//��axial view���и�up prothesis
	void CutUpProthesisInAxialView();


	//��coronal view���и�up prothesis
	void CutUpProthesisInCoronalView();
	//��sagital view���и�up prothesis
	void CutUpProthesisInSagitalView();
	//��axial view���и�down prothesis
	void CutDownProthesisInAxialView();
	//��coronal view���и�down prothesis
	void CutDownProthesisInCoronalView();
	//��sagital view���и�sagital prothesis
	void CutDownProthesisInSagitalView();

protected slots:

	//��Load Button����Ӧ����
	void OnReadDICOM();
	//��ʾ����
	void OnSetProgressBar(int);
	//������item
	void OnImageTableCellClicked(int row, int columm);
	//������ͼ��ʾͼ��
	void OnPreviewData();
	//������ͼ
	void OnUpDateUpRightView(int);
	void OnUpDateLowerLeftView(int);
	void OnUpDateLowerRightView(int);
	//����volume rendering
	void OnUpDateVolumeRendering();
	//�������Ͻ���ͼ
	void OnUpLeftViewButton();
	//�������½���ͼ
	void OnLowerLeftViewButton();
	//�������Ͻ���ͼ
	void OnUpRightViewButton();
	//�������½���ͼ
	void OnLowerRightViewButton();
	//ɾ��ѡ��image
	void OnRemoveImage();
	//�ı�background color1
	void OnSetBackGroundColor1();
	//�ı�background color2
	void OnSetBackGroundColor2();
	//��ʼ�滮
	void OnPreplanSetting();

	//planewidget����ʱ�����»��������ߵ���
	void OnPlaneWidgetForArchCurveInteraction();
	//��������contour widget������Ӧ����
	void OnContourWidgetForArchCurveInteraction();
	//plane widget�Ŀɼ���
	void OnPlaneWidgetForArchCurveVisibility();
	//panaromic in model�Ŀɼ���
	void OnPanaromicInModelVisibility();
	//cross view in model�Ŀɼ���
	void OnCrossViewInModelVisibility();
	//��ʼ��contour widget��������Զ���¼��һ�εĵ㲻����
	void OnDeleteLastContourNode();
	//�л�������ʽ
	void OnChangePanaromicInteractionStyle();
	//�������
	void OnDrawLeftNurve();
	//�����contour�Ľ���
	void OnContourForLeftCurveInterAction();
	//�������ʱ�������һ��ѡ��ĵ�
	void OnDeleteLastNodeForLeftNurve();
	//���ұ���
	void OnDrawRightNurve();
	//�ұ���contour�Ľ���
	void OnContourForRightCurveInterAction();
	//���ұ���ʱ�������һ��ѡ��ĵ�
	void OnDeleteLastNodeForRightNurve();
	//up right�л���axial view
	void OnChange2AxialView();
	//up right�л���arch curve
	void OnChange2ArchCurve();
	//���������ģ��
	void OnLoadUpProthesis();
	//�����������͸����
	void OnChangeUpProthesisOpacity(int);
	//ɾ��������
	void OnDeleteUpProthesis();
	//������Ŀɼ���
	void OnUpProthesisVisibility();
	//���������ģ��
	void OnLoadDownProthesis();
	//�����������͸����
	void OnChangeDownProthesisOpacity(int);
	//ɾ��������
	void OnDeleteDownProthesis();
	//������Ŀɼ���
	void OnDownProthesisVisibility();

};
