/**
 * @file QGLchart.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

/**
 * @class OpenGLError
 * @brief High-level controller class for OpenGLError.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class OpenGLError;
/**
 * @class OpenGLShaderProgram
 * @brief High-level controller class for OpenGLShaderProgram.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class OpenGLShaderProgram;
/**
 * @class QOpenGLDebugLogger
 * @brief High-level controller class for QOpenGLDebugLogger.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class QOpenGLDebugLogger;
/**
 * @class QOpenGLDebugMessage
 * @brief High-level controller class for QOpenGLDebugMessage.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
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

/**
 * @class QGLchart
 * @brief High-level controller class for QGLchart.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class QGLchart : public QOpenGLWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new QGLchart instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit QGLchart(QWidget *parent = Q_NULLPTR);
    bool			event(QEvent *event);			// for handling balloon events

    void			addSeries(QCustomSeries *inputSeries);
    void			setSeries(QCustomSeries inputSeries);
    void			removeSeries(QCustomSeries *inputSeries);
    void			removeAllSeries();
    /**
     * @brief Executes the series procedure for QGLchart.
     * @return Output value of type QVector<QCustomSeries>.
     */
    QVector<QCustomSeries> series() const;

    /**
     * @brief Executes the add axis procedure for QGLchart.
     * @param axis Input parameter specifying axis (QCustomAxis).
     * @param alignment Input parameter specifying alignment (Qt::Alignment).
     */
    void			addAxis(QCustomAxis *axis, Qt::Alignment alignment);
    /**
     * @brief Executes the remove axis procedure for QGLchart.
     * @param axis Input parameter specifying axis (QCustomAxis).
     */
    void			removeAxis(QCustomAxis *axis);
    /**
     * @brief Executes the axes procedure for QGLchart.
     * @param orientation Input parameter specifying orientation (Qt::Orientations).
     * @param series Input parameter specifying series (QCustomSeries).
     * @return Output value of type QList<QCustomAxis*>.
     */
    QList<QCustomAxis*> axes(Qt::Orientations orientation = Qt::Horizontal | Qt::Vertical, QCustomSeries *series = nullptr) const;
    /**
     * @brief Executes the axes procedure for QGLchart.
     * @param alignment Input parameter specifying alignment (Qt::Alignment).
     * @return Output value of type QList<QCustomAxis*>.
     */
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
    /**
     * @brief Retrieves the current horizantal number precision value.
     * @return Output value of type int.
     */
    int getHorizantalNumberPrecision();

    //void setTheme(QChart::ChartTheme theme);
    //QChart::ChartTheme theme() const;

    void			setBackgroundBrush(const QBrush &brush);
    void			setBackgroundPen(const QPen &pen);
    void			setBackgroundVisible(bool visible = true);
    QBrush			backgroundBrush() const;
    /**
     * @brief Executes the background pen procedure for QGLchart.
     * @return Output value of type QPen.
     */
    QPen			backgroundPen() const;
    /**
     * @brief Retrieves the current ackground visible value.
     * @return Output value of type bool.
     */
    bool			isBackgroundVisible() const;

    /**
     * @brief Sets the drop shadow enabled parameter and updates internal state.
     * @param enabled Input parameter specifying enabled (bool).
     */
    void			setDropShadowEnabled(bool enabled = true);
    /**
     * @brief Sets the background roundness parameter and updates internal state.
     * @param diameter Input parameter specifying diameter (qreal).
     */
    void			setBackgroundRoundness(qreal diameter);
    /**
     * @brief Retrieves the current rop shadow enabled value.
     * @return Output value of type bool.
     */
    bool			isDropShadowEnabled() const;
    /**
     * @brief Executes the background roundness procedure for QGLchart.
     * @return Output value of type qreal.
     */
    qreal			backgroundRoundness() const;

    /**
     * @brief Executes the zoom in procedure for QGLchart.
     * @param factor Input parameter specifying factor (qreal).
     */
    void			zoomIn(qreal factor = 2);
    /**
     * @brief Executes the zoom out procedure for QGLchart.
     * @param factor Input parameter specifying factor (qreal).
     */
    void			zoomOut(qreal factor = 2);

    /**
     * @brief Executes the zoom in procedure for QGLchart.
     * @param rect Input parameter specifying rect (const QRectF).
     */
    void			zoomIn(const QRectF &rect);
    /**
     * @brief Executes the zoom procedure for QGLchart.
     * @param factor Input parameter specifying factor (qreal).
     */
    void			zoom(qreal factor);
    /**
     * @brief Executes the zoom reset procedure for QGLchart.
     */
    void			zoomReset();
    /**
     * @brief Retrieves the current oomed value.
     * @return Output value of type bool.
     */
    bool			isZoomed();

    /**
     * @brief Sets the zoom type parameter and updates internal state.
     * @param ZoomType Input parameter specifying ZoomType (ZoomType).
     */
    void			setZoomType(ZoomType);
    /**
     * @brief Retrieves the current zoom type value.
     * @return Output value of type ZoomType.
     */
    ZoomType		getZoomType();
    bool			m_manualZoomType;
    /**
     * @brief Executes the disable manual zoom procedure for QGLchart.
     */
    void			disableManualZoom();
    /**
     * @brief Retrieves the current anual zoom active value.
     * @return Output value of type bool.
     */
    bool			isManualZoomActive();

    /**
     * @brief Sets the vertical upper range margin parameter and updates internal state.
     * @param margin Input parameter specifying margin (double).
     */
    void			setVerticalUpperRangeMargin(double margin);
    /**
     * @brief Sets the vertical lower range margin parameter and updates internal state.
     * @param margin Input parameter specifying margin (double).
     */
    void			setVerticalLowerRangeMargin(double margin);
    /**
     * @brief Retrieves the current vertical upper range margin value.
     * @return Output value of type double.
     */
    double			getVerticalUpperRangeMargin();
    /**
     * @brief Retrieves the current vertical lower range margin value.
     * @return Output value of type double.
     */
    double			getVerticalLowerRangeMargin();
    /**
     * @brief Executes the fine tune vertical range procedure for QGLchart.
     * @param verticalRange Input parameter specifying verticalRange (QVector2D).
     */
    void			fineTuneVerticalRange(QVector2D verticalRange);

    /**
     * @brief Sets the scatter upper range margin parameter and updates internal state.
     * @param margin Input parameter specifying margin (double).
     */
    void			setScatterUpperRangeMargin(double margin);
    /**
     * @brief Sets the scatter lower range margin parameter and updates internal state.
     * @param margin Input parameter specifying margin (double).
     */
    void			setScatterLowerRangeMargin(double margin);
    /**
     * @brief Retrieves the current scatter upper range margin value.
     * @return Output value of type double.
     */
    double			getScatterUpperRangeMargin();
    /**
     * @brief Retrieves the current scatter lower range margin value.
     * @return Output value of type double.
     */
    double			getScatterLowerRangeMargin();

    // add vertical and horizontal zoom as well

    void			scroll(qreal dx, qreal dy);

    //QLegend			*legend() const;

    void			setContentsMargins(const QMarginsF &margins);
    QMarginsF		contentsMargins() const;

    QRectF			plotArea() const;
    /**
     * @brief Sets the plot area background brush parameter and updates internal state.
     * @param brush Input parameter specifying brush (const QBrush).
     */
    void			setPlotAreaBackgroundBrush(const QBrush &brush);
    /**
     * @brief Sets the plot area background pen parameter and updates internal state.
     * @param pen Input parameter specifying pen (const QPen).
     */
    void			setPlotAreaBackgroundPen(const QPen &pen);
    /**
     * @brief Sets the plot area background visible parameter and updates internal state.
     * @param visible Input parameter specifying visible (bool).
     */
    void			setPlotAreaBackgroundVisible(bool visible = true);
    /**
     * @brief Sets the localize numbers parameter and updates internal state.
     * @param localize Input parameter specifying localize (bool).
     */
    void			setLocalizeNumbers(bool localize);
    /**
     * @brief Sets the locale parameter and updates internal state.
     * @param locale Input parameter specifying locale (const QLocale).
     */
    void			setLocale(const QLocale &locale);
    /**
     * @brief Executes the plot area background brush procedure for QGLchart.
     * @return Output value of type QBrush.
     */
    QBrush			plotAreaBackgroundBrush() const;
    /**
     * @brief Executes the plot area background pen procedure for QGLchart.
     * @return Output value of type QPen.
     */
    QPen			plotAreaBackgroundPen() const;
    /**
     * @brief Retrieves the current lot area background visible value.
     * @return Output value of type bool.
     */
    bool			isPlotAreaBackgroundVisible() const;
    /**
     * @brief Executes the localize numbers procedure for QGLchart.
     * @return Output value of type bool.
     */
    bool			localizeNumbers() const;
    /**
     * @brief Executes the locale procedure for QGLchart.
     * @return Output value of type QLocale.
     */
    QLocale			locale() const;

    /**
     * @brief Executes the map to value procedure for QGLchart.
     * @param position Input parameter specifying position (const QPointF).
     * @param series Input parameter specifying series (QCustomSeries).
     * @return Output value of type QPointF.
     */
    QPointF			mapToValue(const QPointF &position, QCustomSeries *series = nullptr);
    /**
     * @brief Executes the map to position procedure for QGLchart.
     * @param value Input parameter specifying value (const QPointF).
     * @param series Input parameter specifying series (QCustomSeries).
     * @return Output value of type QPointF.
     */
    QPointF			mapToPosition(const QPointF &value, QCustomSeries *series = nullptr);

    //ChartType		chartType() const;

    QRect			geometry() const;
    void			setGeometry(const QRect &rect);
    QTime			time;
    quint32			m_frame;


    /**
     * @brief Executes the data in zoom area procedure for QGLchart.
     * @return Output value of type QVector4D.
     */
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
    /**
     * @brief Sets the title text parameter and updates internal state.
     * @param title Input parameter specifying title (const QString).
     */
    void			setTitleText(const QString &title);
    /**
     * @brief Sets the title text font parameter and updates internal state.
     * @param font Input parameter specifying font (const QFont).
     */
    void			setTitleTextFont(const QFont &font);
    /**
     * @brief Sets the title text brush parameter and updates internal state.
     * @param brush Input parameter specifying brush (const QBrush).
     */
    void			setTitleTextBrush(const QBrush &brush);

    /**
     * @brief Executes the title text procedure for QGLchart.
     * @return Output value of type QString.
     */
    QString			titleText() const;
    /**
     * @brief Executes the title text font procedure for QGLchart.
     * @return Output value of type QFont.
     */
    QFont			titleTextFont() const;
    /**
     * @brief Executes the title text brush procedure for QGLchart.
     * @return Output value of type QBrush.
     */
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

    /**
     * @brief Retrieves the current chart horizantal range value.
     * @return Output value of type QVector2D.
     */
    QVector2D		getChartHorizantalRange();
    /**
     * @brief Retrieves the current chart vertical range value.
     * @return Output value of type QVector2D.
     */
    QVector2D		getChartVerticalRange();

    double			m_verticalUpperRangeMargin;
    double			m_verticalLowerRangeMargin;
    double			m_scatterUpperRangeMargin;
    double			m_scatterLowerRangeMargin;

    /**
     * @brief Sets the range min value parameter and updates internal state.
     * @param range Input parameter specifying range (QVector2D).
     * @param scaleType Input parameter specifying scaleType (ScaleType).
     * @return Output value of type QVector2D.
     */
    QVector2D		setRangeMinValue(QVector2D range, ScaleType scaleType = ScaleType::Linear);
    /**
     * @brief Executes the determine zoom area procedure for QGLchart.
     * @param startPoint Input parameter specifying startPoint (QPoint).
     * @param endPoint Input parameter specifying endPoint (QPoint).
     * @param zoomType Input parameter specifying zoomType (ZoomType).
     */
    void			determineZoomArea(QPoint startPoint, QPoint endPoint, ZoomType zoomType);
    /**
     * @brief Executes the determine zoom area rectangle procedure for QGLchart.
     * @param startPoint Input parameter specifying startPoint (QPoint).
     * @param endPoint Input parameter specifying endPoint (QPoint).
     * @param zoomType Input parameter specifying zoomType (ZoomType).
     */
    void			determineZoomAreaRectangle(QPoint startPoint, QPoint endPoint, ZoomType zoomType);
    /**
     * @brief Executes the determine zoom type procedure for QGLchart.
     * @param startPoint Input parameter specifying startPoint (QPoint).
     * @param endPoint Input parameter specifying endPoint (QPoint).
     * @return Output value of type ZoomType.
     */
    ZoomType		determineZoomType(QPoint startPoint, QPoint endPoint);
    /**
     * @brief Executes the project point to draw region procedure for QGLchart.
     * @param endPoint Input parameter specifying endPoint (QPoint).
     * @return Output value of type QPoint.
     */
    QPoint			projectPointToDrawRegion(QPoint endPoint);
    /**
     * @brief Executes the wheel zoom data range procedure for QGLchart.
     * @param mousePos Input parameter specifying mousePos (QPoint).
     * @param angleDelta Input parameter specifying angleDelta (QPoint).
     * @return Output value of type QVector4D.
     */
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

    /**
     * @brief Executes the check masking boundaries procedure for QGLchart.
     * @param diff Input parameter specifying diff (double).
     * @param maskingType Input parameter specifying maskingType (MaskingModifyType).
     * @return Output value of type bool.
     */
    bool				checkMaskingBoundaries(double diff, MaskingModifyType maskingType);
    /**
     * @brief Executes the determine half range procedure for QGLchart.
     * @return Output value of type double.
     */
    double				determineHalfRange();

public:
    /**
     * @brief Executes the enable masking procedure for QGLchart.
     * @param enable Input parameter specifying enable (bool).
     */
    void				enableMasking(bool enable);
    /**
     * @brief Sets the masking modifiable parameter and updates internal state.
     * @param enable Input parameter specifying enable (bool).
     */
    void				setMaskingModifiable(bool enable);
    /**
     * @brief Sets the masking color parameter and updates internal state.
     * @param color Input parameter specifying color (const QColor).
     */
    void				setMaskingColor(const QColor &color);
    /**
     * @brief Sets the masking regions parameter and updates internal state.
     * @param maskingRegions Input parameter specifying maskingRegions (QVector<QVector2D>).
     */
    void				setMaskingRegions(QVector<QVector2D> maskingRegions);

    /**
     * @brief Retrieves the current asking enabled value.
     * @return Output value of type bool.
     */
    bool				isMaskingEnabled() const;
    /**
     * @brief Retrieves the current asking modifiable value.
     * @return Output value of type bool.
     */
    bool				isMaskingModifiable() const;
    /**
     * @brief Executes the masking color procedure for QGLchart.
     * @return Output value of type QColor.
     */
    QColor				maskingColor() const;
    /**
     * @brief Executes the masking regions procedure for QGLchart.
     * @return Output value of type QVector<QVector2D>.
     */
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
    /**
     * @brief Executes the enable marker procedure for QGLchart.
     * @param enable Input parameter specifying enable (bool).
     */
    void				enableMarker(bool enable);
    /**
     * @brief Sets the marker color parameter and updates internal state.
     * @param color Input parameter specifying color (const QColor).
     */
    void				setMarkerColor(const QColor &color);

    /**
     * @brief Retrieves the current arker enabled value.
     * @return Output value of type bool.
     */
    bool				isMarkerEnabled() const;
    /**
     * @brief Executes the marker color procedure for QGLchart.
     * @return Output value of type QColor.
     */
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

    /**
     * @brief Initializes and configures the initialize threshold points for the QGLchart subsystem.
     * @param thrType Input parameter specifying thrType (ThresholdType).
     */
    void				initializeThresholdPoints(ThresholdType thrType);
    /**
     * @brief Executes the translate mouse pos to data range procedure for QGLchart.
     * @param mousePos Input parameter specifying mousePos (QPoint).
     * @return Output value of type QPointF.
     */
    QPointF				translateMousePosToDataRange(QPoint mousePos);
    /**
     * @brief Executes the index of nearest thr point procedure for QGLchart.
     * @param mousePos Input parameter specifying mousePos (QPointF).
     * @return Output value of type int.
     */
    int					indexOfNearestThrPoint(QPointF mousePos);

public:
    /**
     * @brief Sets the threshold points parameter and updates internal state.
     * @param points Input parameter specifying points (QVector<QVector2D>).
     */
    void				setThresholdPoints(QVector<QVector2D> points);
    /**
     * @brief Sets the threshold initial level parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void				setThresholdInitialLevel(double);
    /**
     * @brief Sets the threshold section number parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void				setThresholdSectionNumber(int);
    /**
     * @brief Sets the threshold type parameter and updates internal state.
     * @param ThresholdType Input parameter specifying ThresholdType (ThresholdType).
     */
    void				setThresholdType(ThresholdType);
    /**
     * @brief Sets the threshold color parameter and updates internal state.
     * @param color Input parameter specifying color (const QColor).
     */
    void				setThresholdColor(const QColor &color);
    /**
     * @brief Sets the threshold modifiable parameter and updates internal state.
     * @param bool Input parameter specifying bool (bool).
     */
    void				setThresholdModifiable(bool);
    /**
     * @brief Executes the enable threshold procedure for QGLchart.
     * @param bool Input parameter specifying bool (bool).
     */
    void				enableThreshold(bool);

    /**
     * @brief Executes the threshold initial level procedure for QGLchart.
     * @return Output value of type double.
     */
    double				thresholdInitialLevel() const;
    /**
     * @brief Executes the threshold section number procedure for QGLchart.
     * @return Output value of type int.
     */
    int					thresholdSectionNumber() const;
    /**
     * @brief Executes the threshold type procedure for QGLchart.
     * @return Output value of type ThresholdType.
     */
    ThresholdType		thresholdType() const;
    /**
     * @brief Executes the threshold color procedure for QGLchart.
     * @return Output value of type QColor.
     */
    QColor				thresholdColor() const;
    /**
     * @brief Executes the threshold points procedure for QGLchart.
     * @return Output value of type QVector<QVector2D>.
     */
    QVector<QVector2D>	thresholdPoints() const;
    /**
     * @brief Retrieves the current hreshold enabled value.
     * @return Output value of type bool.
     */
    bool				isThresholdEnabled() const;
    /**
     * @brief Retrieves the current hreshold modifiable value.
     * @return Output value of type bool.
     */
    bool				isThresholdModifiable() const;

    /**
     * @brief Sets the chart type parameter and updates internal state.
     * @param chartType Input parameter specifying chartType (ChartType).
     */
    void				setChartType(ChartType chartType);
    /**
     * @brief Executes the enable synced zoom procedure for QGLchart.
     * @param bool Input parameter specifying bool (bool).
     */
    void				enableSyncedZoom(bool);
    /**
     * @brief Executes the synced zoom procedure for QGLchart.
     * @return Output value of type bool.
     */
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
    /**
     * @brief Sets the vertical label rel pos parameter and updates internal state.
     * @param QVector2D Input parameter specifying QVector2D (QVector2D).
     */
    void		setVerticalLabelRelPos(QVector2D);
    /**
     * @brief Executes the vertical label rel pos procedure for QGLchart.
     * @return Output value of type QVector2D.
     */
    QVector2D	verticalLabelRelPos();
    /**
     * @brief Executes the enable vertical label fine tuning procedure for QGLchart.
     * @param bool Input parameter specifying bool (bool).
     */
    void		enableVerticalLabelFineTuning(bool);

    /**
     * @brief Sets the manual horz scale rel pos parameter and updates internal state.
     * @param manualHorzScalePos Input parameter specifying manualHorzScalePos (QVector2D).
     */
    void		setManualHorzScaleRelPos(QVector2D manualHorzScalePos);
    /**
     * @brief Retrieves the current manual horz scale rel pos value.
     * @return Output value of type QVector2D.
     */
    QVector2D	getManualHorzScaleRelPos() const;

    //////////////////////////////////////////////////
    /*---  Axis related functions and variables  ---*/
    //////////////////////////////////////////////////

private:
    int				m_hNumberCount;
    QVector2D		m_ChartVerticalRangeOffset;

public:
    /**
     * @brief Sets the horizontal number count parameter and updates internal state.
     * @param hNumberCount Input parameter specifying hNumberCount (int).
     */
    void		setHorizontalNumberCount(int hNumberCount);
    /**
     * @brief Retrieves the current horizontal number count value.
     * @return Output value of type int.
     */
    int			getHorizontalNumberCount() const;

    /**
     * @brief Sets the vertical range offset parameter and updates internal state.
     * @param QVector2D Input parameter specifying QVector2D (QVector2D).
     */
    void		setVerticalRangeOffset(QVector2D);
    /**
     * @brief Retrieves the current vertcial range offset value.
     * @return Output value of type QVector2D.
     */
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

    /**
     * @brief Retrieves the current ool tip enabled value.
     * @return Output value of type bool.
     */
    bool				isToolTipEnabled();
    /**
     * @brief Executes the max acceptable distance procedure for QGLchart.
     * @return Output value of type QVector2D.
     */
    QVector2D			maxAcceptableDistance();

private:
    ChartType		m_chartType;
    bool			changeWaterfall;
    bool			changeHorizantalWaterfall;
    int				numberOfLine;
    QTime			timeWaterfall;
    QVector<int>	m_waterfallTimeVector;

signals:
    /**
     * @brief Executes the series removed procedure for QGLchart.
     * @param QCustomSeries* Input parameter specifying QCustomSeries* (QCustomSeries*).
     */
    void			seriesRemoved(QCustomSeries*);
    /**
     * @brief Executes the axis removed procedure for QGLchart.
     * @param QCustomAxis* Input parameter specifying QCustomAxis* (QCustomAxis*).
     */
    void			axisRemoved(QCustomAxis*);
    /**
     * @brief Executes the plot area changed procedure for QGLchart.
     * @param QRect* Input parameter specifying QRect* (QRect*).
     */
    void			plotAreaChanged(QRect*);

    /**
     * @brief Executes the scan area selected procedure for QGLchart.
     * @param QVector2D Input parameter specifying QVector2D (QVector2D).
     */
    void			scanAreaSelected(QVector2D);
    /**
     * @brief Executes the selected zoom area procedure for QGLchart.
     * @param QPair<QVector4D Input parameter specifying QPair<QVector4D (QPair<QVector4D).
     * @param QString> Input parameter specifying QString> (QString>).
     */
    void			selectedZoomArea(QPair<QVector4D, QString>);
    /**
     * @brief Executes the selected zoom out area procedure for QGLchart.
     * @param QPair<QVector4D Input parameter specifying QPair<QVector4D (QPair<QVector4D).
     * @param QString> Input parameter specifying QString> (QString>).
     */
    void			selectedZoomOutArea(QPair<QVector4D, QString>);
    /**
     * @brief Executes the selected mask area procedure for QGLchart.
     * @param QVector2D Input parameter specifying QVector2D (QVector2D).
     */
    void			selectedMaskArea(QVector2D);
    /**
     * @brief Executes the selected marker data procedure for QGLchart.
     * @param QVector2D Input parameter specifying QVector2D (QVector2D).
     */
    void			selectedMarkerData(QVector2D);
    // maybe have an update function that signals the change and have it called by other functions

public:

    void SetupUi(QWidget *parent);

    /**
     * @brief Retrieves the current chart type value.
     * @return Output value of type ChartType.
     */
    ChartType getChartType() const;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

public slots:
    /**
     * @brief Executes the teardown g l procedure for QGLchart.
     */
    void teardownGL();
    /**
     * @brief Updates and synchronizes the update series data data stream and visual indicators.
     * @param QCustomSeries* Input parameter specifying QCustomSeries* (QCustomSeries*).
     */
    void updateSeriesData(QCustomSeries*);
    /**
     * @brief Updates and synchronizes the update series data data stream and visual indicators.
     * @param QCustomSeries Input parameter specifying QCustomSeries (QCustomSeries).
     */
    void updateSeriesData(QCustomSeries);
    /**
     * @brief Executes the zoom enable procedure for QGLchart.
     * @param bool Input parameter specifying bool (bool).
     */
    void zoomEnable(bool);
    /**
     * @brief Executes the area selection enable procedure for QGLchart.
     * @param bool Input parameter specifying bool (bool).
     */
    void areaSelectionEnable(bool);
    /**
     * @brief Executes the message logged procedure for QGLchart.
     * @param msg Input parameter specifying msg (const QOpenGLDebugMessage).
     */
    void messageLogged(const QOpenGLDebugMessage &msg);
    //void newSpectrumData(QCustomSeries*);

private:
    void makeObject();

    void initializeBoxProgram();
    /**
     * @brief Initializes and configures the initialize grid program for the QGLchart subsystem.
     */
    void initializeGridProgram();
    /**
     * @brief Initializes and configures the initialize spectrum program for the QGLchart subsystem.
     */
    void initializeSpectrumProgram();
    /**
     * @brief Initializes and configures the initialize text program for the QGLchart subsystem.
     */
    void initializeTextProgram();
    /**
     * @brief Initializes and configures the initialize zoom area program for the QGLchart subsystem.
     */
    void initializeZoomAreaProgram();
    /**
     * @brief Initializes and configures the initialize marker program for the QGLchart subsystem.
     */
    void initializeMarkerProgram();
    /**
     * @brief Initializes and configures the initialize box water fall program for the QGLchart subsystem.
     */
    void initializeBoxWaterFallProgram();
    /**
     * @brief Initializes and configures the initialize color palettes program for the QGLchart subsystem.
     */
    void initializeColorPalettesProgram();
    /**
     * @brief Initializes and configures the initialize horizantal waterfallprogram for the QGLchart subsystem.
     */
    void initializeHorizantalWaterfallprogram();
    /**
     * @brief Initializes and configures the initialize scatter plot program for the QGLchart subsystem.
     */
    void initializeScatterPlotProgram();
    /**
     * @brief Initializes and configures the initialize threshold program for the QGLchart subsystem.
     */
    void initializeThresholdProgram();
    /**
     * @brief Initializes and configures the initialize masking program for the QGLchart subsystem.
     */
    void initializeMaskingProgram();

    /**
     * @brief Updates and synchronizes the update box program data stream and visual indicators.
     */
    void updateBoxProgram();
    /**
     * @brief Updates and synchronizes the update grid program data stream and visual indicators.
     */
    void updateGridProgram();
    /**
     * @brief Updates and synchronizes the update spectrum program data stream and visual indicators.
     * @param series Input parameter specifying series (QCustomSeries*).
     */
    void updateSpectrumProgram(QCustomSeries* series);
    /**
     * @brief Updates and synchronizes the update zoom area program data stream and visual indicators.
     * @param zoomType Input parameter specifying zoomType (ZoomType).
     */
    void updateZoomAreaProgram(ZoomType zoomType);
    /**
     * @brief Updates and synchronizes the update marker program data stream and visual indicators.
     */
    void updateMarkerProgram();
    /**
     * @brief Updates and synchronizes the update box water fall program data stream and visual indicators.
     */
    void updateBoxWaterFallProgram();
    /**
     * @brief Updates and synchronizes the update old box water fall program data stream and visual indicators.
     */
    void updateOldBoxWaterFallProgram();
    /**
     * @brief Updates and synchronizes the update color palettes program data stream and visual indicators.
     */
    void updateColorPalettesProgram();
    /**
     * @brief Updates and synchronizes the update spectrum colorized program data stream and visual indicators.
     * @param series Input parameter specifying series (QCustomSeries*).
     */
    void updateSpectrumColorizedProgram(QCustomSeries* series);
    /**
     * @brief Updates and synchronizes the update scatter plot program data stream and visual indicators.
     * @param series Input parameter specifying series (QCustomSeries*).
     */
    void updateScatterPlotProgram(QCustomSeries* series);
    /**
     * @brief Updates and synchronizes the update threshold program data stream and visual indicators.
     */
    void updateThresholdProgram();
    /**
     * @brief Updates and synchronizes the update masking program data stream and visual indicators.
     */
    void updateMaskingProgram();

    /**
     * @brief Executes the render text procedure for QGLchart.
     * @param text Input parameter specifying text (std::string).
     * @param x Input parameter specifying x (GLfloat).
     * @param y Input parameter specifying y (GLfloat).
     * @param scale Input parameter specifying scale (GLfloat).
     * @param color Input parameter specifying color (QVector3D).
     */
    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color);
    /**
     * @brief Executes the render text vertical procedure for QGLchart.
     * @param text Input parameter specifying text (std::string).
     * @param x Input parameter specifying x (GLfloat).
     * @param y Input parameter specifying y (GLfloat).
     * @param scale Input parameter specifying scale (GLfloat).
     * @param color Input parameter specifying color (QVector3D).
     * @param alignment Input parameter specifying alignment (Qt::Alignment).
     */
    void RenderTextVertical(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color, Qt::Alignment alignment = Qt::AlignLeft);
    /**
     * @brief Executes the rect to vector procedure for QGLchart.
     * @param rect Input parameter specifying rect (const QRectF).
     * @param vector Input parameter specifying vector (QVector<QVector3D>).
     */
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

    /**
     * @brief Executes the draw chart title procedure for QGLchart.
     */
    void drawChartTitle();
    /**
     * @brief Executes the draw horizontal label procedure for QGLchart.
     */
    void drawHorizontalLabel();
    /**
     * @brief Executes the draw vertical label procedure for QGLchart.
     */
    void drawVerticalLabel();
    /**
     * @brief Executes the draw horizontal scale label procedure for QGLchart.
     */
    void drawHorizontalScaleLabel();
    /**
     * @brief Executes the draw vertical scale label procedure for QGLchart.
     */
    void drawVerticalScaleLabel();
    /**
     * @brief Executes the draw horizontal numbers procedure for QGLchart.
     */
    void drawHorizontalNumbers();
    /**
     * @brief Executes the draw vertical numbers procedure for QGLchart.
     */
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

    /**
     * @brief Executes the min yvalue in all series procedure for QGLchart.
     * @return Output value of type float.
     */
    float						minYvalueInAllSeries();
    /**
     * @brief Executes the max yvalue in all series procedure for QGLchart.
     * @return Output value of type float.
     */
    float						maxYvalueInAllSeries();
    /**
     * @brief Executes the min xvalue in all series procedure for QGLchart.
     * @return Output value of type float.
     */
    float						minXvalueInAllSeries();
    /**
     * @brief Executes the max xvalue in all series procedure for QGLchart.
     * @return Output value of type float.
     */
    float						maxXvalueInAllSeries();

    QPair<qreal, qreal>			m_xRange;

    /**
     * @brief Executes the q color to q vector3 d procedure for QGLchart.
     * @param color Input parameter specifying color (const QColor).
     * @return Output value of type QVector3D.
     */
    QVector3D					qColorToQVector3D(const QColor &color);
};
