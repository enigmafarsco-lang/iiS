// ============================================================================
// File: dataextractor.h
// Subsystem: eLynxSDR clustring
// Description: Subsystem implementation and interface guidance for dataextractor.h.
// ============================================================================

#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QObject>
#include <QThread>

class DataExtractor :public QThread
{
public:
    // Constructor: Initializes DataExtractor subsystem components, default parameters, and state.
    DataExtractor();

    // QThread interface
protected:
    // Executes run routine.
    void run();

signals:
    // Executes new pattern data routine.
    void newPatternData(QVector<double>);
};

#endif // DATAEXTRACTOR_H
