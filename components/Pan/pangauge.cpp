/**
 * @file pangauge.cpp
 * @brief Subsystem module for Pan in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "pangauge.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>
#include "constants/project_constans.h"

/**
 * @brief Constructs a new BPanGauge instance and initializes subsystem components and UI state.
 * @param parent Input parameter specifying parent (QWidget).
 */
BPanGauge::BPanGauge(QWidget *parent)
    : QWidget(parent)
{
    init_panGauge();
}

/**
 * @brief Initializes and configures the init pan gauge for the BPanGauge subsystem.
 */
void BPanGauge::init_panGauge()
{
    m_precision=0;
    this->setFont(QFont("Tahoma", 5));

    // m_steps=10;
    m_steps=10; // Edited by B.Hedayati @ 1398-06-25-1615

    m_thresholdFlag=false;
    m_foreground=QColor(Qt::black);
    m_background=QColor(Qt::white);
    m_foreground=QColor(Qt::white);
    m_background=QColor(Qt::black);

    m_thresholdEnabled=true;
    m_numericIndicatorEnabled=false;

    setMinValue(0);
    setMaxValue(360);
    setValue(55);

    setStartAngle(90); // Edited by B.Hedayati @ 1398-06-25-1615 90
    setEndAngle(-270); // Edited by B.Hedayati @ 1398-06-25-1615 -270
    setMinimumSize(QSize(50,50));
    setLabel("Pan"); // Edited by B.Hedayati @ 1398-06-25-1615
    setThreshold(350); // Edited by B.Hedayati @ 1398-06-25-1615

    setSteps(12); // Added by B.Hedayati @ 1398-06-25-1615

//    setBeginWarningValue(50);
//    setEndWarningValue(100);
}

void BPanGauge::setValue(double value)
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
void BPanGauge::setValue(int value)
{
   setValue((double)value);
   update();
}

/**
 * @brief Sets the min value parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BPanGauge::setMinValue(double value)
{
   m_minValue=value;
   update();
}

/**
 * @brief Sets the min value parameter and updates internal state.
 * @param value Input parameter specifying value (int).
 */
void BPanGauge::setMinValue(int value)
{
  setMinValue((double)value);
}


/**
 * @brief Sets the max value parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BPanGauge::setMaxValue(double value)
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
void BPanGauge::setMaxValue(int value)
{
  setMaxValue((double)value);
}


/**
 * @brief Sets the threshold parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BPanGauge::setThreshold(double value)
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
void BPanGauge::setThreshold(int value)
{
  setThreshold((double)value);
}

/**
 * @brief Sets the precision parameter and updates internal state.
 * @param precision Input parameter specifying precision (int).
 */
void BPanGauge::setPrecision(int precision)
{
   m_precision=precision;
   update();
}

/**
 * @brief Sets the precision numeric parameter and updates internal state.
 * @param precision Input parameter specifying precision (int).
 */
void BPanGauge::setPrecisionNumeric(int precision)
{
   m_precisionNumeric=precision;
   update();
}

/**
 * @brief Sets the units parameter and updates internal state.
 * @param units Input parameter specifying units (QString).
 */
void BPanGauge::setUnits(QString units)
{
  m_units=units;
  update();
}

/**
 * @brief Sets the label parameter and updates internal state.
 * @param label Input parameter specifying label (QString).
 */
void BPanGauge::setLabel(QString label)
{
    m_label=label;
}

/**
 * @brief Executes the paint event procedure for BPanGauge.
 * @param event Input parameter specifying event (QPaintEvent).
 */
void BPanGauge::paintEvent(QPaintEvent *event)
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
void BPanGauge::setSteps(int nSteps)
{
    m_steps=nSteps;
    update();
}

/**
 * @brief Sets the start angle parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BPanGauge::setStartAngle(double value)
{
    m_startAngle=value;
    update();
}

/**
 * @brief Sets the end angle parameter and updates internal state.
 * @param value Input parameter specifying value (double).
 */
void BPanGauge::setEndAngle(double value)
{
    m_endAngle=value;
    update();
}

/**
 * @brief Sets the foreground parameter and updates internal state.
 * @param newForeColor Input parameter specifying newForeColor (QColor).
 */
void BPanGauge::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}

/**
 * @brief Sets the background parameter and updates internal state.
 * @param newBackColor Input parameter specifying newBackColor (QColor).
 */
void BPanGauge::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


/**
 * @brief Executes the threshold manager procedure for BPanGauge.
 */
void BPanGauge::thresholdManager()
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
void BPanGauge::setThresholdEnabled(bool enable)
{
  m_thresholdEnabled=enable;
  update();
}

/**
 * @brief Sets the numeric indicator enabled parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BPanGauge::setNumericIndicatorEnabled(bool enable)
{
  m_numericIndicatorEnabled=enable;
  update();
}

/**
 * @brief Sets the begin valid value parameter and updates internal state.
 * @param beginValue Input parameter specifying beginValue (double).
 */
void BPanGauge::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
    update();
}

/**
 * @brief Sets the end valid value parameter and updates internal state.
 * @param endValue Input parameter specifying endValue (double).
 */
void BPanGauge::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
    update();
}

/**
 * @brief Sets the enable valid window parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BPanGauge::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
    update();
}

/**
 * @brief Sets the begin warning value parameter and updates internal state.
 * @param beginValue Input parameter specifying beginValue (double).
 */
void BPanGauge::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
    update();

}

/**
 * @brief Sets the end warning value parameter and updates internal state.
 * @param endValue Input parameter specifying endValue (double).
 */
void BPanGauge::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
    update();
}

/**
 * @brief Sets the enable warning window parameter and updates internal state.
 * @param enable Input parameter specifying enable (bool).
 */
void BPanGauge::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
    update();
}
