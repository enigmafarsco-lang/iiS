// ============================================================================
// File: QGLchart.h
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for QGLchart.h.
// ============================================================================

#pragma once

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include "newType.h"
#include "QCustomSeries.h"
#include "QCustomAxis.h"

#include <QOpenGLPaintDevice>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLDebugLogger>
#include <QOpenGLDebugMessage>

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions>
#include <QOpenGLTimeMonitor>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QCursor>
#include "transform3d.h"
#include "camera3d.h"
#include <QToolTip>
#include <QtMath>

#include <QFileDialog>
#include <QTime>

#include <QThread>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class OpenGLError;
class OpenGLShaderProgram;
class QOpenGLDebugLogger;
class QOpenGLDebugMessage;

enum ZoomType
{
    None = 0x00,
    HorizantalZoom = 0x01,
    VerticalZoom = 0x02,
    RectangleZoom = 0x03,
    SingleClickZoom = 0x04
};

enum ChartType
{
    Spectrum = 0x00,
    Waterfall = 0x01,
    HorizantalWaterfall = 0x02,
    STFT = 0x03,
    ScatterPlot = 0x04
};

enum ThresholdType
{
    Rectangular = 0x00,
    Triangular = 0x01
};

enum MaskingModifyType
{
    leftEdge	 = 0x00,
    centerRegion = 0x01,
    rightEdge	 = 0x02
};

class QGLchart : public QOpenGLWidget
{
    Q_OBJECT

public:
    // Constructor: Initializes QGLchart subsystem components, default parameters, and state.
    explicit QGLchart(QWidget *parent = Q_NULLPTR);
    bool			event(QEvent *event);			// for handling balloon events

    // Executes add series routine.
    void			addSeries(QCustomSeries *inputSeries);
    // Sets series value and updates internal subsystem configuration.
    void			setSeries(QCustomSeries inputSeries);
    // Executes remove series routine.
    void			removeSeries(QCustomSeries *inputSeries);
    // Executes remove all series routine.
    void			removeAllSeries();
    // Executes series routine.
    QVector<QCustomSeries> series() const;

    // Executes add axis routine.
    void			addAxis(QCustomAxis *axis, Qt::Alignment alignment);
    // Executes remove axis routine.
    void			removeAxis(QCustomAxis *axis);
    // Executes axes routine.
    QList<QCustomAxis*> axes(Qt::Orientations orientation = Qt::Horizontal | Qt::Vertical, QCustomSeries *series = nullptr) const;
    // Executes axes routine.
    QList<QCustomAxis*> axes(Qt::Alignment alignment) const;

    QCustomAxis*	xAxis;
    QCustomAxis*	xAxis2;
    QCustomAxis*	yAxis;
    QCustomAxis*	yAxis2;

    // *** deprecated ***
    void			setAxisX(QCustomAxis *axis, QCustomSeries *series = nullptr);
    // Sets axis y value and updates internal subsystem configuration.
    void			setAxisY(QCustomAxis *axis, QCustomSeries *series = nullptr);
    QCustomAxis		*axisX(QCustomSeries *series = nullptr) const;
    QCustomAxis		*axisY(QCustomSeries *series = nullptr) const;
    // ******************

    // Executes create default axes routine.
    void createDefaultAxes();
    // Sets vertical number precision value and updates internal subsystem configuration.
    void setVerticalNumberPrecision(int);
    // Sets horizantal number precision value and updates internal subsystem configuration.
    void setHorizantalNumberPrecision(int);
    // Retrieves current vertical number precision state.
    int getVerticalNumberPrecision();
    // Retrieves current horizantal number precision state.
    int getHorizantalNumberPrecision();

    //void setTheme(QChart::ChartTheme theme);
    //QChart::ChartTheme theme() const;

    // Sets background brush value and updates internal subsystem configuration.
    void			setBackgroundBrush(const QBrush &brush);
    // Sets background pen value and updates internal subsystem configuration.
    void			setBackgroundPen(const QPen &pen);
    // Sets background visible value and updates internal subsystem configuration.
    void			setBackgroundVisible(bool visible = true);
    // Executes background brush routine.
    QBrush			backgroundBrush() const;
    // Executes background pen routine.
    QPen			backgroundPen() const;
    // Retrieves current ackground visible state.
    bool			isBackgroundVisible() const;

    // Sets drop shadow enabled value and updates internal subsystem configuration.
    void			setDropShadowEnabled(bool enabled = true);
    // Sets background roundness value and updates internal subsystem configuration.
    void			setBackgroundRoundness(qreal diameter);
    // Retrieves current rop shadow enabled state.
    bool			isDropShadowEnabled() const;
    // Executes background roundness routine.
    qreal			backgroundRoundness() const;

    // Executes zoom in routine.
    void			zoomIn(qreal factor = 2);
    // Executes zoom out routine.
    void			zoomOut(qreal factor = 2);

    // Executes zoom in routine.
    void			zoomIn(const QRectF &rect);
    // Executes zoom routine.
    void			zoom(qreal factor);
    // Executes zoom reset routine.
    void			zoomReset();
    // Retrieves current oomed state.
    bool			isZoomed();

    // Sets zoom type value and updates internal subsystem configuration.
    void			setZoomType(ZoomType);
    // Retrieves current zoom type state.
    ZoomType		getZoomType();
    bool			m_manualZoomType;
    // Executes disable manual zoom routine.
    void			disableManualZoom();
    // Retrieves current anual zoom active state.
    bool			isManualZoomActive();

    // Sets vertical upper range margin value and updates internal subsystem configuration.
    void			setVerticalUpperRangeMargin(double margin);
    // Sets vertical lower range margin value and updates internal subsystem configuration.
    void			setVerticalLowerRangeMargin(double margin);
    // Retrieves current vertical upper range margin state.
    double			getVerticalUpperRangeMargin();
    // Retrieves current vertical lower range margin state.
    double			getVerticalLowerRangeMargin();
    // Executes fine tune vertical range routine.
    void			fineTuneVerticalRange(QVector2D verticalRange);

    // Sets scatter upper range margin value and updates internal subsystem configuration.
    void			setScatterUpperRangeMargin(double margin);
    // Sets scatter lower range margin value and updates internal subsystem configuration.
    void			setScatterLowerRangeMargin(double margin);
    // Retrieves current scatter upper range margin state.
    double			getScatterUpperRangeMargin();
    // Retrieves current scatter lower range margin state.
    double			getScatterLowerRangeMargin();

    // add vertical and horizontal zoom as well

    // Executes scroll routine.
    void			scroll(qreal dx, qreal dy);

    //QLegend			*legend() const;

    // Sets contents margins value and updates internal subsystem configuration.
    void			setContentsMargins(const QMarginsF &margins);
    // Executes contents margins routine.
    QMarginsF		contentsMargins() const;

    // Executes plot area routine.
    QRectF			plotArea() const;
    // Sets plot area background brush value and updates internal subsystem configuration.
    void			setPlotAreaBackgroundBrush(const QBrush &brush);
    // Sets plot area background pen value and updates internal subsystem configuration.
    void			setPlotAreaBackgroundPen(const QPen &pen);
    // Sets plot area background visible value and updates internal subsystem configuration.
    void			setPlotAreaBackgroundVisible(bool visible = true);
    // Sets localize numbers value and updates internal subsystem configuration.
    void			setLocalizeNumbers(bool localize);
    // Sets locale value and updates internal subsystem configuration.
    void			setLocale(const QLocale &locale);
    // Executes plot area background brush routine.
    QBrush			plotAreaBackgroundBrush() const;
    // Executes plot area background pen routine.
    QPen			plotAreaBackgroundPen() const;
    // Retrieves current lot area background visible state.
    bool			isPlotAreaBackgroundVisible() const;
    // Executes localize numbers routine.
    bool			localizeNumbers() const;
    // Executes locale routine.
    QLocale			locale() const;

    // Executes map to value routine.
    QPointF			mapToValue(const QPointF &position, QCustomSeries *series = nullptr);
    // Executes map to position routine.
    QPointF			mapToPosition(const QPointF &value, QCustomSeries *series = nullptr);

    //ChartType		chartType() const;

    // Executes geometry routine.
    QRect			geometry() const;
    // Sets geometry value and updates internal subsystem configuration.
    void			setGeometry(const QRect &rect);
    QTime			time;
    quint32			m_frame;


    // Executes data in zoom area routine.
    QVector4D		dataInZoomArea();

protected:
    void			mouseMoveEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void			mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void			mouseReleaseEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    void			mouseDoubleClickEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;

    void			keyPressEvent(QKeyEvent *ev) Q_DECL_OVERRIDE;
    void			keyReleaseEvent(QKeyEvent *ev) Q_DECL_OVERRIDE;

    ///////////////////////////////////////////////////////
    /*--- Chart Title related functions and variables ---*/
    ///////////////////////////////////////////////////////

private:
    QString			m_titleText;
    QFont			m_titleFont;
    QBrush			m_titleBrush;

public:
    // Sets title text value and updates internal subsystem configuration.
    void			setTitleText(const QString &title);
    // Sets title text font value and updates internal subsystem configuration.
    void			setTitleTextFont(const QFont &font);
    // Sets title text brush value and updates internal subsystem configuration.
    void			setTitleTextBrush(const QBrush &brush);

    // Executes title text routine.
    QString			titleText() const;
    // Executes title text font routine.
    QFont			titleTextFont() const;
    // Executes title text brush routine.
    QBrush			titleTextBrush() const;

    ////////////////////////////////////////////////
    /*--- Zoom related functions and variables ---*/
    ////////////////////////////////////////////////

private:
    bool			ctrlKeyPressed;

    QPoint			m_firstMousePos;
    bool			m_mouseMoved;
    QRect			m_zoomArea;
    bool			m_zoomAreaSelected;
    QVector2D		m_ChartHorizantalRange;
    QVector2D		m_ChartVerticalRange;

    // Retrieves current chart horizantal range state.
    QVector2D		getChartHorizantalRange();
    // Retrieves current chart vertical range state.
    QVector2D		getChartVerticalRange();

    double			m_verticalUpperRangeMargin;
    double			m_verticalLowerRangeMargin;
    double			m_scatterUpperRangeMargin;
    double			m_scatterLowerRangeMargin;

    // Sets range min value value and updates internal subsystem configuration.
    QVector2D		setRangeMinValue(QVector2D range, ScaleType scaleType = ScaleType::Linear);
    // Executes determine zoom area routine.
    void			determineZoomArea(QPoint startPoint, QPoint endPoint, ZoomType zoomType);
    // Executes determine zoom area rectangle routine.
    void			determineZoomAreaRectangle(QPoint startPoint, QPoint endPoint, ZoomType zoomType);
    // Executes determine zoom type routine.
    ZoomType		determineZoomType(QPoint startPoint, QPoint endPoint);
    // Executes project point to draw region routine.
    QPoint			projectPointToDrawRegion(QPoint endPoint);
    // Executes wheel zoom data range routine.
    QVector4D		wheelZoomDataRange(QPoint mousePos, QPoint angleDelta);
    ZoomType		m_zoomType;
    QRect			m_zoomAreaRectangle;
    QVector4D		m_zoomedData;
    bool			m_verticalChart;
    bool			m_SyncedZoom;
    QVector<QVector4D>		m_previousZoomedData;

    QVector2D		m_previousHorizantalRange;
    QVector2D		m_previousVerticalRange;

    /////////////////////////////////////////////////////////
    /*--- Selected area related functions and variables ---*/
    /////////////////////////////////////////////////////////

    bool			m_areaSelectionState;
    QVector2D		m_selectedScanData;

    /////////////////////////////////////////////////////
    /*---  Masking related functions and variables  ---*/
    /////////////////////////////////////////////////////

private:
    bool				m_maskingEnabled;
    bool				m_maskingModifiable;
    MaskingModifyType	m_MaskingModifyType;
    bool				m_endOfMaskingModify;
    QColor				m_maskingColor;
    QVector<QVector2D>  m_maskingCenterPoints;

    QPointF				m_firstMaskingPressPos;
    float				m_initialMaskingLowerEdge;
    float				m_initialMaskingHigherEdge;

    // Executes check masking boundaries routine.
    bool				checkMaskingBoundaries(double diff, MaskingModifyType maskingType);
    // Executes determine half range routine.
    double				determineHalfRange();

public:
    // Executes enable masking routine.
    void				enableMasking(bool enable);
    // Sets masking modifiable value and updates internal subsystem configuration.
    void				setMaskingModifiable(bool enable);
    // Sets masking color value and updates internal subsystem configuration.
    void				setMaskingColor(const QColor &color);
    // Sets masking regions value and updates internal subsystem configuration.
    void				setMaskingRegions(QVector<QVector2D> maskingRegions);

    // Retrieves current asking enabled state.
    bool				isMaskingEnabled() const;
    // Retrieves current asking modifiable state.
    bool				isMaskingModifiable() const;
    // Executes masking color routine.
    QColor				maskingColor() const;
    // Executes masking regions routine.
    QVector<QVector2D>	maskingRegions() const;

    /////////////////////////////////////////////////////
    /*---   Marker related functions and variables  ---*/
    /////////////////////////////////////////////////////

private:
    bool				m_markerEnabled;
    QColor				m_markerColor;
    QVector<QVector2D>	m_markerCenterPoint;
    int					m_currentMarkerIndex;

    /* might later change the way marker is handled ti incorporate "inputDataIndex"
       in each marker point along its position */

    // Initializes initialize marker position for QGLchart.
    void				initializeMarkerPosition();
    // Executes info of next marker data routine.
    QVector2D			infoOfNextMarkerData(QPoint angleDelta, int inputDataIndex);
    // Executes translate data info to mouse pos routine.
    QPoint				translateDataInfoToMousePos(QVector2D dataInfo);
    // Updates and synchronizes update marker center points data stream.
    void				updateMarkerCenterPoints();

public:
    // Executes enable marker routine.
    void				enableMarker(bool enable);
    // Sets marker color value and updates internal subsystem configuration.
    void				setMarkerColor(const QColor &color);

    // Retrieves current arker enabled state.
    bool				isMarkerEnabled() const;
    // Executes marker color routine.
    QColor				markerColor() const;


    /////////////////////////////////////////////////////
    /*--- Threshold related functions and variables ---*/
    /////////////////////////////////////////////////////

private:
    QVector<QVector2D>	m_thresholdPoints;
    bool				m_thresholdEnabled;
    bool				m_thresholdModifiable;
    double				m_thresholdInitialLevel;
    int					m_modifiableSectionsNumber;
    ThresholdType		m_thresholdType;
    QColor				m_thresholdColor;
    QPointF				m_PreviousPressPos;
    int					m_selectedIndex;
    bool				m_endOfThresholdModify;

    // Initializes initialize threshold points for QGLchart.
    void				initializeThresholdPoints(ThresholdType thrType);
    // Executes translate mouse pos to data range routine.
    QPointF				translateMousePosToDataRange(QPoint mousePos);
    // Executes index of nearest thr point routine.
    int					indexOfNearestThrPoint(QPointF mousePos);

public:
    // Sets threshold points value and updates internal subsystem configuration.
    void				setThresholdPoints(QVector<QVector2D> points);
    // Sets threshold initial level value and updates internal subsystem configuration.
    void				setThresholdInitialLevel(double);
    // Sets threshold section number value and updates internal subsystem configuration.
    void				setThresholdSectionNumber(int);
    // Sets threshold type value and updates internal subsystem configuration.
    void				setThresholdType(ThresholdType);
    // Sets threshold color value and updates internal subsystem configuration.
    void				setThresholdColor(const QColor &color);
    // Sets threshold modifiable value and updates internal subsystem configuration.
    void				setThresholdModifiable(bool);
    // Executes enable threshold routine.
    void				enableThreshold(bool);

    // Executes threshold initial level routine.
    double				thresholdInitialLevel() const;
    // Executes threshold section number routine.
    int					thresholdSectionNumber() const;
    // Executes threshold type routine.
    ThresholdType		thresholdType() const;
    // Executes threshold color routine.
    QColor				thresholdColor() const;
    // Executes threshold points routine.
    QVector<QVector2D>	thresholdPoints() const;
    // Retrieves current hreshold enabled state.
    bool				isThresholdEnabled() const;
    // Retrieves current hreshold modifiable state.
    bool				isThresholdModifiable() const;

    // Sets chart type value and updates internal subsystem configuration.
    void				setChartType(ChartType chartType);
    // Executes enable synced zoom routine.
    void				enableSyncedZoom(bool);
    // Executes synced zoom routine.
    bool				syncedZoom();

    ////////////////////////////////////////////////////////////
    /*---  Label Position related functions and variables  ---*/
    ////////////////////////////////////////////////////////////

private:
    QVector2D	m_verticalLebelRelPos;
    bool		m_applyFineTuning;

    bool		m_manualHorzScalePosEnabled;
    QVector2D	m_manualHorzScalePos;

public:
    // Sets vertical label rel pos value and updates internal subsystem configuration.
    void		setVerticalLabelRelPos(QVector2D);
    // Executes vertical label rel pos routine.
    QVector2D	verticalLabelRelPos();
    // Executes enable vertical label fine tuning routine.
    void		enableVerticalLabelFineTuning(bool);

    // Sets manual horz scale rel pos value and updates internal subsystem configuration.
    void		setManualHorzScaleRelPos(QVector2D manualHorzScalePos);
    // Retrieves current manual horz scale rel pos state.
    QVector2D	getManualHorzScaleRelPos() const;

    //////////////////////////////////////////////////
    /*---  Axis related functions and variables  ---*/
    //////////////////////////////////////////////////

private:
    int				m_hNumberCount;
    QVector2D		m_ChartVerticalRangeOffset;

public:
    // Sets horizontal number count value and updates internal subsystem configuration.
    void		setHorizontalNumberCount(int hNumberCount);
    // Retrieves current horizontal number count state.
    int			getHorizontalNumberCount() const;

    // Sets vertical range offset value and updates internal subsystem configuration.
    void		setVerticalRangeOffset(QVector2D);
    // Retrieves current vertcial range offset state.
    QVector2D	getVertcialRangeOffset() const;

    /////////////////////////////////////////////////////
    /*---  ToolTip related functions and variables  ---*/
    /////////////////////////////////////////////////////

private:
    bool				m_ToolTipEnabled;
    QVector2D			m_maxAcceptableDistance;

    /* This function is not used due to mismatch between data points and pixel count */
    // Executes info of nearest data routine.
    QVector2D			infoOfNearestData(QPointF mousePosPoint, int inputDataIndex);

public:
    // Executes enable tool tip routine.
    void				enableToolTip(bool state);
    // Sets max acceptable distance value and updates internal subsystem configuration.
    void				setmaxAcceptableDistance(QVector2D distance);

    // Retrieves current ool tip enabled state.
    bool				isToolTipEnabled();
    // Executes max acceptable distance routine.
    QVector2D			maxAcceptableDistance();

private:
    ChartType		m_chartType;
    bool			changeWaterfall;
    bool			changeHorizantalWaterfall;
    int				numberOfLine;
    QTime			timeWaterfall;
    QVector<int>	m_waterfallTimeVector;

signals:
    // Executes series removed routine.
    void			seriesRemoved(QCustomSeries*);
    // Executes axis removed routine.
    void			axisRemoved(QCustomAxis*);
    // Executes plot area changed routine.
    void			plotAreaChanged(QRect*);

    // Executes scan area selected routine.
    void			scanAreaSelected(QVector2D);
    // Executes selected zoom area routine.
    void			selectedZoomArea(QPair<QVector4D, QString>);
    // Executes selected zoom out area routine.
    void			selectedZoomOutArea(QPair<QVector4D, QString>);
    // Executes selected mask area routine.
    void			selectedMaskArea(QVector2D);
    // Executes selected marker data routine.
    void			selectedMarkerData(QVector2D);
    // maybe have an update function that signals the change and have it called by other functions

public:

    // Executes setup ui routine.
    void SetupUi(QWidget *parent);

    // Retrieves current chart type state.
    ChartType getChartType() const;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

public slots:
    // Executes teardown g l routine.
    void teardownGL();
    // Updates and synchronizes update series data data stream.
    void updateSeriesData(QCustomSeries*);
    // Updates and synchronizes update series data data stream.
    void updateSeriesData(QCustomSeries);
    // Executes zoom enable routine.
    void zoomEnable(bool);
    // Executes area selection enable routine.
    void areaSelectionEnable(bool);
    // Executes message logged routine.
    void messageLogged(const QOpenGLDebugMessage &msg);
    //void newSpectrumData(QCustomSeries*);

private:
    // Executes make object routine.
    void makeObject();

    // Initializes initialize box program for QGLchart.
    void initializeBoxProgram();
    // Initializes initialize grid program for QGLchart.
    void initializeGridProgram();
    // Initializes initialize spectrum program for QGLchart.
    void initializeSpectrumProgram();
    // Initializes initialize text program for QGLchart.
    void initializeTextProgram();
    // Initializes initialize zoom area program for QGLchart.
    void initializeZoomAreaProgram();
    // Initializes initialize marker program for QGLchart.
    void initializeMarkerProgram();
    // Initializes initialize box water fall program for QGLchart.
    void initializeBoxWaterFallProgram();
    // Initializes initialize color palettes program for QGLchart.
    void initializeColorPalettesProgram();
    // Initializes initialize horizantal waterfallprogram for QGLchart.
    void initializeHorizantalWaterfallprogram();
    // Initializes initialize scatter plot program for QGLchart.
    void initializeScatterPlotProgram();
    // Initializes initialize threshold program for QGLchart.
    void initializeThresholdProgram();
    // Initializes initialize masking program for QGLchart.
    void initializeMaskingProgram();

    // Updates and synchronizes update box program data stream.
    void updateBoxProgram();
    // Updates and synchronizes update grid program data stream.
    void updateGridProgram();
    // Updates and synchronizes update spectrum program data stream.
    void updateSpectrumProgram(QCustomSeries* series);
    // Updates and synchronizes update zoom area program data stream.
    void updateZoomAreaProgram(ZoomType zoomType);
    // Updates and synchronizes update marker program data stream.
    void updateMarkerProgram();
    // Updates and synchronizes update box water fall program data stream.
    void updateBoxWaterFallProgram();
    // Updates and synchronizes update old box water fall program data stream.
    void updateOldBoxWaterFallProgram();
    // Updates and synchronizes update color palettes program data stream.
    void updateColorPalettesProgram();
    // Updates and synchronizes update spectrum colorized program data stream.
    void updateSpectrumColorizedProgram(QCustomSeries* series);
    // Updates and synchronizes update scatter plot program data stream.
    void updateScatterPlotProgram(QCustomSeries* series);
    // Updates and synchronizes update threshold program data stream.
    void updateThresholdProgram();
    // Updates and synchronizes update masking program data stream.
    void updateMaskingProgram();

    // Executes render text routine.
    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color);
    // Executes render text vertical routine.
    void RenderTextVertical(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color, Qt::Alignment alignment = Qt::AlignLeft);
    // Executes rect to vector routine.
    void rectToVector(const QRectF &rect, QVector<QVector3D> *vector);

    bool m_newSpectrumDataReceived;

    /*---Box---*/
    QOpenGLShaderProgram *m_boxProgram;
    QOpenGLBuffer m_boxBuffer;
    QVector<QVector3D> m_boxPosition;
    QOpenGLVertexArrayObject m_boxObject;

    int u_boxModelToWorld;
    int u_boxWorldToCamera;
    int u_boxCameraToView;
    int u_boxColor;

    /*---Horizantal Grid---*/
    QOpenGLBuffer       m_horizontalGridBuffer;
    QVector<QVector2D>  m_horizontalGridPosition;
    QOpenGLVertexArrayObject m_horizontalGridObject;
    QOpenGLShaderProgram *m_horizontalGridProgram;

    int u_horizontalGridModelToWorld;
    int u_horizontalGridWorldToCamera;
    int u_horizontalGridCameraToView;
    int u_horizontalGridColor;

    /*---Vertical Grid---*/
    QOpenGLBuffer       m_verticalGridBuffer;
    QVector<QVector2D>  m_verticalGridPosition;
    QOpenGLVertexArrayObject m_verticalGridObject;
    QOpenGLShaderProgram *m_verticalGridProgram;

    int u_verticalGridModelToWorld;
    int u_verticalGridWorldToCamera;
    int u_verticalGridCameraToView;
    int u_verticalGridColor;

    /*---Spectrum Program---*/
    QOpenGLBuffer       m_spectrumBuffer;
    QOpenGLVertexArrayObject m_spectrumObject;
    QOpenGLShaderProgram *m_spectrumProgram;

    int u_spectrumModelToWorld;
    int u_spectrumWorldToCamera;
    int u_spectrumCameraToView;
    int u_spectrumCameraToView2;
    int u_spectrumColor;
    int u_spectrumLineWidth;
    int u_spectrumMiterLimit;
    int u_spectrumWindowScale;

    QOpenGLBuffer       m_spectrumCfarBuffer;
    int					u_spectrumTime;

    /*---Zoom Area Program---*/
    QOpenGLBuffer       m_zoomAreaBuffer;
    QOpenGLVertexArrayObject m_zoomAreaObject;
    QVector<QVector2D> m_zoomAreaPosition;
    QOpenGLShaderProgram *m_zoomAreaProgram;

    int u_zoomAreaModelToWorld;
    int u_zoomAreaWorldToCamera;
    int u_zoomAreaCameraToView;
    int u_zoomAreaColor;

    /*---Masking Program---*/
    QOpenGLBuffer				m_maskingBuffer;
    QOpenGLBuffer				m_maskCenterBuffer;
    QOpenGLVertexArrayObject	m_maskingObject;
    QOpenGLShaderProgram		*m_maskingProgram;
    QVector<QVector2D>			m_boxMaskPosition;

    int u_maskingModelToWorld;
    int u_maskingWorldToCamera;
    int u_maskingCameraToView;
    int u_maskingColor;

    /*---Threshold Program---*/
    QOpenGLBuffer				m_thresholdBuffer;
    QOpenGLVertexArrayObject	m_thresholdObject;
    QVector<QVector2D>			m_thresholdPosition;
    QOpenGLShaderProgram		*m_thresholdProgram;

    int u_thresholdModelToWorld;
    int u_thresholdWorldToCamera;
    int u_thresholdCameraToView;
    int u_thresholdColor;

    /*---Marker Program---*/
    QOpenGLBuffer				m_markerBuffer;
    QOpenGLBuffer				m_markerCenterBuffer;
    QOpenGLVertexArrayObject	m_markerObject;
    QVector<QVector2D>			m_markerPosition;
    QOpenGLShaderProgram		*m_markerProgram;

    int u_MarkerModelToWorld;
    int u_MarkerWorldToCamera;
    int u_MarkerCameraToView;
    int u_MarkerColor;
    int u_xMarkerPlotScale;
    int u_yMarkerPlotScale;

    /*---Box Waterfall Program---*/
    QOpenGLBuffer       m_boxWaterFallBuffer;
    QVector<QVector4D>  m_boxWaterFallPosition;
    QOpenGLShaderProgram *m_boxWaterFallProgram;
    QOpenGLVertexArrayObject m_boxWaterFallObject;

    int u_boxWaterFallModelToWorld;
    int u_boxWaterFallWorldToCamera;
    int u_boxWaterFallCameraToView;

    QOpenGLFramebufferObject *m_newWaterfallFrameBuffer;
    QOpenGLFramebufferObject *m_oldWaterfallFrameBuffer;

    /*---Color Palette program---*/
    QOpenGLBuffer       m_colorPalettesBuffer;
    QVector<QVector2D>  m_colorPalettesPosition;
    QOpenGLVertexArrayObject m_colorPalettesObject;
    QOpenGLShaderProgram *m_colorPalettesProgram;

    int u_colorPalettesModelToWorld;
    int u_colorPalettesWorldToCamera;
    int u_colorPalettesCameraToView;

    /*---Scatter Plot program---*/
    QOpenGLBuffer				m_scatterBoxBuffer;
    QOpenGLBuffer				m_scatterCenterBuffer;
    QOpenGLVertexArrayObject	m_scatterPlotObject;
    QOpenGLShaderProgram		*m_scatterPlotProgram;
    QVector<QVector2D>			m_scatterBoxPosition;

    int u_scatterPlotModelToWorld;
    int u_scatterPlotWorldToCamera;
    int u_scatterPlotCameraToView;
    int u_scatterColor;
    int u_scatterPlotStretch;
    int u_xScatterPlotScale;
    int u_yScatterPlotScale;

    /*---Render Text---*/
    QOpenGLBuffer       m_textBuffer;
    QOpenGLVertexArrayObject m_textObject;
    QOpenGLShaderProgram *m_textProgram;

    int u_textModelToWorld;
    int u_textWorldToCamera;
    int u_textCameraToView;
    int u_textScale;
    int u_textColor;

    QMatrix4x4	m_projection;
    QMatrix4x4	m_projection2;
    Camera3D	m_camera;
    Camera3D	m_camera2;
    Transform3D m_transform;
    Transform3D m_transform2;
    QColor		m_boxColor;
    GLuint		framebuffer;

    // Executes draw chart title routine.
    void drawChartTitle();
    // Executes draw horizontal label routine.
    void drawHorizontalLabel();
    // Executes draw vertical label routine.
    void drawVerticalLabel();
    // Executes draw horizontal scale label routine.
    void drawHorizontalScaleLabel();
    // Executes draw vertical scale label routine.
    void drawVerticalScaleLabel();
    // Executes draw horizontal numbers routine.
    void drawHorizontalNumbers();
    // Executes draw vertical numbers routine.
    void drawVerticalNumbers();
    QVector<QPair<QString, qreal> > yAxisNumberTuner(QVector<QString>);		// right aligning and converting the numbers to scientific format
    // Computes mathematical calculate scatter scale calculation.
    float calculateScatterScale(int, float);

    /*----- Yaxis2 related functiolns -----*/
    // Executes draw yaxis2 numbers routine.
    void drawYaxis2Numbers();
    // Executes draw yaxis2 text routine.
    void drawYaxis2Text();
    // Executes draw yaxis2 label text routine.
    void drawYaxis2LabelText();

    int m_verticalNumberPrecision;
    int m_HorizantalNumberPrecision;

private:
    std::map<GLchar, Character> Characters;
    QVector<QCustomSeries*>		m_addedSeries;
    QVector<QCustomSeries>		m_addedSeriesData;

    QVector<QCustomAxis*>		m_currentChartAxis;
    QCustomAxis*				m_horizantalAxes;
    QCustomAxis*				m_verticalAxes;

    backgroundProperties		m_background;
    plotAreaProperties			m_plotAreaBackground;

    QRect						m_geometry;			// The geometry of the WIDGET
    QRect						m_drawRegion;		// The Rect containing the drawing region
    bool						m_zoomState;

    QSizeF						m_chartSize;
    QSizeF						m_preferredSize;
    QSizeF						m_maximumSize;
    QSizeF						m_minimumSize;
    QSizePolicy					m_chartSizePolicy;

    QPointF						m_position;
    //QPointF					m_geometry;

    QMarginsF					m_contentsMargin;

    //QGLchart::ChartTheme		m_theme;

    QOpenGLFunctions_3_3_Core *f;
    QOpenGLExtraFunctions *fExt;
    QOpenGLDebugLogger *m_debugLogger;

private:
    QPoint						m_viewPortBottomLeft;		// The bottom left cornet of the viewport
    QPair<int, int>				m_viewPortSize;
    // Computes mathematical calculate view port calculation.
    void						calculateViewPort(const QRect &geometry, const QMarginsF &margins);
    bool						m_verticalGridVisible;

    // Executes max series size routine.
    int							maxSeriesSize();

    // Executes min yvalue in all series routine.
    float						minYvalueInAllSeries();
    // Executes max yvalue in all series routine.
    float						maxYvalueInAllSeries();
    // Executes min xvalue in all series routine.
    float						minXvalueInAllSeries();
    // Executes max xvalue in all series routine.
    float						maxXvalueInAllSeries();

    QPair<qreal, qreal>			m_xRange;

    // Executes q color to q vector3 d routine.
    QVector3D					qColorToQVector3D(const QColor &color);
};
