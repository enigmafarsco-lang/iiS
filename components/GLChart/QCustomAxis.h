/**
 * @file QCustomAxis.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
#include <qpen.h>

enum ScaleType
{
    Linear = 0x00,
    Logrithmic = 0x01
};

/**
 * @class QCustomAxis
 * @brief High-level controller class for QCustomAxis.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class QCustomAxis : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new QCustomAxis instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QObject).
     */
    QCustomAxis(QObject *parent = nullptr);
    /**
     * @brief Destructor for QCustomAxis, releasing allocated hardware resources, timers, and buffers.
     */
    ~QCustomAxis();

    /**
     * @brief Sets the alignment parameter and updates internal state.
     * @param alignment Input parameter specifying alignment (const Qt::Alignment).
     */
    void			setAlignment(const Qt::Alignment &alignment);
    /**
     * @brief Executes the alignment procedure for QCustomAxis.
     * @return Output value of type Qt::Alignment.
     */
    Qt::Alignment	alignment() const;
    /**
     * @brief Sets the orientation parameter and updates internal state.
     * @param orientation Input parameter specifying orientation (const Qt::Orientation).
     */
    void			setOrientation(const Qt::Orientation &orientation);
    /**
     * @brief Executes the orientation procedure for QCustomAxis.
     * @return Output value of type Qt::Orientation.
     */
    Qt::Orientation	orientation() const;

    /***----- Range related functions -----***/

    void			setMax(const double &max);
    void			setMin(const double &min);
    void			setRange(const double &min, const double &max);

    /**
     * @brief Retrieves the current max value.
     * @return Output value of type double.
     */
    double			getMax() const;
    /**
     * @brief Retrieves the current min value.
     * @return Output value of type double.
     */
    double			getMin() const;
    /**
     * @brief Retrieves the current range value.
     * @return Output value of type QPair<qreal, qreal>.
     */
    QPair<qreal, qreal> getRange() const;

    /**
     * @brief Executes the visibility procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			visibility() const;
    /**
     * @brief Sets the visible parameter and updates internal state.
     * @param on Input parameter specifying on (bool).
     */
    void			setVisible(bool on);
    /**
     * @brief Executes the range reversed procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			rangeReversed() const;
    /**
     * @brief Sets the range reversed parameter and updates internal state.
     * @param reversed Input parameter specifying reversed (bool).
     */
    void			setRangeReversed(bool reversed);
    /**
     * @brief Retrieves the current artial range set value.
     * @return Output value of type QPair<bool, bool>.
     */
    QPair<bool, bool>	isPartialRangeSet() const;

    /***----- Label related functions -----***/

    void			setLabelsAngle(const qint32 &angle);
    void			setLabelsBrush(const QBrush &brush);
    void			setLabelsColor(const QColor &color);
    void			setLabelsFont(const QFont &font);
    /**
     * @brief Sets the labels text parameter and updates internal state.
     * @param text Input parameter specifying text (const QString).
     */
    void			setLabelsText(const QString &text);
    /**
     * @brief Sets the labels visible parameter and updates internal state.
     * @param visible Input parameter specifying visible (bool).
     */
    void			setLabelsVisible(bool visible = true);

    /**
     * @brief Executes the labels angle procedure for QCustomAxis.
     * @return Output value of type qint32.
     */
    qint32			labelsAngle() const;
    /**
     * @brief Executes the labels brush procedure for QCustomAxis.
     * @return Output value of type QBrush.
     */
    QBrush			labelsBrush() const;
    /**
     * @brief Executes the labels color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor			labelsColor() const;
    /**
     * @brief Executes the labels font procedure for QCustomAxis.
     * @return Output value of type QFont.
     */
    QFont			labelsFont() const;
    /**
     * @brief Executes the labels text procedure for QCustomAxis.
     * @return Output value of type QString.
     */
    QString			labelsText() const;
    /**
     * @brief Executes the labels visible procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			labelsVisible() const;

    /***----- Line Pen Setting -----***/

    void			setLinePen(const QPen &pen);
    void			setLinePenColor(const QColor &penColor);
    void			setLineVisible(bool visible = true);

    /**
     * @brief Executes the line pen procedure for QCustomAxis.
     * @return Output value of type QPen.
     */
    QPen			linePen() const;
    /**
     * @brief Executes the line pen color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor			linePenColor() const;
    /**
     * @brief Executes the line visible procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			lineVisible() const;

    /***----- Grid related functions -----***/

    void			setGridLinePen(const QPen &pen);
    void			setGridLineColor(const QColor &color);
    void			setGridLineVisible(bool visible = true);
    void			setGridLineCount(int count);

    /**
     * @brief Executes the grid line pen procedure for QCustomAxis.
     * @return Output value of type QPen.
     */
    QPen			gridLinePen() const;
    /**
     * @brief Executes the grid line color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor			gridLineColor() const;
    /**
     * @brief Executes the grid line visible procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			gridLineVisible() const;
    /**
     * @brief Executes the grid line count procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int				gridLineCount() const;

    /**
     * @brief Sets the minor grid line pen parameter and updates internal state.
     * @param pen Input parameter specifying pen (const QPen).
     */
    void			setMinorGridLinePen(const QPen &pen);
    /**
     * @brief Sets the minor grid line color parameter and updates internal state.
     * @param color Input parameter specifying color (const QColor).
     */
    void			setMinorGridLineColor(const QColor &color);
    /**
     * @brief Sets the minor grid line visible parameter and updates internal state.
     * @param visible Input parameter specifying visible (bool).
     */
    void			setMinorGridLineVisible(bool visible = true);

    /**
     * @brief Executes the minor grid line pen procedure for QCustomAxis.
     * @return Output value of type QPen.
     */
    QPen			minorGridLinePen() const;
    /**
     * @brief Executes the minor grid line color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor			minorGridLineColor() const;
    /**
     * @brief Executes the minor grid line visible procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			minorGridLineVisible() const;

    /***----- Shades related functions -----***/

    void			setShadesBorderColor(const QColor &color);
    void			setShadesBrush(const QBrush &brush);
    void			setShadesColor(const QColor &color);
    void			setShadesPen(const QPen &Pen);
    /**
     * @brief Sets the shades visible parameter and updates internal state.
     * @param visible Input parameter specifying visible (bool).
     */
    void			setShadesVisible(bool visible = true);

    /**
     * @brief Executes the shades border color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor			shadesBorderColor() const;
    /**
     * @brief Executes the shades brush procedure for QCustomAxis.
     * @return Output value of type QBrush.
     */
    QBrush			shadesBrush() const;
    /**
     * @brief Executes the shades color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor			shadesColor() const;
    /**
     * @brief Executes the shades pen procedure for QCustomAxis.
     * @return Output value of type QPen.
     */
    QPen			shadesPen() const;
    /**
     * @brief Executes the shades visible procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool			shadesVisible() const;

    /***----- Title related functions -----***/

    void			setTitleBrush(const QBrush &brush);
    void			setTitlesFont(const QFont &font);
    void			setTitleText(const QString &text);
    void			setTitleVisible(bool visible = true); // title position might be added later

    QBrush			titleBrush() const;
    QFont			titleFont() const;
    QString			titleText() const;
    bool			titleVisible() const;

    /***--- Tick related functions -----***/

    void setTicks(bool show);
    void setTickLabels(bool show);
    void setTickLabelPadding(int padding);
    void setTickLabelFont(const QFont &font);
    /**
     * @brief Sets the tick label color parameter and updates internal state.
     * @param color Input parameter specifying color (const QColor).
     */
    void setTickLabelColor(const QColor &color);
    /**
     * @brief Sets the tick length parameter and updates internal state.
     * @param inside Input parameter specifying inside (int).
     * @param outside Input parameter specifying outside (int).
     */
    void setTickLength(int inside, int outside = 0);
    /**
     * @brief Sets the tick length in parameter and updates internal state.
     * @param inside Input parameter specifying inside (int).
     */
    void setTickLengthIn(int inside);
    /**
     * @brief Sets the tick length out parameter and updates internal state.
     * @param outside Input parameter specifying outside (int).
     */
    void setTickLengthOut(int outside);
    /**
     * @brief Sets the tick pen parameter and updates internal state.
     * @param pen Input parameter specifying pen (const QPen).
     */
    void setTickPen(const QPen &pen);

    /**
     * @brief Executes the ticks procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool ticks() const;
    /**
     * @brief Executes the ticks labels procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool ticksLabels() const;
    /**
     * @brief Executes the tick label padding procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int tickLabelPadding() const;
    /**
     * @brief Executes the tick label font procedure for QCustomAxis.
     * @return Output value of type QFont.
     */
    QFont tickLabelFont() const;
    /**
     * @brief Executes the tick label color procedure for QCustomAxis.
     * @return Output value of type QColor.
     */
    QColor tickLabelColor() const;
    /**
     * @brief Executes the tick length procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int tickLength() const;
    /**
     * @brief Executes the tick length in procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int tickLengthIn() const;
    /**
     * @brief Executes the tick length out procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int tickLengthOut() const;
    /**
     * @brief Executes the tick pen procedure for QCustomAxis.
     * @return Output value of type QPen.
     */
    QPen tickPen() const;

    /***----- Sub tick related functions -----***/

    void setSubTicks(bool show);
    void setSubTickLength(int inside, int outside = 0);
    void setSubTickLengthIn(int inside);
    void setSubTickLengthOut(int outside);
    /**
     * @brief Sets the sub tick pen parameter and updates internal state.
     * @param pen Input parameter specifying pen (const QPen).
     */
    void setSubTickPen(const QPen &pen);

    /**
     * @brief Executes the sub ticks procedure for QCustomAxis.
     * @return Output value of type bool.
     */
    bool subTicks() const;
    /**
     * @brief Executes the sub tick length procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int subTickLength() const;
    /**
     * @brief Executes the sub tick length in procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int subTickLengthIn() const;
    /**
     * @brief Executes the sub tick length out procedure for QCustomAxis.
     * @return Output value of type int.
     */
    int subTickLengthOut() const;
    /**
     * @brief Executes the sub tick pen procedure for QCustomAxis.
     * @return Output value of type QPen.
     */
    QPen subTickPen() const;

    /***----- Number precision related functions -----***/

    void setNumberPrecision(int precision);
    int numberPrecision() const;

    /***----- Scale Type related functions -----***/

    void setScaleType(ScaleType scaleType);
    ScaleType scaleType() const;

signals:
    /**
     * @brief Executes the alignment changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const Qt::Alignment).
     */
    void			alignmentChanged(const Qt::Alignment &);
    /**
     * @brief Executes the orientation changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const Qt::Orientation).
     */
    void			orientationChanged(const Qt::Orientation &);

    /**
     * @brief Executes the labels angle changed procedure for QCustomAxis.
     * @param qint32 Input parameter specifying qint32 (qint32).
     */
    void			labelsAngleChanged(qint32);
    /**
     * @brief Executes the labels brush changed procedure for QCustomAxis.
     * @param QBrush Input parameter specifying QBrush (QBrush).
     */
    void			labelsBrushChanged(QBrush);
    /**
     * @brief Executes the labels color changed procedure for QCustomAxis.
     * @param QColor Input parameter specifying QColor (QColor).
     */
    void			labelsColorChanged(QColor);
    /**
     * @brief Executes the labels font changed procedure for QCustomAxis.
     * @param QFont Input parameter specifying QFont (QFont).
     */
    void			labelsFontChanged(QFont);
    /**
     * @brief Executes the labels text changed procedure for QCustomAxis.
     * @param QString Input parameter specifying QString (QString).
     */
    void			labelsTextChanged(QString);
    /**
     * @brief Executes the labels visible changed procedure for QCustomAxis.
     * @param bool Input parameter specifying bool (bool).
     */
    void			labelsVisibleChanged(bool);

    /**
     * @brief Executes the line pen changed procedure for QCustomAxis.
     * @param QPen Input parameter specifying QPen (QPen).
     */
    void			linePenChanged(QPen);
    /**
     * @brief Executes the line pen color changed procedure for QCustomAxis.
     * @param QColor Input parameter specifying QColor (QColor).
     */
    void			linePenColorChanged(QColor);
    /**
     * @brief Executes the line visible changed procedure for QCustomAxis.
     * @param bool Input parameter specifying bool (bool).
     */
    void			lineVisibleChanged(bool);

    /**
     * @brief Executes the grid line pen changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QPen).
     */
    void			gridLinePenChanged(const QPen &);
    /**
     * @brief Executes the grid line color changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QColor).
     */
    void			gridLineColorChanged(const QColor &);
    /**
     * @brief Executes the grid line visible changed procedure for QCustomAxis.
     * @param bool Input parameter specifying bool (bool).
     */
    void			gridLineVisibleChanged(bool);

    /**
     * @brief Executes the minor grid line pen changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QPen).
     */
    void			minorGridLinePenChanged(const QPen &);
    /**
     * @brief Executes the minor grid line color changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QColor).
     */
    void			minorGridLineColorChanged(const QColor &);
    /**
     * @brief Executes the minor grid line visible changed procedure for QCustomAxis.
     * @param bool Input parameter specifying bool (bool).
     */
    void			minorGridLineVisibleChanged(bool);

    /**
     * @brief Executes the shades border color changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QColor).
     */
    void			shadesBorderColorChanged(const QColor &);
    /**
     * @brief Executes the shades brush changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QBrush).
     */
    void			shadesBrushChanged(const QBrush &);
    /**
     * @brief Executes the shades color changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QColor).
     */
    void			shadesColorChanged(const QColor &);
    /**
     * @brief Executes the shades pen changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QPen).
     */
    void			shadesPenChanged(const QPen &);
    /**
     * @brief Executes the shades visible changed procedure for QCustomAxis.
     * @param bool Input parameter specifying bool (bool).
     */
    void			shadesVisibleChanged(bool);

    /**
     * @brief Executes the title brush changed procedure for QCustomAxis.
     * @param  Input parameter specifying  (const QBrush).
     */
    void			titleBrushChanged(const QBrush &);
    /**
     * @brief Executes the title font changed procedure for QCustomAxis.
     * @param QFont Input parameter specifying QFont (QFont).
     */
    void			titleFontChanged(QFont);
    /**
     * @brief Executes the title text changed procedure for QCustomAxis.
     * @param QString Input parameter specifying QString (QString).
     */
    void			titleTextChanged(QString);
    /**
     * @brief Executes the title visible changed procedure for QCustomAxis.
     * @param bool Input parameter specifying bool (bool).
     */
    void			titleVisibleChanged(bool);

private:

    Qt::Alignment	m_alignment;
    Qt::Orientation m_orientation;

    double			m_max;
    double			m_min;
    bool			m_lowerPartialRangeSet;
    bool			m_upperPartialRangeSet;
    bool			m_axisVisibility;
    bool			m_reverseRange;

    qint32			m_labelsAngle;
    QBrush			m_labelsBrush;
    QColor			m_labelsColor;
    QFont			m_labelsFont;
    bool			m_labelsVisibility;
    QString			m_labelsText;

    QPen			m_linePen;
    QColor			m_linePenColor;
    bool			m_lineVisible;

    /***--- Grid related variables -----***/

    QPen			m_gridLinePen;
    QColor			m_gridLineColor;
    bool			m_gridLineVisible;
    int				m_gridLineCount;	// Should be synced with the tick count later

    QPen			m_minorGridLinePen;
    QColor			m_minorGridLineColor;
    bool			m_minorGridLineVisible;

    /***--- Shades related variables -----***/

    QColor			m_shadesBorderColor;
    QBrush			m_shadesBrush;
    QColor			m_shadesColor;
    QPen			m_shadesPen;
    bool			m_shadesVisible;

    QBrush			m_titleBrush;
    QFont			m_titleFont;
    QString			m_titleText;
    bool			m_titleVisible;

    int				m_numberPrecision;

    /***--- Tick related variables -----***/

    bool			m_showTicks;
    bool			m_showTickLabels;
    int				m_tickLabelPadding;
    QFont			m_tickLabelFont;
    QColor			m_tickLabelColor;
    int				m_tickLengthIn;
    int				m_tickLengthOut;
    QPen			m_tickPen;

    /***----- Sub tick related variables -----***/

    bool			m_showSubTicks;
    int				m_subTickLengthIn;
    int				m_subTickLengthOut;
    QPen			m_subTickPen;

    /***----- Scale Type related variables -----***/

    ScaleType		m_scaleType;
};
