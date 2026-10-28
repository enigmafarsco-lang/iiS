// ============================================================================
// File: QCustomAxis.cpp
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for QCustomAxis.cpp.
// ============================================================================

#include "QCustomAxis.h"

// Constructor: Initializes QCustomAxis subsystem components, default parameters, and state.
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

// Destructor: Cleans up QCustomAxis allocated resources, timers, and buffers.
QCustomAxis::~QCustomAxis()
{
}

// Sets alignment parameter and updates internal configuration.
void QCustomAxis::setAlignment(const Qt::Alignment &alignment)
{
    if (m_alignment != alignment)
    {
        m_alignment = alignment;
        alignmentChanged(alignment);
    }
}

// Executes alignment routine.
Qt::Alignment QCustomAxis::alignment() const
{
    return m_alignment;
}

// Sets orientation parameter and updates internal configuration.
void QCustomAxis::setOrientation(const Qt::Orientation &orientation)
{
    if (m_orientation != orientation)
    {
        m_orientation = orientation;
        orientationChanged(orientation);
    }
}

// Executes orientation routine.
Qt::Orientation QCustomAxis::orientation() const
{
    return m_orientation;
}

// Sets max parameter and updates internal configuration.
void QCustomAxis::setMax(const double &max)
{
    m_upperPartialRangeSet = true;
    if (m_max != max)
    {
        m_max = max;
    }
}

// Sets min parameter and updates internal configuration.
void QCustomAxis::setMin(const double &min)
{
    m_lowerPartialRangeSet = true;
    if (m_min != min)
    {
        m_min = min;
    }
}

// Executes visibility routine.
bool QCustomAxis::visibility() const
{
    return m_axisVisibility;
}

// Sets visible parameter and updates internal configuration.
void QCustomAxis::setVisible(bool on)
{
    m_axisVisibility = on;
}

// Executes range reversed routine.
bool QCustomAxis::rangeReversed() const
{
    return m_reverseRange;
}

// Sets range reversed parameter and updates internal configuration.
void QCustomAxis::setRangeReversed(bool reversed)
{
    m_reverseRange = reversed;
}

// Sets range parameter and updates internal configuration.
void QCustomAxis::setRange(const double &min, const double &max)
{
    setMin(min);
    setMax(max);
    m_lowerPartialRangeSet = false;
    m_upperPartialRangeSet = false;
}

// Retrieves current artial range set value.
QPair<bool, bool> QCustomAxis::isPartialRangeSet() const
{
    return QPair<bool, bool>(m_lowerPartialRangeSet, m_upperPartialRangeSet);
}

// Retrieves current max value.
double QCustomAxis::getMax() const
{
    return m_max;
}

// Retrieves current min value.
double QCustomAxis::getMin() const
{
    return m_min;
}

// Retrieves current range value.
QPair<qreal, qreal> QCustomAxis::getRange() const
{
    QPair<qreal, qreal> range(m_min, m_max);
    return range;
}

// Sets labels angle parameter and updates internal configuration.
void QCustomAxis::setLabelsAngle(const qint32 &angle)
{
    if (m_labelsAngle != angle)
    {
        m_labelsAngle = angle;
        labelsAngleChanged(angle);
    }
}

// Sets labels brush parameter and updates internal configuration.
void QCustomAxis::setLabelsBrush(const QBrush &brush)
{
    if (m_labelsBrush != brush)
    {
        m_labelsBrush = brush;
        labelsBrushChanged(brush);
    }
}

// Sets labels color parameter and updates internal configuration.
void QCustomAxis::setLabelsColor(const QColor &color)
{
    if (m_labelsColor != color)
    {
        m_labelsColor = color;
        labelsColorChanged(color);
    }
}

// Sets labels font parameter and updates internal configuration.
void QCustomAxis::setLabelsFont(const QFont &font)
{
    if (m_labelsFont != font)
    {
        m_labelsFont = font;
        labelsFontChanged(font);
    }
}

// Sets labels text parameter and updates internal configuration.
void QCustomAxis::setLabelsText(const QString &text)
{
    if (m_labelsText != text)
    {
        m_labelsText = text;
        labelsTextChanged(text);
    }
}

// Sets labels visible parameter and updates internal configuration.
void QCustomAxis::setLabelsVisible(bool visible)
{
    if (m_labelsVisibility != visible)
    {
        m_labelsVisibility = visible;
        labelsVisibleChanged(visible);
    }
}

// Executes labels angle routine.
qint32 QCustomAxis::labelsAngle() const
{
    return m_labelsAngle;
}

// Executes labels brush routine.
QBrush QCustomAxis::labelsBrush() const
{
    return m_labelsBrush;
}

// Executes labels color routine.
QColor QCustomAxis::labelsColor() const
{
    return m_labelsColor;
}

// Executes labels font routine.
QFont QCustomAxis::labelsFont() const
{
    return m_labelsFont;
}

// Executes labels text routine.
QString QCustomAxis::labelsText() const
{
    return m_labelsText;
}

// Executes labels visible routine.
bool QCustomAxis::labelsVisible() const
{
    return m_labelsVisibility;
}

// Sets line pen parameter and updates internal configuration.
void QCustomAxis::setLinePen(const QPen &pen)
{
    if (m_linePen != pen)
    {
        m_linePen = pen;
        linePenChanged(pen);
    }
}

// Sets line pen color parameter and updates internal configuration.
void QCustomAxis::setLinePenColor(const QColor &color)
{
    if (m_linePenColor != color)
    {
        m_linePenColor = color;
        linePenColorChanged(color);
    }
}

// Sets line visible parameter and updates internal configuration.
void QCustomAxis::setLineVisible(bool visible)
{
    if (m_lineVisible != visible)
    {
        m_lineVisible = visible;
        lineVisibleChanged(visible);
    }
}

// Executes line pen routine.
QPen QCustomAxis::linePen() const
{
    return m_linePen;
}

// Executes line pen color routine.
QColor QCustomAxis::linePenColor() const
{
    return m_linePenColor;
}

// Executes line visible routine.
bool QCustomAxis::lineVisible() const
{
    return m_lineVisible;
}

// Sets shades border color parameter and updates internal configuration.
void QCustomAxis::setShadesBorderColor(const QColor &color)
{
    if (m_shadesBorderColor != color)
    {
        m_shadesBorderColor = color;
        shadesBorderColorChanged(color);
    }
}

// Sets shades brush parameter and updates internal configuration.
void QCustomAxis::setShadesBrush(const QBrush &brush)
{
    if (m_shadesBrush != brush)
    {
        m_shadesBrush = brush;
        shadesBrushChanged(brush);
    }
}

// Sets shades color parameter and updates internal configuration.
void QCustomAxis::setShadesColor(const QColor &color)
{
    if (m_shadesColor != color)
    {
        m_shadesColor = color;
        shadesColorChanged(color);
    }
}

// Sets shades pen parameter and updates internal configuration.
void QCustomAxis::setShadesPen(const QPen &pen)
{
    if (m_shadesPen != pen)
    {
        m_shadesPen = pen;
        shadesPenChanged(pen);
    }
}

// Sets shades visible parameter and updates internal configuration.
void QCustomAxis::setShadesVisible(bool visible)
{
    if (m_shadesVisible != visible)
    {
        m_shadesVisible = visible;
        shadesVisibleChanged(visible);
    }
}

// Executes shades border color routine.
QColor QCustomAxis::shadesBorderColor() const
{
    return m_shadesBorderColor;
}

// Executes shades brush routine.
QBrush QCustomAxis::shadesBrush() const
{
    return m_shadesBrush;
}

// Executes shades color routine.
QColor QCustomAxis::shadesColor() const
{
    return m_shadesColor;
}

// Executes shades pen routine.
QPen QCustomAxis::shadesPen() const
{
    return m_shadesPen;
}

// Executes shades visible routine.
bool QCustomAxis::shadesVisible() const
{
    return m_shadesVisible;
}

// Sets title brush parameter and updates internal configuration.
void QCustomAxis::setTitleBrush(const QBrush &brush)
{
    if (m_titleBrush != brush)
    {
        m_titleBrush = brush;
        titleBrushChanged(brush);
    }
}

// Sets titles font parameter and updates internal configuration.
void QCustomAxis::setTitlesFont(const QFont &font)
{
    if (m_titleFont != font)
    {
        m_titleFont = font;
        titleFontChanged(font);
    }
}

// Sets title text parameter and updates internal configuration.
void QCustomAxis::setTitleText(const QString &text)
{
    if (m_titleText != text)
    {
        m_titleText = text;
        titleTextChanged(text);
    }
}

// Sets title visible parameter and updates internal configuration.
void QCustomAxis::setTitleVisible(bool visible)
{
    if (m_titleVisible != visible)
    {
        m_titleVisible = visible;
        titleVisibleChanged(visible);
    }
}

// Executes title brush routine.
QBrush QCustomAxis::titleBrush() const
{
    return m_titleBrush;
}

// Executes title font routine.
QFont QCustomAxis::titleFont() const
{
    return m_titleFont;
}

// Executes title text routine.
QString QCustomAxis::titleText() const
{
    return m_titleText;
}

// Executes title visible routine.
bool QCustomAxis::titleVisible() const
{
    return m_titleVisible;
}

// Sets ticks parameter and updates internal configuration.
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

// Sets grid line color parameter and updates internal configuration.
void QCustomAxis::setGridLineColor(const QColor &color)
{
    if (m_gridLineColor != color)
    {
        m_gridLineColor = color;
        gridLineColorChanged(color);
    }
}

// Sets grid line visible parameter and updates internal configuration.
void QCustomAxis::setGridLineVisible(bool visible)
{
    if (m_gridLineVisible != visible)
    {
        m_gridLineVisible = visible;
        gridLineVisibleChanged(visible);
    }
}

// Sets grid line count parameter and updates internal configuration.
void QCustomAxis::setGridLineCount(int count)
{
    m_gridLineCount = count;
}

// Executes grid line pen routine.
QPen QCustomAxis::gridLinePen() const
{
    return m_gridLinePen;
}

// Executes grid line color routine.
QColor QCustomAxis::gridLineColor() const
{
    return m_gridLineColor;
}

// Executes grid line visible routine.
bool QCustomAxis::gridLineVisible() const
{
    return m_gridLineVisible;
}

// Executes grid line count routine.
int QCustomAxis::gridLineCount() const
{
    return m_gridLineCount;
}

// Sets minor grid line pen parameter and updates internal configuration.
void QCustomAxis::setMinorGridLinePen(const QPen &pen)
{
    if (m_minorGridLinePen != pen)
    {
        m_minorGridLinePen = pen;
        minorGridLinePenChanged(pen);
    }
}

// Sets minor grid line color parameter and updates internal configuration.
void QCustomAxis::setMinorGridLineColor(const QColor &color)
{
    if (m_minorGridLineColor != color)
    {
        m_minorGridLineColor = color;
        minorGridLineColorChanged(color);
    }
}

// Sets minor grid line visible parameter and updates internal configuration.
void QCustomAxis::setMinorGridLineVisible(bool visible)
{
    if (m_minorGridLineVisible != visible)
    {
        m_minorGridLineVisible = visible;
        minorGridLineVisibleChanged(visible);
    }
}

// Executes minor grid line pen routine.
QPen QCustomAxis::minorGridLinePen() const
{
    return m_minorGridLinePen;
}

// Executes minor grid line color routine.
QColor QCustomAxis::minorGridLineColor() const
{
    return m_minorGridLineColor;
}

// Executes minor grid line visible routine.
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

// Sets tick label padding parameter and updates internal configuration.
void QCustomAxis::setTickLabelPadding(int padding)
{
    m_tickLabelPadding = padding;
}

// Sets tick label font parameter and updates internal configuration.
void QCustomAxis::setTickLabelFont(const QFont &font)
{
    m_tickLabelFont = font;
}

// Sets tick label color parameter and updates internal configuration.
void QCustomAxis::setTickLabelColor(const QColor &color)
{
    m_tickLabelColor = color;
}

// Sets tick length parameter and updates internal configuration.
void QCustomAxis::setTickLength(int inside, int outside)
{
    m_tickLengthIn = inside;
    m_tickLengthOut = outside;
}

// Sets tick length in parameter and updates internal configuration.
void QCustomAxis::setTickLengthIn(int inside)
{
    m_tickLengthIn = inside;
}

// Sets tick length out parameter and updates internal configuration.
void QCustomAxis::setTickLengthOut(int outside)
{
    m_tickLengthOut = outside;
}

// Sets tick pen parameter and updates internal configuration.
void QCustomAxis::setTickPen(const QPen &pen)
{
    m_tickPen = pen;
}

// Executes ticks routine.
bool QCustomAxis::ticks() const
{
    return m_showTicks;
}

// Executes ticks labels routine.
bool QCustomAxis::ticksLabels() const
{
    return m_showTickLabels;
}

// Executes tick label padding routine.
int QCustomAxis::tickLabelPadding() const
{
    return m_tickLabelPadding;
}

// Executes tick label font routine.
QFont QCustomAxis::tickLabelFont() const
{
    return m_tickLabelFont;
}

// Executes tick label color routine.
QColor QCustomAxis::tickLabelColor() const
{
    return m_tickLabelColor;
}

// Executes tick length routine.
int QCustomAxis::tickLength() const
{
    return (m_tickLengthIn + m_tickLengthOut);
}

// Executes tick length in routine.
int QCustomAxis::tickLengthIn() const
{
    return m_tickLengthIn;
}

// Executes tick length out routine.
int QCustomAxis::tickLengthOut() const
{
    return m_tickLengthOut;
}

// Executes tick pen routine.
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

// Sets sub tick length parameter and updates internal configuration.
void QCustomAxis::setSubTickLength(int inside, int outside)
{
    m_subTickLengthIn = inside;
    m_subTickLengthOut = outside;
}

// Sets sub tick length in parameter and updates internal configuration.
void QCustomAxis::setSubTickLengthIn(int inside)
{
    m_subTickLengthIn = inside;
}

// Sets sub tick length out parameter and updates internal configuration.
void QCustomAxis::setSubTickLengthOut(int outside)
{
    m_subTickLengthOut = outside;
}

// Sets sub tick pen parameter and updates internal configuration.
void QCustomAxis::setSubTickPen(const QPen &pen)
{
    m_subTickPen = pen;
}

// Executes sub ticks routine.
bool QCustomAxis::subTicks() const
{
    return m_showSubTicks;
}

// Executes sub tick length routine.
int QCustomAxis::subTickLength() const
{
    return (m_subTickLengthIn + m_subTickLengthOut);
}

// Executes sub tick length in routine.
int QCustomAxis::subTickLengthIn() const
{
    return m_subTickLengthIn;
}

// Executes sub tick length out routine.
int QCustomAxis::subTickLengthOut() const
{
    return m_subTickLengthOut;
}

// Executes sub tick pen routine.
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

// Executes number precision routine.
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

// Executes scale type routine.
ScaleType QCustomAxis::scaleType() const
{
    return m_scaleType;
}
