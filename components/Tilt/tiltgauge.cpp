/**
 * @file tiltgauge.cpp
 * @brief Subsystem module for Tilt in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "tiltgauge.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>

/**
 * @brief Constructs a new BTiltGauge instance and initializes subsystem components and UI state.
 * @param parent Input parameter specifying parent (QWidget).
 */
BTiltGauge::BTiltGauge(QWidget *parent)
    : QWidget(parent)
{
    init_panGauge();
}

/**
 * @brief Initializes and configures the init pan gauge for the BTiltGauge subsystem.
 */
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

/**
 * @brief Sets the value parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
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

/**
 * @brief Sets the value parameter and updates internal state.
 * @param value Input parameter specifying value (int).
 */
void BTiltGauge::setValue(int value)
{
    setValue((double)value);
    update();
}

/**
 * @brief Sets the min value parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BTiltGauge::setMinValue(double value)
{
    m_minValue=value;
    update();
}

/**
 * @brief Sets the min value parameter and updates internal state.
 * @param value Input parameter specifying value (int).
 */
void BTiltGauge::setMinValue(int value)
{
    setMinValue((double)value);
}


/**
 * @brief Sets the max value parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
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

/**
 * @brief Sets the max value parameter and updates internal state.
 * @param value Input parameter specifying value (int).
 */
void BTiltGauge::setMaxValue(int value)
{
    setMaxValue((double)value);
}


/**
 * @brief Sets the threshold parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
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

/**
 * @brief Sets the threshold parameter and updates internal state.
 * @param value Input parameter specifying value (int).
 */
void BTiltGauge::setThreshold(int value)
{
    setThreshold((double)value);
}

/**
 * @brief Sets the precision parameter and updates internal state.
 * @param precision Input parameter specifying precision (int).
 */
void BTiltGauge::setPrecision(int precision)
{
    m_precision=precision;
    update();
}

/**
 * @brief Sets the precision numeric parameter and updates internal state.
 * @param precision Input parameter specifying precision (int).
 */
void BTiltGauge::setPrecisionNumeric(int precision)
{
    m_precisionNumeric=precision;
    update();
}

/**
 * @brief Sets the units parameter and updates internal state.
 * @param units Input parameter specifying units (QString).
 */
void BTiltGauge::setUnits(QString units)
{
    m_units=units;
    update();
}

/**
 * @brief Sets the label parameter and updates internal state.
 * @param label Input parameter specifying label (QString).
 */
void BTiltGauge::setLabel(QString label)
{
    m_label=label;
}

/**
 * @brief Executes the paint event procedure for BTiltGauge.
 * @param event Input parameter specifying event (QPaintEvent).
 */
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

/**
 * @brief Sets the steps parameter and updates internal state.
 * @param nSteps Input parameter specifying nSteps (int).
 */
void BTiltGauge::setSteps(int nSteps)
{
    m_steps=nSteps;
    update();
}

/**
 * @brief Sets the start angle parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BTiltGauge::setStartAngle(double value)
{
    m_startAngle=value;
    update();
}

/**
 * @brief Sets the end angle parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BTiltGauge::setEndAngle(double value)
{
    m_endAngle=value;
    update();
}

/**
 * @brief Sets the foreground parameter and updates internal state.
 * @param newForeColor Input parameter specifying newForeColor (QColor).
 */
void BTiltGauge::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}

/**
 * @brief Sets the background parameter and updates internal state.
 * @param newBackColor Input parameter specifying newBackColor (QColor).
 */
void BTiltGauge::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


/**
 * @brief Executes the threshold manager procedure for BTiltGauge.
 */
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

/**
 * @brief Sets the threshold enabled parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BTiltGauge::setThresholdEnabled(bool enable)
{
    m_thresholdEnabled=enable;
    update();
}

/**
 * @brief Sets the numeric indicator enabled parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BTiltGauge::setNumericIndicatorEnabled(bool enable)
{
    m_numericIndicatorEnabled=enable;
    update();
}

/**
 * @brief Sets the begin valid value parameter and updates internal state.
 * @param beginValue Input parameter specifying beginValue (double).
 */
void BTiltGauge::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
    update();
}

/**
 * @brief Sets the end valid value parameter and updates internal state.
 * @param endValue Input parameter specifying endValue (double).
 */
void BTiltGauge::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
    update();
}

/**
 * @brief Sets the enable valid window parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BTiltGauge::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
    update();
}

/**
 * @brief Sets the begin warning value parameter and updates internal state.
 * @param beginValue Input parameter specifying beginValue (double).
 */
void BTiltGauge::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
    update();

}

/**
 * @brief Sets the end warning value parameter and updates internal state.
 * @param endValue Input parameter specifying endValue (double).
 */
void BTiltGauge::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
    update();
}

/**
 * @brief Sets the enable warning window parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BTiltGauge::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
    update();
}
