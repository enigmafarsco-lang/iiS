// ============================================================================
// File: tiltgauge.cpp
// Subsystem: eLynxSDR Tilt
// Description: Subsystem implementation and interface guidance for tiltgauge.cpp.
// ============================================================================

#include "tiltgauge.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>

// Constructor: Initializes BTiltGauge subsystem components, default parameters, and state.
BTiltGauge::BTiltGauge(QWidget *parent)
    : QWidget(parent)
{
    init_panGauge();
}

// Initializes init pan gauge for BTiltGauge.
void BTiltGauge::init_panGauge()
{
    this->setFont(QFont("Arial", 5));
    m_precision=0;
    m_steps=1;

    m_thresholdFlag=false;
    m_foreground=QColor(Qt::black);
    m_background=QColor(Qt::white);
    m_foreground=QColor(Qt::white);
    m_background=QColor(Qt::black);

    m_thresholdEnabled=true;
    m_numericIndicatorEnabled=false;

    setMinValue(-10);
    setMaxValue(30);
    setValue(25);
    setStartAngle(-90);
    setEndAngle(90);
    setMinimumSize(QSize(80,80));
    setLabel("Tilt");
    setThreshold(25);
    setSteps(8);

    setBeginWarningValue(-10);
    setEndWarningValue(-5);
}

// Sets value parameter and updates internal configuration.
void BTiltGauge::setValue(double value)
{
    if(value>m_maxValue)
    {
        m_value=m_maxValue;
        emit errorSignal(OutOfRange);
    }
    else
        if(value<m_minValue)
        {
            m_value=m_minValue;
            emit errorSignal(OutOfRange);
        }
        else
            m_value=value;

    if(m_thresholdEnabled)
        thresholdManager();
    update();
}

// Sets value parameter and updates internal configuration.
void BTiltGauge::setValue(int value)
{
    setValue((double)value);
    update();
}

// Sets min value parameter and updates internal configuration.
void BTiltGauge::setMinValue(double value)
{
    m_minValue=value;
    update();
}

// Sets min value parameter and updates internal configuration.
void BTiltGauge::setMinValue(int value)
{
    setMinValue((double)value);
}


// Sets max value parameter and updates internal configuration.
void BTiltGauge::setMaxValue(double value)
{
    if(value > m_minValue)
    {
        m_maxValue=value;
        update();
    }
    else
        emit errorSignal(MaxValueError);
}

// Sets max value parameter and updates internal configuration.
void BTiltGauge::setMaxValue(int value)
{
    setMaxValue((double)value);
}


// Sets threshold parameter and updates internal configuration.
void BTiltGauge::setThreshold(double value)
{
    if(value > m_minValue && value < m_maxValue)
    {
        m_threshold=value;
        update();
    }
    else
        emit errorSignal(ThresholdError);
}

// Sets threshold parameter and updates internal configuration.
void BTiltGauge::setThreshold(int value)
{
    setThreshold((double)value);
}

// Sets precision parameter and updates internal configuration.
void BTiltGauge::setPrecision(int precision)
{
    m_precision=precision;
    update();
}

// Sets precision numeric parameter and updates internal configuration.
void BTiltGauge::setPrecisionNumeric(int precision)
{
    m_precisionNumeric=precision;
    update();
}

// Sets units parameter and updates internal configuration.
void BTiltGauge::setUnits(QString units)
{
    m_units=units;
    update();
}

// Sets label parameter and updates internal configuration.
void BTiltGauge::setLabel(QString label)
{
    m_label=label;
}

// Executes paint event routine.
void BTiltGauge::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int side = qMin(width(), height());

    painter.setViewport((width() - side) / 2, (height() - side) / 2,side, side);
    painter.setWindow(-50, -50, 100, 100);


    drawBackground(&painter);
    drawCoverGlass(&painter);
    drawTicks(&painter);
    drawScale(&painter);
    drawUnits(&painter);
    drawThresholdLine(&painter);
    drawWarningWindow((&painter));
    drawValidWindow(&painter);
    drawNeedle(&painter);
    drawNumericValue(&painter);
    drawLabel(&painter);
    drawCrown(&painter);
}

// Sets steps parameter and updates internal configuration.
void BTiltGauge::setSteps(int nSteps)
{
    m_steps=nSteps;
    update();
}

// Sets start angle parameter and updates internal configuration.
void BTiltGauge::setStartAngle(double value)
{
    m_startAngle=value;
    update();
}

// Sets end angle parameter and updates internal configuration.
void BTiltGauge::setEndAngle(double value)
{
    m_endAngle=value;
    update();
}

// Sets foreground parameter and updates internal configuration.
void BTiltGauge::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}

// Sets background parameter and updates internal configuration.
void BTiltGauge::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


// Executes threshold manager routine.
void BTiltGauge::thresholdManager()
{
    if(m_value > m_threshold && (!m_thresholdFlag) )
    {
        m_thresholdFlag=true;
        emit thresholdAlarm(m_thresholdFlag);
    }
    else
        if(m_value < m_threshold && m_thresholdFlag)
        {
            m_thresholdFlag=false;
            emit thresholdAlarm(m_thresholdFlag);
        }
}

// Sets threshold enabled parameter and updates internal configuration.
void BTiltGauge::setThresholdEnabled(bool enable)
{
    m_thresholdEnabled=enable;
    update();
}

// Sets numeric indicator enabled parameter and updates internal configuration.
void BTiltGauge::setNumericIndicatorEnabled(bool enable)
{
    m_numericIndicatorEnabled=enable;
    update();
}

// Sets begin valid value parameter and updates internal configuration.
void BTiltGauge::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
    update();
}

// Sets end valid value parameter and updates internal configuration.
void BTiltGauge::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
    update();
}

// Sets enable valid window parameter and updates internal configuration.
void BTiltGauge::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
    update();
}

// Sets begin warning value parameter and updates internal configuration.
void BTiltGauge::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
    update();

}

// Sets end warning value parameter and updates internal configuration.
void BTiltGauge::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
    update();
}

// Sets enable warning window parameter and updates internal configuration.
void BTiltGauge::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
    update();
}
