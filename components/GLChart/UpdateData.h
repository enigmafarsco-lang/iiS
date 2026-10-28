// ============================================================================
// File: UpdateData.h
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for UpdateData.h.
// ============================================================================

#ifndef UPDATEDATA_H
#define UPDATEDATA_H

#include <QThread>
#include "components/GLChart/QGLchart.h"

class UpdateData : public QThread
{
    Q_OBJECT

public:
    // Constructor: Initializes UpdateData subsystem components, default parameters, and state.
    UpdateData(QGLchart *chart);
    // Executes run routine.
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
    // Computes mathematical calculate max hold calculation.
    void calculateMaxHold(QCustomSeries input);

    // Executes spectrum update data routine.
    void spectrumUpdateData();
    // Executes waterfall update data routine.
    void waterfallUpdateData();
    // Executes scatter update data routine.
    void scatterUpdateData();
    // Executes max hold routine.
    void maxHold(bool state);
    // Executes masking data routine.
    void maskingData(bool state, QVector<QVector2D> range={{0, 0}});
    // Computes mathematical calculate mask data calculation.
    void calculateMaskData(QVector2D point);

signals:
    // Updates and synchronizes update data data stream.
    void updateData(QCustomSeries a);
    // Updates and synchronizes update data data stream.
    void updateData(QCustomSeries *a);
    // Executes clear data routine.
    void clearData(bool state);

public slots:
    // Executes clear data slot routine.
    void clearDataSlot(bool state);
    // Executes pause slot routine.
    void pauseSlot(bool state);


};

#endif // UPDATEDATA_H
