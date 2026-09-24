#include "tiltgauge.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>

BTiltGauge::BTiltGauge(QWidget *parent)
    : QWidget(parent)
{
    init_panGauge();
}

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

void BTiltGauge::setValue(int value)
{
    setValue((double)value);
    update();
}

void BTiltGauge::setMinValue(double value)
{
    m_minValue=value;
    update();
}

void BTiltGauge::setMinValue(int value)
{
    setMinValue((double)value);
}


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

void BTiltGauge::setMaxValue(int value)
{
    setMaxValue((double)value);
}


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

void BTiltGauge::setThreshold(int value)
{
    setThreshold((double)value);
}

void BTiltGauge::setPrecision(int precision)
{
    m_precision=precision;
    update();
}

void BTiltGauge::setPrecisionNumeric(int precision)
{
    m_precisionNumeric=precision;
    update();
}

void BTiltGauge::setUnits(QString units)
{
    m_units=units;
    update();
}

void BTiltGauge::setLabel(QString label)
{
    m_label=label;
}

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

void BTiltGauge::setSteps(int nSteps)
{
    m_steps=nSteps;
    update();
}

void BTiltGauge::setStartAngle(double value)
{
    m_startAngle=value;
    update();
}

void BTiltGauge::setEndAngle(double value)
{
    m_endAngle=value;
    update();
}

void BTiltGauge::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}

void BTiltGauge::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


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

void BTiltGauge::setThresholdEnabled(bool enable)
{
    m_thresholdEnabled=enable;
    update();
}

void BTiltGauge::setNumericIndicatorEnabled(bool enable)
{
    m_numericIndicatorEnabled=enable;
    update();
}

void BTiltGauge::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
    update();
}

void BTiltGauge::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
    update();
}

void BTiltGauge::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
    update();
}

void BTiltGauge::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
    update();

}

void BTiltGauge::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
    update();
}

void BTiltGauge::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
    update();
}
