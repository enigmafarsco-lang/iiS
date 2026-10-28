// ============================================================================
// File: QCustomSeries.cpp
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for QCustomSeries.cpp.
// ============================================================================

#include "QCustomSeries.h"

// Constructor: Initializes QCustomSeries subsystem components, default parameters, and state.
QCustomSeries::QCustomSeries(QObject *parent)
    : QObject(parent)
{
    setSeriesColor(Qt::yellow);
    setSeriesName("");
    setVisible(true);
}

// Constructor: Initializes QCustomSeries subsystem components, default parameters, and state.
QCustomSeries::QCustomSeries(const QCustomSeries& newSeries)
{
    m_inputSeries = newSeries.m_inputSeries;
    m_seriesColor = newSeries.m_seriesColor;
    m_visible = newSeries.m_visible;
    m_seriesName = newSeries.m_seriesName;
}

// Constructor: Initializes QCustomSeries subsystem components, default parameters, and state.
QCustomSeries::QCustomSeries(QCustomSeries* newSeries)
{
    m_inputSeries = newSeries->m_inputSeries;
    m_seriesColor = newSeries->m_seriesColor;
    m_visible = newSeries->m_visible;
    m_seriesName = newSeries->m_seriesName;
}

QCustomSeries QCustomSeries::operator=(const QCustomSeries& newSeries)
{
    return QCustomSeries(newSeries);
}

// Destructor: Cleans up QCustomSeries allocated resources, timers, and buffers.
QCustomSeries::~QCustomSeries()
{
}

// Executes add series routine.
void QCustomSeries::addSeries(const QVector<QVector2D> &inputSeries)
{
    m_inputSeries = inputSeries;
}

// Executes remove all points routine.
void QCustomSeries::removeAllPoints()
{
    if(m_inputSeries.size())
        m_inputSeries.clear();
}

// Executes add points routine.
void QCustomSeries::addPoints(const QVector2D &inputPoint)
{
    m_inputSeries.append(inputPoint);
}


// Executes points vector routine.
QVector<QVector2D> QCustomSeries::pointsVector() const
{
    return m_inputSeries;
}

// Sets series color parameter and updates internal configuration.
void QCustomSeries::setSeriesColor(const QColor &color)
{
    m_seriesColor = color;
}

// Executes series color routine.
QColor QCustomSeries::seriesColor() const
{
    return m_seriesColor;
}

// Executes series size routine.
int QCustomSeries::seriesSize() const
{
    return m_inputSeries.count();
}

// Sets visible parameter and updates internal configuration.
void QCustomSeries::setVisible(bool visible)
{
    m_visible = visible;
}

// Retrieves current isible value.
bool QCustomSeries::isVisible() const
{
    return m_visible;
}

// Sets series name parameter and updates internal configuration.
void QCustomSeries::setSeriesName(QString name)
{
    m_seriesName = name;
}

// Retrieves current series name value.
QString QCustomSeries::getSeriesName() const
{
    return m_seriesName;
}

// Executes min y routine.
float QCustomSeries::minY()
{
    float min_Y = 1e100;
    for (int i = 0; i < m_inputSeries.count(); i++)
    {
        if (min_Y > m_inputSeries.at(i).y())
            min_Y = m_inputSeries.at(i).y();
    }
    return min_Y;
}

// Executes max y routine.
float QCustomSeries::maxY()
{
    float max_Y = -1e100;
    for (int i = 0; i < m_inputSeries.count(); i++)
    {
        if (max_Y < m_inputSeries.at(i).y())
            max_Y = m_inputSeries.at(i).y();
    }
    return max_Y;
}

// Executes min x routine.
float QCustomSeries::minX()
{
    float min_X = 1e100;
    for (int i = 0; i < m_inputSeries.count(); i++)
    {
        if (min_X > m_inputSeries.at(i).x())
            min_X = m_inputSeries.at(i).x();
    }
    return min_X;
}

// Executes max x routine.
float QCustomSeries::maxX()
{
    float max_X = -1e100;
    for (int i = 0; i < m_inputSeries.count(); i++)
    {
        if (max_X < m_inputSeries.at(i).x())
            max_X = m_inputSeries.at(i).x();
    }
    return max_X;
}
