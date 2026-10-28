/**
 * @file UpdateData.cpp
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#include "UpdateData.h"

/**
 * @brief Constructs a new UpdateData instance and initializes subsystem components and UI state.
 * @param chart Input parameter specifying chart (QGLchart).
 */
UpdateData::UpdateData(QGLchart *chart)
{
    this->chart = chart;

    m_clearData     = false;
    m_pause         = false;
    m_maxHold       = false;
    m_maskingData   = false;
    m_updateMasking = false;

    maxHoldSeries = new QCustomSeries();
    maxHoldSeries->setSeriesColor(Qt::red);


    //    start();
}

void UpdateData::run()
{
    if(chart->getChartType() == ChartType::Spectrum)
    {
        spectrumUpdateData();
    }
    else if (chart->getChartType() == ChartType::Waterfall)
    {
        waterfallUpdateData();
    }
    else if (chart->getChartType() == ChartType::ScatterPlot)
    {
        scatterUpdateData();
    }
}

/**
 * @brief Executes the spectrum update data procedure for UpdateData.
 */
void UpdateData::spectrumUpdateData()
{
    QCustomSeries a;
    QVector2D v2d;

    a.setSeriesName("a");

    QCustomSeries *maskSeries = new QCustomSeries();
    maskSeries->setSeriesColor(Qt::magenta);

    while(true)
    {
        //        if(m_pause)
        //        {
        //            continue;
        //        }
        if(m_clearData)
        {
            a.addPoints({0, 0});
            a.addPoints({0, 1});
            emit(updateData(a));
            a.removeAllPoints();
            msleep(100);
            continue;
        }
        for(int i = 0; i < 10000; i++)
        {
            v2d.setX(8000 + i);
            v2d.setY(20 * log10f(abs( 1.0 / ((RAND_MAX) * qrand()) )) + 100);
            a.addPoints(v2d);

            if(m_maskingData)
            {
                calculateMaskData(v2d);
            }
        }
        if(m_maxHold)
        {
            calculateMaxHold(a);
            emit(updateData(maxHoldSeries));
        }

        emit(updateData(a));

        if(m_maskingData)
        {
            maskSeries->addSeries(maskData);
            emit(updateData(maskSeries));
            maskData.clear();
        }
        msleep(100);
        a.removeAllPoints();
    }

}

/**
 * @brief Executes the waterfall update data procedure for UpdateData.
 */
void UpdateData::waterfallUpdateData()
{
    QCustomSeries a;
    QVector2D v2d;

    while(true)
    {
        //        if(m_pause)
        //            continue;

        if(m_clearData)
        {
            a.addPoints({0, 0});
            a.addPoints({0, 1});
            emit(updateData(a));
            a.removeAllPoints();
            msleep(100);
            continue;
        }
        for(int i = 0; i < 10000; i++)
        {
            v2d.setX(8000 + i);
            v2d.setY(20 * log10f(abs( 1.0 / ((RAND_MAX) * qrand()) )) + 100);
            a.addPoints(v2d);
        }
        msleep(50);
        emit(updateData(a));
        a.removeAllPoints();
    }
}

/**
 * @brief Executes the scatter update data procedure for UpdateData.
 */
void UpdateData::scatterUpdateData()
{
    QCustomSeries a;
    QVector2D v2d;
    a.setSeriesName("a");

    QCustomSeries *redSeries = new QCustomSeries();
    QVector<QVector2D> redData;
    redSeries->setSeriesColor(Qt::red);
    a.setSeriesColor(Qt::cyan);

    int toa = 1;
    while(true)
    {
        if(m_pause)
            continue;

        if(m_clearData)
        {
            a.addPoints({0, 0});
            a.addPoints({0, 1});
            emit(updateData(a));
            a.removeAllPoints();
            msleep(100);
            continue;
        }
        for(int i = 0; i < 10000; i++)
        {
            v2d.setX(toa);
            toa += 200;
            v2d.setY(20 * log10f(abs( 1.0 / ((RAND_MAX) * qrand()) )) + 100);
            if(v2d.y() > -50)
                redData.append(v2d);
            else
                a.addPoints(v2d);
        }
        emit(updateData(a));

        redSeries->addSeries(redData);
        emit(updateData(redSeries));
        redData.clear();

        msleep(100);
        a.removeAllPoints();
    }
}

/**
 * @brief Calculates and returns the mathematical calculate max hold formulation.
 * @param inputSeries Input parameter specifying inputSeries (QCustomSeries).
 */
void UpdateData::calculateMaxHold(QCustomSeries inputSeries)
{
    QVector<QVector2D> inputVector = inputSeries.pointsVector();
    {
        if (maxHoldSeries->seriesSize() == 0)
        {
            maxHoldSeries->addSeries(inputVector);
        }
        else
        {
            QVector<QVector2D> maxHoldVector = maxHoldSeries->pointsVector();
            if (maxHoldVector.size() != inputVector.size())
            {
                maxHoldVector = inputVector;
            }
            for (int i = 0; i < inputVector.size(); i++)
            {
                if (maxHoldVector[i].y() < inputVector[i].y())
                    maxHoldVector[i].setY(inputVector[i].y());
            }
            maxHoldSeries->addSeries(maxHoldVector);
        }
    }
}

/**
 * @brief Executes the clear data slot procedure for UpdateData.
 * @param state Input parameter specifying state (bool).
 */
void UpdateData::clearDataSlot(bool state)
{
    m_clearData = state;
}

/**
 * @brief Executes the pause slot procedure for UpdateData.
 * @param state Input parameter specifying state (bool).
 */
void UpdateData::pauseSlot(bool state)
{
    m_pause = state;
}

/**
 * @brief Executes the max hold procedure for UpdateData.
 * @param state Input parameter specifying state (bool).
 */
void UpdateData::maxHold(bool state)
{
    if(!state)
        chart->removeAllSeries();
    m_maxHold = state;
    maxHoldSeries->removeAllPoints();
}

/**
 * @brief Executes the masking data procedure for UpdateData.
 * @param state Input parameter specifying state (bool).
 * @param range Input parameter specifying range (QVector<QVector2D>).
 */
void UpdateData::maskingData(bool state, QVector<QVector2D> range)
{
    m_updateMasking = true;
    chart->removeAllSeries();

    if(upper.size())
    {
        upper.clear();
        lower.clear();
    }
    for(int i = 0; i < range.size(); i++)
    {
        upper.append(range[i].x() + (range[i].y() / 2));
        lower.append(range[i].x() - (range[i].y() / 2));
    }

    m_maskingData = state;
    m_updateMasking = false;
}

/**
 * @brief Calculates and returns the mathematical calculate mask data formulation.
 * @param point Input parameter specifying point (QVector2D).
 */
void UpdateData::calculateMaskData(QVector2D point)
{
    // for lock in lower and upper that could be change in maskingData
    while(m_updateMasking);

    for(int i = 0; i < lower.size(); i++)
    {
        if((point.x() < upper.at(i)) && (point.x() > lower.at(i)))
            maskData.append(point);
    }
}
