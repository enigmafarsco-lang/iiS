/**
 * @file QCustomSeries.cpp
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "QCustomSeries.h"

/**
 * @brief Constructs a new QCustomSeries instance and initializes subsystem components and UI state.
 * @param parent Input parameter specifying parent (QObject).
 */
QCustomSeries::QCustomSeries(QObject *parent)
    : QObject(parent)
{
    setSeriesColor(Qt::yellow);
    setSeriesName("");
    setVisible(true);
}

/**
 * @brief Constructs a new QCustomSeries instance and initializes subsystem components and UI state.
 * @param newSeries Input parameter specifying newSeries (const QCustomSeries&).
 */
QCustomSeries::QCustomSeries(const QCustomSeries& newSeries)
{
    m_inputSeries = newSeries.m_inputSeries;
    m_seriesColor = newSeries.m_seriesColor;
    m_visible = newSeries.m_visible;
    m_seriesName = newSeries.m_seriesName;
}

/**
 * @brief Constructs a new QCustomSeries instance and initializes subsystem components and UI state.
 * @param newSeries Input parameter specifying newSeries (QCustomSeries*).
 */
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

/**
 * @brief Destructor for QCustomSeries, releasing allocated hardware resources, timers, and buffers.
 */
QCustomSeries::~QCustomSeries()
{
}

/**
 * @brief Executes the add series procedure for QCustomSeries.
 * @param inputSeries Input parameter specifying inputSeries (const QVector<QVector2D>).
 */
void QCustomSeries::addSeries(const QVector<QVector2D> &inputSeries)
{
    m_inputSeries = inputSeries;
}

/**
 * @brief Executes the remove all points procedure for QCustomSeries.
 */
void QCustomSeries::removeAllPoints()
{
    if(m_inputSeries.size())
        m_inputSeries.clear();
}

/**
 * @brief Executes the add points procedure for QCustomSeries.
 * @param inputPoint Input parameter specifying inputPoint (const QVector2D).
 */
void QCustomSeries::addPoints(const QVector2D &inputPoint)
{
    m_inputSeries.append(inputPoint);
}


/**
 * @brief Executes the points vector procedure for QCustomSeries.
 * @return Output value of type QVector<QVector2D>.
 */
QVector<QVector2D> QCustomSeries::pointsVector() const
{
    return m_inputSeries;
}

/**
 * @brief Sets the series color parameter and updates internal state.
 * @param color Input parameter specifying color (const QColor).
 */
void QCustomSeries::setSeriesColor(const QColor &color)
{
    m_seriesColor = color;
}

/**
 * @brief Executes the series color procedure for QCustomSeries.
 * @return Output value of type QColor.
 */
QColor QCustomSeries::seriesColor() const
{
    return m_seriesColor;
}

/**
 * @brief Executes the series size procedure for QCustomSeries.
 * @return Output value of type int.
 */
int QCustomSeries::seriesSize() const
{
    return m_inputSeries.count();
}

/**
 * @brief Sets the visible parameter and updates internal state.
 * @param visible Input parameter specifying visible (bool).
 */
void QCustomSeries::setVisible(bool visible)
{
    m_visible = visible;
}

/**
 * @brief Retrieves the current isible value.
 * @return Output value of type bool.
 */
bool QCustomSeries::isVisible() const
{
    return m_visible;
}

/**
 * @brief Sets the series name parameter and updates internal state.
 * @param name Input parameter specifying name (QString).
 */
void QCustomSeries::setSeriesName(QString name)
{
    m_seriesName = name;
}

/**
 * @brief Retrieves the current series name value.
 * @return Output value of type QString.
 */
QString QCustomSeries::getSeriesName() const
{
    return m_seriesName;
}

/**
 * @brief Executes the min y procedure for QCustomSeries.
 * @return Output value of type float.
 */
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

/**
 * @brief Executes the max y procedure for QCustomSeries.
 * @return Output value of type float.
 */
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

/**
 * @brief Executes the min x procedure for QCustomSeries.
 * @return Output value of type float.
 */
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

/**
 * @brief Executes the max x procedure for QCustomSeries.
 * @return Output value of type float.
 */
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
