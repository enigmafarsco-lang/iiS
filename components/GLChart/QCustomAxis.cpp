/**
 * @file QCustomAxis.cpp
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "QCustomAxis.h"

/**
 * @brief Constructs a new QCustomAxis instance and initializes subsystem components and UI state.
 * @param parent Input parameter specifying parent (QObject).
 */
QCustomAxis::QCustomAxis(QObject *parent)
    : QObject(parent)
{
    m_titleBrush.setColor(Qt::red);
    m_gridLineVisible = true;
    m_showTicks = true;
    m_showSubTicks = true;
    m_reverseRange = false;

    m_numberPrecision = 2;
    m_tickLabelPadding = 3;
    m_gridLineCount = 16;

    m_scaleType = ScaleType::Linear;

    setRange(0, 0);
    m_lowerPartialRangeSet = false;
    m_upperPartialRangeSet = false;
}

/**
 * @brief Destructor for QCustomAxis, releasing allocated hardware resources, timers, and buffers.
 */
QCustomAxis::~QCustomAxis()
{
}

/**
 * @brief Sets the alignment parameter and updates internal state.
 * @param alignment Input parameter specifying alignment (const Qt::Alignment).
 */
void QCustomAxis::setAlignment(const Qt::Alignment &alignment)
{
    if (m_alignment != alignment)
    {
        m_alignment = alignment;
        alignmentChanged(alignment);
    }
}

/**
 * @brief Executes the alignment procedure for QCustomAxis.
 * @return Output value of type Qt::Alignment.
 */
Qt::Alignment QCustomAxis::alignment() const
{
    return m_alignment;
}

/**
 * @brief Sets the orientation parameter and updates internal state.
 * @param orientation Input parameter specifying orientation (const Qt::Orientation).
 */
void QCustomAxis::setOrientation(const Qt::Orientation &orientation)
{
    if (m_orientation != orientation)
    {
        m_orientation = orientation;
        orientationChanged(orientation);
    }
}

/**
 * @brief Executes the orientation procedure for QCustomAxis.
 * @return Output value of type Qt::Orientation.
 */
Qt::Orientation QCustomAxis::orientation() const
{
    return m_orientation;
}

/**
 * @brief Sets the max parameter and updates internal state.
 * @param max Input parameter specifying max (const double).
 */
void QCustomAxis::setMax(const double &max)
{
    m_upperPartialRangeSet = true;
    if (m_max != max)
    {
        m_max = max;
    }
}

/**
 * @brief Sets the min parameter and updates internal state.
 * @param min Input parameter specifying min (const double).
 */
void QCustomAxis::setMin(const double &min)
{
    m_lowerPartialRangeSet = true;
    if (m_min != min)
    {
        m_min = min;
    }
}

/**
 * @brief Executes the visibility procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::visibility() const
{
    return m_axisVisibility;
}

/**
 * @brief Sets the visible parameter and updates internal state.
 * @param on Input parameter specifying on (bool).
 */
void QCustomAxis::setVisible(bool on)
{
    m_axisVisibility = on;
}

/**
 * @brief Executes the range reversed procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::rangeReversed() const
{
    return m_reverseRange;
}

/**
 * @brief Sets the range reversed parameter and updates internal state.
 * @param reversed Input parameter specifying reversed (bool).
 */
void QCustomAxis::setRangeReversed(bool reversed)
{
    m_reverseRange = reversed;
}

/**
 * @brief Sets the range parameter and updates internal state.
 * @param min Input parameter specifying min (const double).
 * @param max Input parameter specifying max (const double).
 */
void QCustomAxis::setRange(const double &min, const double &max)
{
    setMin(min);
    setMax(max);
    m_lowerPartialRangeSet = false;
    m_upperPartialRangeSet = false;
}

/**
 * @brief Retrieves the current artial range set value.
 * @return Output value of type QPair<bool, bool>.
 */
QPair<bool, bool> QCustomAxis::isPartialRangeSet() const
{
    return QPair<bool, bool>(m_lowerPartialRangeSet, m_upperPartialRangeSet);
}

/**
 * @brief Retrieves the current max value.
 * @return Output value of type double.
 */
double QCustomAxis::getMax() const
{
    return m_max;
}

/**
 * @brief Retrieves the current min value.
 * @return Output value of type double.
 */
double QCustomAxis::getMin() const
{
    return m_min;
}

/**
 * @brief Retrieves the current range value.
 * @return Output value of type QPair<qreal, qreal>.
 */
QPair<qreal, qreal> QCustomAxis::getRange() const
{
    QPair<qreal, qreal> range(m_min, m_max);
    return range;
}

/**
 * @brief Sets the labels angle parameter and updates internal state.
 * @param angle Input parameter specifying angle (const qint32).
 */
void QCustomAxis::setLabelsAngle(const qint32 &angle)
{
    if (m_labelsAngle != angle)
    {
        m_labelsAngle = angle;
        labelsAngleChanged(angle);
    }
}

/**
 * @brief Sets the labels brush parameter and updates internal state.
 * @param brush Input parameter specifying brush (const QBrush).
 */
void QCustomAxis::setLabelsBrush(const QBrush &brush)
{
    if (m_labelsBrush != brush)
    {
        m_labelsBrush = brush;
        labelsBrushChanged(brush);
    }
}

/**
 * @brief Sets the labels color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setLabelsColor(const QColor &color)
{
    if (m_labelsColor != color)
    {
        m_labelsColor = color;
        labelsColorChanged(color);
    }
}

/**
 * @brief Sets the labels font parameter and updates internal state.
 * @param font Input parameter specifying font (const QFont).
 */
void QCustomAxis::setLabelsFont(const QFont &font)
{
    if (m_labelsFont != font)
    {
        m_labelsFont = font;
        labelsFontChanged(font);
    }
}

/**
 * @brief Sets the labels text parameter and updates internal state.
 * @param text Input parameter specifying text (const QString).
 */
void QCustomAxis::setLabelsText(const QString &text)
{
    if (m_labelsText != text)
    {
        m_labelsText = text;
        labelsTextChanged(text);
    }
}

/**
 * @brief Sets the labels visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomAxis::setLabelsVisible(bool visible)
{
    if (m_labelsVisibility != visible)
    {
        m_labelsVisibility = visible;
        labelsVisibleChanged(visible);
    }
}

/**
 * @brief Executes the labels angle procedure for QCustomAxis.
 * @return Output value of type qint32.
 */
qint32 QCustomAxis::labelsAngle() const
{
    return m_labelsAngle;
}

/**
 * @brief Executes the labels brush procedure for QCustomAxis.
 * @return Output value of type QBrush.
 */
QBrush QCustomAxis::labelsBrush() const
{
    return m_labelsBrush;
}

/**
 * @brief Executes the labels color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::labelsColor() const
{
    return m_labelsColor;
}

/**
 * @brief Executes the labels font procedure for QCustomAxis.
 * @return Output value of type QFont.
 */
QFont QCustomAxis::labelsFont() const
{
    return m_labelsFont;
}

/**
 * @brief Executes the labels text procedure for QCustomAxis.
 * @return Output value of type QString.
 */
QString QCustomAxis::labelsText() const
{
    return m_labelsText;
}

/**
 * @brief Executes the labels visible procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::labelsVisible() const
{
    return m_labelsVisibility;
}

/**
 * @brief Sets the line pen parameter and updates internal state.
 * @param pen Input parameter specifying pen (const QPen).
 */
void QCustomAxis::setLinePen(const QPen &pen)
{
    if (m_linePen != pen)
    {
        m_linePen = pen;
        linePenChanged(pen);
    }
}

/**
 * @brief Sets the line pen color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setLinePenColor(const QColor &color)
{
    if (m_linePenColor != color)
    {
        m_linePenColor = color;
        linePenColorChanged(color);
    }
}

/**
 * @brief Sets the line visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomAxis::setLineVisible(bool visible)
{
    if (m_lineVisible != visible)
    {
        m_lineVisible = visible;
        lineVisibleChanged(visible);
    }
}

/**
 * @brief Executes the line pen procedure for QCustomAxis.
 * @return Output value of type QPen.
 */
QPen QCustomAxis::linePen() const
{
    return m_linePen;
}

/**
 * @brief Executes the line pen color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::linePenColor() const
{
    return m_linePenColor;
}

/**
 * @brief Executes the line visible procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::lineVisible() const
{
    return m_lineVisible;
}

/**
 * @brief Sets the shades border color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setShadesBorderColor(const QColor &color)
{
    if (m_shadesBorderColor != color)
    {
        m_shadesBorderColor = color;
        shadesBorderColorChanged(color);
    }
}

/**
 * @brief Sets the shades brush parameter and updates internal state.
 * @param brush Input parameter specifying brush (const QBrush).
 */
void QCustomAxis::setShadesBrush(const QBrush &brush)
{
    if (m_shadesBrush != brush)
    {
        m_shadesBrush = brush;
        shadesBrushChanged(brush);
    }
}

/**
 * @brief Sets the shades color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setShadesColor(const QColor &color)
{
    if (m_shadesColor != color)
    {
        m_shadesColor = color;
        shadesColorChanged(color);
    }
}

/**
 * @brief Sets the shades pen parameter and updates internal state.
 * @param pen Input parameter specifying pen (const QPen).
 */
void QCustomAxis::setShadesPen(const QPen &pen)
{
    if (m_shadesPen != pen)
    {
        m_shadesPen = pen;
        shadesPenChanged(pen);
    }
}

/**
 * @brief Sets the shades visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomAxis::setShadesVisible(bool visible)
{
    if (m_shadesVisible != visible)
    {
        m_shadesVisible = visible;
        shadesVisibleChanged(visible);
    }
}

/**
 * @brief Executes the shades border color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::shadesBorderColor() const
{
    return m_shadesBorderColor;
}

/**
 * @brief Executes the shades brush procedure for QCustomAxis.
 * @return Output value of type QBrush.
 */
QBrush QCustomAxis::shadesBrush() const
{
    return m_shadesBrush;
}

/**
 * @brief Executes the shades color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::shadesColor() const
{
    return m_shadesColor;
}

/**
 * @brief Executes the shades pen procedure for QCustomAxis.
 * @return Output value of type QPen.
 */
QPen QCustomAxis::shadesPen() const
{
    return m_shadesPen;
}

/**
 * @brief Executes the shades visible procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::shadesVisible() const
{
    return m_shadesVisible;
}

/**
 * @brief Sets the title brush parameter and updates internal state.
 * @param brush Input parameter specifying brush (const QBrush).
 */
void QCustomAxis::setTitleBrush(const QBrush &brush)
{
    if (m_titleBrush != brush)
    {
        m_titleBrush = brush;
        titleBrushChanged(brush);
    }
}

/**
 * @brief Sets the titles font parameter and updates internal state.
 * @param font Input parameter specifying font (const QFont).
 */
void QCustomAxis::setTitlesFont(const QFont &font)
{
    if (m_titleFont != font)
    {
        m_titleFont = font;
        titleFontChanged(font);
    }
}

/**
 * @brief Sets the title text parameter and updates internal state.
 * @param text Input parameter specifying text (const QString).
 */
void QCustomAxis::setTitleText(const QString &text)
{
    if (m_titleText != text)
    {
        m_titleText = text;
        titleTextChanged(text);
    }
}

/**
 * @brief Sets the title visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomAxis::setTitleVisible(bool visible)
{
    if (m_titleVisible != visible)
    {
        m_titleVisible = visible;
        titleVisibleChanged(visible);
    }
}

/**
 * @brief Executes the title brush procedure for QCustomAxis.
 * @return Output value of type QBrush.
 */
QBrush QCustomAxis::titleBrush() const
{
    return m_titleBrush;
}

/**
 * @brief Executes the title font procedure for QCustomAxis.
 * @return Output value of type QFont.
 */
QFont QCustomAxis::titleFont() const
{
    return m_titleFont;
}

/**
 * @brief Executes the title text procedure for QCustomAxis.
 * @return Output value of type QString.
 */
QString QCustomAxis::titleText() const
{
    return m_titleText;
}

/**
 * @brief Executes the title visible procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::titleVisible() const
{
    return m_titleVisible;
}

/**
 * @brief Sets the ticks parameter and updates internal state.
 * @param show Input parameter specifying show (bool).
 */
void QCustomAxis::setTicks(bool show)
{
    m_showTicks = show;
}

/****************************************/
/***----- Grid related functions -----***/
/****************************************/

void QCustomAxis::setGridLinePen(const QPen &pen)
{
    if (m_gridLinePen != pen)
    {
        m_gridLinePen = pen;
        gridLinePenChanged(pen);
    }
}

/**
 * @brief Sets the grid line color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setGridLineColor(const QColor &color)
{
    if (m_gridLineColor != color)
    {
        m_gridLineColor = color;
        gridLineColorChanged(color);
    }
}

/**
 * @brief Sets the grid line visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomAxis::setGridLineVisible(bool visible)
{
    if (m_gridLineVisible != visible)
    {
        m_gridLineVisible = visible;
        gridLineVisibleChanged(visible);
    }
}

/**
 * @brief Sets the grid line count parameter and updates internal state.
 * @param count Input parameter specifying count (int).
 */
void QCustomAxis::setGridLineCount(int count)
{
    m_gridLineCount = count;
}

/**
 * @brief Executes the grid line pen procedure for QCustomAxis.
 * @return Output value of type QPen.
 */
QPen QCustomAxis::gridLinePen() const
{
    return m_gridLinePen;
}

/**
 * @brief Executes the grid line color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::gridLineColor() const
{
    return m_gridLineColor;
}

/**
 * @brief Executes the grid line visible procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::gridLineVisible() const
{
    return m_gridLineVisible;
}

/**
 * @brief Executes the grid line count procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::gridLineCount() const
{
    return m_gridLineCount;
}

/**
 * @brief Sets the minor grid line pen parameter and updates internal state.
 * @param pen Input parameter specifying pen (const QPen).
 */
void QCustomAxis::setMinorGridLinePen(const QPen &pen)
{
    if (m_minorGridLinePen != pen)
    {
        m_minorGridLinePen = pen;
        minorGridLinePenChanged(pen);
    }
}

/**
 * @brief Sets the minor grid line color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setMinorGridLineColor(const QColor &color)
{
    if (m_minorGridLineColor != color)
    {
        m_minorGridLineColor = color;
        minorGridLineColorChanged(color);
    }
}

/**
 * @brief Sets the minor grid line visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomAxis::setMinorGridLineVisible(bool visible)
{
    if (m_minorGridLineVisible != visible)
    {
        m_minorGridLineVisible = visible;
        minorGridLineVisibleChanged(visible);
    }
}

/**
 * @brief Executes the minor grid line pen procedure for QCustomAxis.
 * @return Output value of type QPen.
 */
QPen QCustomAxis::minorGridLinePen() const
{
    return m_minorGridLinePen;
}

/**
 * @brief Executes the minor grid line color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::minorGridLineColor() const
{
    return m_minorGridLineColor;
}

/**
 * @brief Executes the minor grid line visible procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::minorGridLineVisible() const
{
    return m_minorGridLineVisible;
}

/****************************************/
/***----- Tick related functions -----***/
/****************************************/

void QCustomAxis::setTickLabels(bool show)
{
    m_showTickLabels = show;
}

/**
 * @brief Sets the tick label padding parameter and updates internal state.
 * @param padding Input parameter specifying padding (int).
 */
void QCustomAxis::setTickLabelPadding(int padding)
{
    m_tickLabelPadding = padding;
}

/**
 * @brief Sets the tick label font parameter and updates internal state.
 * @param font Input parameter specifying font (const QFont).
 */
void QCustomAxis::setTickLabelFont(const QFont &font)
{
    m_tickLabelFont = font;
}

/**
 * @brief Sets the tick label color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomAxis::setTickLabelColor(const QColor &color)
{
    m_tickLabelColor = color;
}

/**
 * @brief Sets the tick length parameter and updates internal state.
 * @param inside Input parameter specifying inside (int).
 * @param outside Input parameter specifying outside (int).
 */
void QCustomAxis::setTickLength(int inside, int outside)
{
    m_tickLengthIn = inside;
    m_tickLengthOut = outside;
}

/**
 * @brief Sets the tick length in parameter and updates internal state.
 * @param inside Input parameter specifying inside (int).
 */
void QCustomAxis::setTickLengthIn(int inside)
{
    m_tickLengthIn = inside;
}

/**
 * @brief Sets the tick length out parameter and updates internal state.
 * @param outside Input parameter specifying outside (int).
 */
void QCustomAxis::setTickLengthOut(int outside)
{
    m_tickLengthOut = outside;
}

/**
 * @brief Sets the tick pen parameter and updates internal state.
 * @param pen Input parameter specifying pen (const QPen).
 */
void QCustomAxis::setTickPen(const QPen &pen)
{
    m_tickPen = pen;
}

/**
 * @brief Executes the ticks procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::ticks() const
{
    return m_showTicks;
}

/**
 * @brief Executes the ticks labels procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::ticksLabels() const
{
    return m_showTickLabels;
}

/**
 * @brief Executes the tick label padding procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::tickLabelPadding() const
{
    return m_tickLabelPadding;
}

/**
 * @brief Executes the tick label font procedure for QCustomAxis.
 * @return Output value of type QFont.
 */
QFont QCustomAxis::tickLabelFont() const
{
    return m_tickLabelFont;
}

/**
 * @brief Executes the tick label color procedure for QCustomAxis.
 * @return Output value of type QColor.
 */
QColor QCustomAxis::tickLabelColor() const
{
    return m_tickLabelColor;
}

/**
 * @brief Executes the tick length procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::tickLength() const
{
    return (m_tickLengthIn + m_tickLengthOut);
}

/**
 * @brief Executes the tick length in procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::tickLengthIn() const
{
    return m_tickLengthIn;
}

/**
 * @brief Executes the tick length out procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::tickLengthOut() const
{
    return m_tickLengthOut;
}

/**
 * @brief Executes the tick pen procedure for QCustomAxis.
 * @return Output value of type QPen.
 */
QPen QCustomAxis::tickPen() const
{
    return m_tickPen;
}

/********************************************/
/***----- Sub tick related functions -----***/
/********************************************/

void QCustomAxis::setSubTicks(bool show)
{
    m_showSubTicks = show;
}

/**
 * @brief Sets the sub tick length parameter and updates internal state.
 * @param inside Input parameter specifying inside (int).
 * @param outside Input parameter specifying outside (int).
 */
void QCustomAxis::setSubTickLength(int inside, int outside)
{
    m_subTickLengthIn = inside;
    m_subTickLengthOut = outside;
}

/**
 * @brief Sets the sub tick length in parameter and updates internal state.
 * @param inside Input parameter specifying inside (int).
 */
void QCustomAxis::setSubTickLengthIn(int inside)
{
    m_subTickLengthIn = inside;
}

/**
 * @brief Sets the sub tick length out parameter and updates internal state.
 * @param outside Input parameter specifying outside (int).
 */
void QCustomAxis::setSubTickLengthOut(int outside)
{
    m_subTickLengthOut = outside;
}

/**
 * @brief Sets the sub tick pen parameter and updates internal state.
 * @param pen Input parameter specifying pen (const QPen).
 */
void QCustomAxis::setSubTickPen(const QPen &pen)
{
    m_subTickPen = pen;
}

/**
 * @brief Executes the sub ticks procedure for QCustomAxis.
 * @return Output value of type bool.
 */
bool QCustomAxis::subTicks() const
{
    return m_showSubTicks;
}

/**
 * @brief Executes the sub tick length procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::subTickLength() const
{
    return (m_subTickLengthIn + m_subTickLengthOut);
}

/**
 * @brief Executes the sub tick length in procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::subTickLengthIn() const
{
    return m_subTickLengthIn;
}

/**
 * @brief Executes the sub tick length out procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::subTickLengthOut() const
{
    return m_subTickLengthOut;
}

/**
 * @brief Executes the sub tick pen procedure for QCustomAxis.
 * @return Output value of type QPen.
 */
QPen QCustomAxis::subTickPen() const
{
    return m_subTickPen;
}

/****************************************************/
/***----- Number Precision related functions -----***/
/****************************************************/

void QCustomAxis::setNumberPrecision(int precision)
{
    m_numberPrecision = precision;
}

/**
 * @brief Executes the number precision procedure for QCustomAxis.
 * @return Output value of type int.
 */
int QCustomAxis::numberPrecision() const
{
    return m_numberPrecision;
}

/*****************************************/
/***----- Scale related functions -----***/
/*****************************************/

void QCustomAxis::setScaleType(ScaleType scaleType)
{
    m_scaleType = scaleType;
}

/**
 * @brief Executes the scale type procedure for QCustomAxis.
 * @return Output value of type ScaleType.
 */
ScaleType QCustomAxis::scaleType() const
{
    return m_scaleType;
}
