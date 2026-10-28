/**
 * @file dataextractor.h
 * @brief Subsystem module for clustring in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QObject>
#include <QThread>

/**
 * @class DataExtractor
 * @brief High-level controller class for DataExtractor.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class DataExtractor :public QThread
{
public:
    /**
     * @brief Constructs a new DataExtractor instance and initializes subsystem components and UI state.
     */
    DataExtractor();

    // QThread interface
protected:
    void run();

signals:
    void newPatternData(QVector<double>);
};

#endif // DATAEXTRACTOR_H
