#ifndef UPDATEDATA_H
#define UPDATEDATA_H

#include <QThread>
#include "components/GLChart/QGLchart.h"

class UpdateData : public QThread
{
    Q_OBJECT

public:
    UpdateData(QGLchart *chart);
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
    void calculateMaxHold(QCustomSeries input);

    void spectrumUpdateData();
    void waterfallUpdateData();
    void scatterUpdateData();
    void maxHold(bool state);
    void maskingData(bool state, QVector<QVector2D> range={{0, 0}});
    void calculateMaskData(QVector2D point);

signals:
    void updateData(QCustomSeries a);
    void updateData(QCustomSeries *a);
    void clearData(bool state);

public slots:
    void clearDataSlot(bool state);
    void pauseSlot(bool state);


};

#endif // UPDATEDATA_H
