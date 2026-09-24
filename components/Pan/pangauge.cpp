#include "pangauge.h"
#include <QtGlobal>
#include <QtGui>
#include <QPainter>
#include <QFont>
#include "constants/project_constans.h"

BPanGauge::BPanGauge(QWidget *parent)
    : QWidget(parent)
{
    init_panGauge();
}

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

void BPanGauge::setValue(int value)
{
   setValue((double)value);
   update();
}

void BPanGauge::setMinValue(double value)
{
   m_minValue=value;
   update();
}

void BPanGauge::setMinValue(int value)
{
  setMinValue((double)value);
}


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

void BPanGauge::setMaxValue(int value)
{
  setMaxValue((double)value);
}


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

void BPanGauge::setThreshold(int value)
{
  setThreshold((double)value);
}

void BPanGauge::setPrecision(int precision)
{
   m_precision=precision;
   update();
}

void BPanGauge::setPrecisionNumeric(int precision)
{
   m_precisionNumeric=precision;
   update();
}

void BPanGauge::setUnits(QString units)
{
  m_units=units;
  update();
}

void BPanGauge::setLabel(QString label)
{
    m_label=label;
}

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

void BPanGauge::setSteps(int nSteps)
{
    m_steps=nSteps;
    update();
}

void BPanGauge::setStartAngle(double value)
{
    m_startAngle=value;
    update();
}

void BPanGauge::setEndAngle(double value)
{
    m_endAngle=value;
    update();
}

void BPanGauge::setForeground(QColor newForeColor)
{
    m_foreground=newForeColor;
    update();
}

void BPanGauge::setBackground(QColor newBackColor)
{
    m_background=newBackColor;
    update();
}


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

void BPanGauge::setThresholdEnabled(bool enable)
{
  m_thresholdEnabled=enable;
  update();
}

void BPanGauge::setNumericIndicatorEnabled(bool enable)
{
  m_numericIndicatorEnabled=enable;
  update();
}

void BPanGauge::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
    update();
}

void BPanGauge::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
    update();
}

void BPanGauge::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
    update();
}

void BPanGauge::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
    update();

}

void BPanGauge::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
    update();
}

void BPanGauge::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
    update();
}
