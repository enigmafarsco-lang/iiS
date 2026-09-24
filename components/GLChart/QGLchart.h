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
    explicit QGLchart(QWidget *parent = Q_NULLPTR);
    bool			event(QEvent *event);			// for handling balloon events

    void			addSeries(QCustomSeries *inputSeries);
    void			setSeries(QCustomSeries inputSeries);
    void			removeSeries(QCustomSeries *inputSeries);
    void			removeAllSeries();
    QVector<QCustomSeries> series() const;

    void			addAxis(QCustomAxis *axis, Qt::Alignment alignment);
    void			removeAxis(QCustomAxis *axis);
    QList<QCustomAxis*> axes(Qt::Orientations orientation = Qt::Horizontal | Qt::Vertical, QCustomSeries *series = nullptr) const;
    QList<QCustomAxis*> axes(Qt::Alignment alignment) const;

    QCustomAxis*	xAxis;
    QCustomAxis*	xAxis2;
    QCustomAxis*	yAxis;
    QCustomAxis*	yAxis2;

    // *** deprecated ***
    void			setAxisX(QCustomAxis *axis, QCustomSeries *series = nullptr);
    void			setAxisY(QCustomAxis *axis, QCustomSeries *series = nullptr);
    QCustomAxis		*axisX(QCustomSeries *series = nullptr) const;
    QCustomAxis		*axisY(QCustomSeries *series = nullptr) const;
    // ******************

    void createDefaultAxes();
    void setVerticalNumberPrecision(int);
    void setHorizantalNumberPrecision(int);
    int getVerticalNumberPrecision();
    int getHorizantalNumberPrecision();

    //void setTheme(QChart::ChartTheme theme);
    //QChart::ChartTheme theme() const;

    void			setBackgroundBrush(const QBrush &brush);
    void			setBackgroundPen(const QPen &pen);
    void			setBackgroundVisible(bool visible = true);
    QBrush			backgroundBrush() const;
    QPen			backgroundPen() const;
    bool			isBackgroundVisible() const;

    void			setDropShadowEnabled(bool enabled = true);
    void			setBackgroundRoundness(qreal diameter);
    bool			isDropShadowEnabled() const;
    qreal			backgroundRoundness() const;

    void			zoomIn(qreal factor = 2);
    void			zoomOut(qreal factor = 2);

    void			zoomIn(const QRectF &rect);
    void			zoom(qreal factor);
    void			zoomReset();
    bool			isZoomed();

    void			setZoomType(ZoomType);
    ZoomType		getZoomType();
    bool			m_manualZoomType;
    void			disableManualZoom();
    bool			isManualZoomActive();

    void			setVerticalUpperRangeMargin(double margin);
    void			setVerticalLowerRangeMargin(double margin);
    double			getVerticalUpperRangeMargin();
    double			getVerticalLowerRangeMargin();
    void			fineTuneVerticalRange(QVector2D verticalRange);

    void			setScatterUpperRangeMargin(double margin);
    void			setScatterLowerRangeMargin(double margin);
    double			getScatterUpperRangeMargin();
    double			getScatterLowerRangeMargin();

    // add vertical and horizontal zoom as well

    void			scroll(qreal dx, qreal dy);

    //QLegend			*legend() const;

    void			setContentsMargins(const QMarginsF &margins);
    QMarginsF		contentsMargins() const;

    QRectF			plotArea() const;
    void			setPlotAreaBackgroundBrush(const QBrush &brush);
    void			setPlotAreaBackgroundPen(const QPen &pen);
    void			setPlotAreaBackgroundVisible(bool visible = true);
    void			setLocalizeNumbers(bool localize);
    void			setLocale(const QLocale &locale);
    QBrush			plotAreaBackgroundBrush() const;
    QPen			plotAreaBackgroundPen() const;
    bool			isPlotAreaBackgroundVisible() const;
    bool			localizeNumbers() const;
    QLocale			locale() const;

    QPointF			mapToValue(const QPointF &position, QCustomSeries *series = nullptr);
    QPointF			mapToPosition(const QPointF &value, QCustomSeries *series = nullptr);

    //ChartType		chartType() const;

    QRect			geometry() const;
    void			setGeometry(const QRect &rect);
    QTime			time;
    quint32			m_frame;


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
    void			setTitleText(const QString &title);
    void			setTitleTextFont(const QFont &font);
    void			setTitleTextBrush(const QBrush &brush);

    QString			titleText() const;
    QFont			titleTextFont() const;
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

    QVector2D		getChartHorizantalRange();
    QVector2D		getChartVerticalRange();

    double			m_verticalUpperRangeMargin;
    double			m_verticalLowerRangeMargin;
    double			m_scatterUpperRangeMargin;
    double			m_scatterLowerRangeMargin;

    QVector2D		setRangeMinValue(QVector2D range, ScaleType scaleType = ScaleType::Linear);
    void			determineZoomArea(QPoint startPoint, QPoint endPoint, ZoomType zoomType);
    void			determineZoomAreaRectangle(QPoint startPoint, QPoint endPoint, ZoomType zoomType);
    ZoomType		determineZoomType(QPoint startPoint, QPoint endPoint);
    QPoint			projectPointToDrawRegion(QPoint endPoint);
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

    bool				checkMaskingBoundaries(double diff, MaskingModifyType maskingType);
    double				determineHalfRange();

public:
    void				enableMasking(bool enable);
    void				setMaskingModifiable(bool enable);
    void				setMaskingColor(const QColor &color);
    void				setMaskingRegions(QVector<QVector2D> maskingRegions);

    bool				isMaskingEnabled() const;
    bool				isMaskingModifiable() const;
    QColor				maskingColor() const;
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

    void				initializeMarkerPosition();
    QVector2D			infoOfNextMarkerData(QPoint angleDelta, int inputDataIndex);
    QPoint				translateDataInfoToMousePos(QVector2D dataInfo);
    void				updateMarkerCenterPoints();

public:
    void				enableMarker(bool enable);
    void				setMarkerColor(const QColor &color);

    bool				isMarkerEnabled() const;
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

    void				initializeThresholdPoints(ThresholdType thrType);
    QPointF				translateMousePosToDataRange(QPoint mousePos);
    int					indexOfNearestThrPoint(QPointF mousePos);

public:
    void				setThresholdPoints(QVector<QVector2D> points);
    void				setThresholdInitialLevel(double);
    void				setThresholdSectionNumber(int);
    void				setThresholdType(ThresholdType);
    void				setThresholdColor(const QColor &color);
    void				setThresholdModifiable(bool);
    void				enableThreshold(bool);

    double				thresholdInitialLevel() const;
    int					thresholdSectionNumber() const;
    ThresholdType		thresholdType() const;
    QColor				thresholdColor() const;
    QVector<QVector2D>	thresholdPoints() const;
    bool				isThresholdEnabled() const;
    bool				isThresholdModifiable() const;

    void				setChartType(ChartType chartType);
    void				enableSyncedZoom(bool);
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
    void		setVerticalLabelRelPos(QVector2D);
    QVector2D	verticalLabelRelPos();
    void		enableVerticalLabelFineTuning(bool);

    void		setManualHorzScaleRelPos(QVector2D manualHorzScalePos);
    QVector2D	getManualHorzScaleRelPos() const;

    //////////////////////////////////////////////////
    /*---  Axis related functions and variables  ---*/
    //////////////////////////////////////////////////

private:
    int				m_hNumberCount;
    QVector2D		m_ChartVerticalRangeOffset;

public:
    void		setHorizontalNumberCount(int hNumberCount);
    int			getHorizontalNumberCount() const;

    void		setVerticalRangeOffset(QVector2D);
    QVector2D	getVertcialRangeOffset() const;

    /////////////////////////////////////////////////////
    /*---  ToolTip related functions and variables  ---*/
    /////////////////////////////////////////////////////

private:
    bool				m_ToolTipEnabled;
    QVector2D			m_maxAcceptableDistance;

    /* This function is not used due to mismatch between data points and pixel count */
    QVector2D			infoOfNearestData(QPointF mousePosPoint, int inputDataIndex);

public:
    void				enableToolTip(bool state);
    void				setmaxAcceptableDistance(QVector2D distance);

    bool				isToolTipEnabled();
    QVector2D			maxAcceptableDistance();

private:
    ChartType		m_chartType;
    bool			changeWaterfall;
    bool			changeHorizantalWaterfall;
    int				numberOfLine;
    QTime			timeWaterfall;
    QVector<int>	m_waterfallTimeVector;

signals:
    void			seriesRemoved(QCustomSeries*);
    void			axisRemoved(QCustomAxis*);
    void			plotAreaChanged(QRect*);

    void			scanAreaSelected(QVector2D);
    void			selectedZoomArea(QPair<QVector4D, QString>);
    void			selectedZoomOutArea(QPair<QVector4D, QString>);
    void			selectedMaskArea(QVector2D);
    void			selectedMarkerData(QVector2D);
    // maybe have an update function that signals the change and have it called by other functions

public:

    void SetupUi(QWidget *parent);

    ChartType getChartType() const;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

public slots:
    void teardownGL();
    void updateSeriesData(QCustomSeries*);
    void updateSeriesData(QCustomSeries);
    void zoomEnable(bool);
    void areaSelectionEnable(bool);
    void messageLogged(const QOpenGLDebugMessage &msg);
    //void newSpectrumData(QCustomSeries*);

private:
    void makeObject();

    void initializeBoxProgram();
    void initializeGridProgram();
    void initializeSpectrumProgram();
    void initializeTextProgram();
    void initializeZoomAreaProgram();
    void initializeMarkerProgram();
    void initializeBoxWaterFallProgram();
    void initializeColorPalettesProgram();
    void initializeHorizantalWaterfallprogram();
    void initializeScatterPlotProgram();
    void initializeThresholdProgram();
    void initializeMaskingProgram();

    void updateBoxProgram();
    void updateGridProgram();
    void updateSpectrumProgram(QCustomSeries* series);
    void updateZoomAreaProgram(ZoomType zoomType);
    void updateMarkerProgram();
    void updateBoxWaterFallProgram();
    void updateOldBoxWaterFallProgram();
    void updateColorPalettesProgram();
    void updateSpectrumColorizedProgram(QCustomSeries* series);
    void updateScatterPlotProgram(QCustomSeries* series);
    void updateThresholdProgram();
    void updateMaskingProgram();

    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color);
    void RenderTextVertical(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color, Qt::Alignment alignment = Qt::AlignLeft);
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

    void drawChartTitle();
    void drawHorizontalLabel();
    void drawVerticalLabel();
    void drawHorizontalScaleLabel();
    void drawVerticalScaleLabel();
    void drawHorizontalNumbers();
    void drawVerticalNumbers();
    QVector<QPair<QString, qreal> > yAxisNumberTuner(QVector<QString>);		// right aligning and converting the numbers to scientific format
    float calculateScatterScale(int, float);

    /*----- Yaxis2 related functiolns -----*/
    void drawYaxis2Numbers();
    void drawYaxis2Text();
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
    void						calculateViewPort(const QRect &geometry, const QMarginsF &margins);
    bool						m_verticalGridVisible;

    int							maxSeriesSize();

    float						minYvalueInAllSeries();
    float						maxYvalueInAllSeries();
    float						minXvalueInAllSeries();
    float						maxXvalueInAllSeries();

    QPair<qreal, qreal>			m_xRange;

    QVector3D					qColorToQVector3D(const QColor &color);
};
