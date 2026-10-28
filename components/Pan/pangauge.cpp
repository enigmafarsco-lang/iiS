// ============================================================================
// File: pangauge.cpp
// Subsystem: eLynxSDR Pan
// Description: Subsystem implementation and interface guidance for pangauge.cpp.
// ============================================================================

#include "pangauge.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>
#include "constants/project_constans.h"

// Constructor: Initializes BPanGauge subsystem components, state, and UI bindings.
BPanGauge::BPanGauge(QWidget *parent)
    : QWidget(parent)
{
    init_panGauge();
}

// Initializes init pan gauge for BPanGauge.
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

// Sets value parameter and updates internal configuration.
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

// Sets value parameter and updates internal configuration.
void BPanGauge::setValue(int value)
{
   setValue((double)value);
   update();
}

// Sets min value parameter and updates internal configuration.
void BPanGauge::setMinValue(double value)
{
   m_minValue=value;
   update();
}

// Sets min value parameter and updates internal configuration.
void BPanGauge::setMinValue(int value)
{
  setMinValue((double)value);
}


// Sets max value parameter and updates internal configuration.
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

// Sets max value parameter and updates internal configuration.
void BPanGauge::setMaxValue(int value)
{
  setMaxValue((double)value);
}


// Sets threshold parameter and updates internal configuration.
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

// Sets threshold parameter and updates internal configuration.
void BPanGauge::setThreshold(int value)
{
  setThreshold((double)value);
}

// Sets precision parameter and updates internal configuration.
void BPanGauge::setPrecision(int precision)
{
   m_precision=precision;
   update();
}

// Sets precision numeric parameter and updates internal configuration.
void BPanGauge::setPrecisionNumeric(int precision)
{
   m_precisionNumeric=precision;
   update();
}

// Sets units parameter and updates internal configuration.
void BPanGauge::setUnits(QString units)
{
  m_units=units;
  update();
}

// Sets label parameter and updates internal configuration.
void BPanGauge::setLabel(QString label)
{
    m_label=label;
}

// Executes paint event routine.
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

// Sets steps parameter and updates internal configuration.
void BPanGauge::setSteps(int nSteps)
{
    m_steps=nSteps;
    update();
}

// Sets start angle parameter and updates internal configuration.
void BPanGauge::setStartAngle(double value)
{
    m_startAngle=value;
    update();
}

// Sets end angle parameter and updates internal configuration.
void BPanGauge::setEndAngle(double value)
{
    m_endAngle=value;
    update();
}

// Sets foreground parameter and updates internal configuration.
void BPanGauge::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}

// Sets background parameter and updates internal configuration.
void BPanGauge::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


// Executes threshold manager routine.
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

// Sets threshold enabled parameter and updates internal configuration.
void BPanGauge::setThresholdEnabled(bool enable)
{
  m_thresholdEnabled=enable;
  update();
}

// Sets numeric indicator enabled parameter and updates internal configuration.
void BPanGauge::setNumericIndicatorEnabled(bool enable)
{
  m_numericIndicatorEnabled=enable;
  update();
}

// Sets begin valid value parameter and updates internal configuration.
void BPanGauge::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
    update();
}

// Sets end valid value parameter and updates internal configuration.
void BPanGauge::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
    update();
}

// Sets enable valid window parameter and updates internal configuration.
void BPanGauge::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
    update();
}

// Sets begin warning value parameter and updates internal configuration.
void BPanGauge::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
    update();

}

// Sets end warning value parameter and updates internal configuration.
void BPanGauge::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
    update();
}

// Sets enable warning window parameter and updates internal configuration.
void BPanGauge::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
    update();
}
