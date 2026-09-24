#include "QCustomAxis.h"

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

QCustomAxis::~QCustomAxis()
{
}

void QCustomAxis::setAlignment(const Qt::Alignment &alignment)
{
    if (m_alignment != alignment)
    {
        m_alignment = alignment;
        alignmentChanged(alignment);
    }
}

Qt::Alignment QCustomAxis::alignment() const
{
    return m_alignment;
}

void QCustomAxis::setOrientation(const Qt::Orientation &orientation)
{
    if (m_orientation != orientation)
    {
        m_orientation = orientation;
        orientationChanged(orientation);
    }
}

Qt::Orientation QCustomAxis::orientation() const
{
    return m_orientation;
}

void QCustomAxis::setMax(const double &max)
{
    m_upperPartialRangeSet = true;
    if (m_max != max)
    {
        m_max = max;
    }
}

void QCustomAxis::setMin(const double &min)
{
    m_lowerPartialRangeSet = true;
    if (m_min != min)
    {
        m_min = min;
    }
}

bool QCustomAxis::visibility() const
{
    return m_axisVisibility;
}

void QCustomAxis::setVisible(bool on)
{
    m_axisVisibility = on;
}

bool QCustomAxis::rangeReversed() const
{
    return m_reverseRange;
}

void QCustomAxis::setRangeReversed(bool reversed)
{
    m_reverseRange = reversed;
}

void QCustomAxis::setRange(const double &min, const double &max)
{
    setMin(min);
    setMax(max);
    m_lowerPartialRangeSet = false;
    m_upperPartialRangeSet = false;
}

QPair<bool, bool> QCustomAxis::isPartialRangeSet() const
{
    return QPair<bool, bool>(m_lowerPartialRangeSet, m_upperPartialRangeSet);
}

double QCustomAxis::getMax() const
{
    return m_max;
}

double QCustomAxis::getMin() const
{
    return m_min;
}

QPair<qreal, qreal> QCustomAxis::getRange() const
{
    QPair<qreal, qreal> range(m_min, m_max);
    return range;
}

void QCustomAxis::setLabelsAngle(const qint32 &angle)
{
    if (m_labelsAngle != angle)
    {
        m_labelsAngle = angle;
        labelsAngleChanged(angle);
    }
}

void QCustomAxis::setLabelsBrush(const QBrush &brush)
{
    if (m_labelsBrush != brush)
    {
        m_labelsBrush = brush;
        labelsBrushChanged(brush);
    }
}

void QCustomAxis::setLabelsColor(const QColor &color)
{
    if (m_labelsColor != color)
    {
        m_labelsColor = color;
        labelsColorChanged(color);
    }
}

void QCustomAxis::setLabelsFont(const QFont &font)
{
    if (m_labelsFont != font)
    {
        m_labelsFont = font;
        labelsFontChanged(font);
    }
}

void QCustomAxis::setLabelsText(const QString &text)
{
    if (m_labelsText != text)
    {
        m_labelsText = text;
        labelsTextChanged(text);
    }
}

void QCustomAxis::setLabelsVisible(bool visible)
{
    if (m_labelsVisibility != visible)
    {
        m_labelsVisibility = visible;
        labelsVisibleChanged(visible);
    }
}

qint32 QCustomAxis::labelsAngle() const
{
    return m_labelsAngle;
}

QBrush QCustomAxis::labelsBrush() const
{
    return m_labelsBrush;
}

QColor QCustomAxis::labelsColor() const
{
    return m_labelsColor;
}

QFont QCustomAxis::labelsFont() const
{
    return m_labelsFont;
}

QString QCustomAxis::labelsText() const
{
    return m_labelsText;
}

bool QCustomAxis::labelsVisible() const
{
    return m_labelsVisibility;
}

void QCustomAxis::setLinePen(const QPen &pen)
{
    if (m_linePen != pen)
    {
        m_linePen = pen;
        linePenChanged(pen);
    }
}

void QCustomAxis::setLinePenColor(const QColor &color)
{
    if (m_linePenColor != color)
    {
        m_linePenColor = color;
        linePenColorChanged(color);
    }
}

void QCustomAxis::setLineVisible(bool visible)
{
    if (m_lineVisible != visible)
    {
        m_lineVisible = visible;
        lineVisibleChanged(visible);
    }
}

QPen QCustomAxis::linePen() const
{
    return m_linePen;
}

QColor QCustomAxis::linePenColor() const
{
    return m_linePenColor;
}

bool QCustomAxis::lineVisible() const
{
    return m_lineVisible;
}

void QCustomAxis::setShadesBorderColor(const QColor &color)
{
    if (m_shadesBorderColor != color)
    {
        m_shadesBorderColor = color;
        shadesBorderColorChanged(color);
    }
}

void QCustomAxis::setShadesBrush(const QBrush &brush)
{
    if (m_shadesBrush != brush)
    {
        m_shadesBrush = brush;
        shadesBrushChanged(brush);
    }
}

void QCustomAxis::setShadesColor(const QColor &color)
{
    if (m_shadesColor != color)
    {
        m_shadesColor = color;
        shadesColorChanged(color);
    }
}

void QCustomAxis::setShadesPen(const QPen &pen)
{
    if (m_shadesPen != pen)
    {
        m_shadesPen = pen;
        shadesPenChanged(pen);
    }
}

void QCustomAxis::setShadesVisible(bool visible)
{
    if (m_shadesVisible != visible)
    {
        m_shadesVisible = visible;
        shadesVisibleChanged(visible);
    }
}

QColor QCustomAxis::shadesBorderColor() const
{
    return m_shadesBorderColor;
}

QBrush QCustomAxis::shadesBrush() const
{
    return m_shadesBrush;
}

QColor QCustomAxis::shadesColor() const
{
    return m_shadesColor;
}

QPen QCustomAxis::shadesPen() const
{
    return m_shadesPen;
}

bool QCustomAxis::shadesVisible() const
{
    return m_shadesVisible;
}

void QCustomAxis::setTitleBrush(const QBrush &brush)
{
    if (m_titleBrush != brush)
    {
        m_titleBrush = brush;
        titleBrushChanged(brush);
    }
}

void QCustomAxis::setTitlesFont(const QFont &font)
{
    if (m_titleFont != font)
    {
        m_titleFont = font;
        titleFontChanged(font);
    }
}

void QCustomAxis::setTitleText(const QString &text)
{
    if (m_titleText != text)
    {
        m_titleText = text;
        titleTextChanged(text);
    }
}

void QCustomAxis::setTitleVisible(bool visible)
{
    if (m_titleVisible != visible)
    {
        m_titleVisible = visible;
        titleVisibleChanged(visible);
    }
}

QBrush QCustomAxis::titleBrush() const
{
    return m_titleBrush;
}

QFont QCustomAxis::titleFont() const
{
    return m_titleFont;
}

QString QCustomAxis::titleText() const
{
    return m_titleText;
}

bool QCustomAxis::titleVisible() const
{
    return m_titleVisible;
}

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

void QCustomAxis::setGridLineColor(const QColor &color)
{
    if (m_gridLineColor != color)
    {
        m_gridLineColor = color;
        gridLineColorChanged(color);
    }
}

void QCustomAxis::setGridLineVisible(bool visible)
{
    if (m_gridLineVisible != visible)
    {
        m_gridLineVisible = visible;
        gridLineVisibleChanged(visible);
    }
}

void QCustomAxis::setGridLineCount(int count)
{
    m_gridLineCount = count;
}

QPen QCustomAxis::gridLinePen() const
{
    return m_gridLinePen;
}

QColor QCustomAxis::gridLineColor() const
{
    return m_gridLineColor;
}

bool QCustomAxis::gridLineVisible() const
{
    return m_gridLineVisible;
}

int QCustomAxis::gridLineCount() const
{
    return m_gridLineCount;
}

void QCustomAxis::setMinorGridLinePen(const QPen &pen)
{
    if (m_minorGridLinePen != pen)
    {
        m_minorGridLinePen = pen;
        minorGridLinePenChanged(pen);
    }
}

void QCustomAxis::setMinorGridLineColor(const QColor &color)
{
    if (m_minorGridLineColor != color)
    {
        m_minorGridLineColor = color;
        minorGridLineColorChanged(color);
    }
}

void QCustomAxis::setMinorGridLineVisible(bool visible)
{
    if (m_minorGridLineVisible != visible)
    {
        m_minorGridLineVisible = visible;
        minorGridLineVisibleChanged(visible);
    }
}

QPen QCustomAxis::minorGridLinePen() const
{
    return m_minorGridLinePen;
}

QColor QCustomAxis::minorGridLineColor() const
{
    return m_minorGridLineColor;
}

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

void QCustomAxis::setTickLabelPadding(int padding)
{
    m_tickLabelPadding = padding;
}

void QCustomAxis::setTickLabelFont(const QFont &font)
{
    m_tickLabelFont = font;
}

void QCustomAxis::setTickLabelColor(const QColor &color)
{
    m_tickLabelColor = color;
}

void QCustomAxis::setTickLength(int inside, int outside)
{
    m_tickLengthIn = inside;
    m_tickLengthOut = outside;
}

void QCustomAxis::setTickLengthIn(int inside)
{
    m_tickLengthIn = inside;
}

void QCustomAxis::setTickLengthOut(int outside)
{
    m_tickLengthOut = outside;
}

void QCustomAxis::setTickPen(const QPen &pen)
{
    m_tickPen = pen;
}

bool QCustomAxis::ticks() const
{
    return m_showTicks;
}

bool QCustomAxis::ticksLabels() const
{
    return m_showTickLabels;
}

int QCustomAxis::tickLabelPadding() const
{
    return m_tickLabelPadding;
}

QFont QCustomAxis::tickLabelFont() const
{
    return m_tickLabelFont;
}

QColor QCustomAxis::tickLabelColor() const
{
    return m_tickLabelColor;
}

int QCustomAxis::tickLength() const
{
    return (m_tickLengthIn + m_tickLengthOut);
}

int QCustomAxis::tickLengthIn() const
{
    return m_tickLengthIn;
}

int QCustomAxis::tickLengthOut() const
{
    return m_tickLengthOut;
}

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

void QCustomAxis::setSubTickLength(int inside, int outside)
{
    m_subTickLengthIn = inside;
    m_subTickLengthOut = outside;
}

void QCustomAxis::setSubTickLengthIn(int inside)
{
    m_subTickLengthIn = inside;
}

void QCustomAxis::setSubTickLengthOut(int outside)
{
    m_subTickLengthOut = outside;
}

void QCustomAxis::setSubTickPen(const QPen &pen)
{
    m_subTickPen = pen;
}

bool QCustomAxis::subTicks() const
{
    return m_showSubTicks;
}

int QCustomAxis::subTickLength() const
{
    return (m_subTickLengthIn + m_subTickLengthOut);
}

int QCustomAxis::subTickLengthIn() const
{
    return m_subTickLengthIn;
}

int QCustomAxis::subTickLengthOut() const
{
    return m_subTickLengthOut;
}

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

ScaleType QCustomAxis::scaleType() const
{
    return m_scaleType;
}
