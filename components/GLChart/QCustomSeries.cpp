#include "QCustomSeries.h"

QCustomSeries::QCustomSeries(QObject *parent)
    : QObject(parent)
{
    setSeriesColor(Qt::yellow);
    setSeriesName("");
    setVisible(true);
}

QCustomSeries::QCustomSeries(const QCustomSeries& newSeries)
{
    m_inputSeries = newSeries.m_inputSeries;
    m_seriesColor = newSeries.m_seriesColor;
    m_visible = newSeries.m_visible;
    m_seriesName = newSeries.m_seriesName;
}

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

QCustomSeries::~QCustomSeries()
{
}

void QCustomSeries::addSeries(const QVector<QVector2D> &inputSeries)
{
    m_inputSeries = inputSeries;
}

void QCustomSeries::removeAllPoints()
{
    if(m_inputSeries.size())
        m_inputSeries.clear();
}

void QCustomSeries::addPoints(const QVector2D &inputPoint)
{
    m_inputSeries.append(inputPoint);
}


QVector<QVector2D> QCustomSeries::pointsVector() const
{
    return m_inputSeries;
}

void QCustomSeries::setSeriesColor(const QColor &color)
{
    m_seriesColor = color;
}

QColor QCustomSeries::seriesColor() const
{
    return m_seriesColor;
}

int QCustomSeries::seriesSize() const
{
    return m_inputSeries.count();
}

void QCustomSeries::setVisible(bool visible)
{
    m_visible = visible;
}

bool QCustomSeries::isVisible() const
{
    return m_visible;
}

void QCustomSeries::setSeriesName(QString name)
{
    m_seriesName = name;
}

QString QCustomSeries::getSeriesName() const
{
    return m_seriesName;
}

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
