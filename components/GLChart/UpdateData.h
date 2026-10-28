/**
 * @file UpdateData.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef UPDATEDATA_H
#define UPDATEDATA_H

#include <QThread>
#include "components/GLChart/QGLchart.h"

/**
 * @class UpdateData
 * @brief High-level controller class for UpdateData.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class UpdateData : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new UpdateData instance and initializes subsystem components and UI state.
     * @param chart Input parameter specifying chart (QGLchart).
     */
    UpdateData(QGLchart *chart);
    /**
     * @brief Executes the run procedure for UpdateData.
     */
    void run();

    QGLchart *chart;
    bool m_clearData;
    bool m_pause;
    bool m_maxHold;
    bool m_maskingData;
    bool m_updateMasking;

    QVector2D m_maskingRange;
    QVector<int> upper;
    QVector<int> lower;
    QVector<QVector2D> maskData;

    QCustomSeries *maxHoldSeries;
    /**
     * @brief Calculates and returns the mathematical calculate max hold formulation.
     * @param input Input parameter specifying input (QCustomSeries).
     */
    void calculateMaxHold(QCustomSeries input);

    /**
     * @brief Executes the spectrum update data procedure for UpdateData.
     */
    void spectrumUpdateData();
    /**
     * @brief Executes the waterfall update data procedure for UpdateData.
     */
    void waterfallUpdateData();
    /**
     * @brief Executes the scatter update data procedure for UpdateData.
     */
    void scatterUpdateData();
    /**
     * @brief Executes the max hold procedure for UpdateData.
     * @param state Input parameter specifying state (bool).
     */
    void maxHold(bool state);
    /**
     * @brief Executes the masking data procedure for UpdateData.
     * @param state Input parameter specifying state (bool).
     * @param range Input parameter specifying range (QVector<QVector2D>).
     * @param 0}} Input parameter specifying 0}} (0}}).
     */
    void maskingData(bool state, QVector<QVector2D> range={{0, 0}});
    /**
     * @brief Calculates and returns the mathematical calculate mask data formulation.
     * @param point Input parameter specifying point (QVector2D).
     */
    void calculateMaskData(QVector2D point);

signals:
    /**
     * @brief Updates and synchronizes the update data data stream and visual indicators.
     * @param a Input parameter specifying a (QCustomSeries).
     */
    void updateData(QCustomSeries a);
    /**
     * @brief Updates and synchronizes the update data data stream and visual indicators.
     * @param a Input parameter specifying a (QCustomSeries).
     */
    void updateData(QCustomSeries *a);
    /**
     * @brief Executes the clear data procedure for UpdateData.
     * @param state Input parameter specifying state (bool).
     */
    void clearData(bool state);

public slots:
    /**
     * @brief Executes the clear data slot procedure for UpdateData.
     * @param state Input parameter specifying state (bool).
     */
    void clearDataSlot(bool state);
    /**
     * @brief Executes the pause slot procedure for UpdateData.
     * @param state Input parameter specifying state (bool).
     */
    void pauseSlot(bool state);


};

#endif // UPDATEDATA_H
