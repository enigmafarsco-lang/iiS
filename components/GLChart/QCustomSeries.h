// ============================================================================
// File: QCustomSeries.h
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for QCustomSeries.h.
// ============================================================================

#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
#include <QVector2D>

class QCustomSeries : public QObject
{
    Q_OBJECT

public:
    // Constructor: Initializes QCustomSeries subsystem components, default parameters, and state.
    QCustomSeries(QObject *parent = nullptr);
    // Constructor: Initializes QCustomSeries subsystem components, default parameters, and state.
    QCustomSeries(const QCustomSeries&);
    // Constructor: Initializes QCustomSeries subsystem components, default parameters, and state.
    QCustomSeries(QCustomSeries*);
    // Destructor: Cleans up QCustomSeries resources, timers, and allocated memory.
    ~QCustomSeries();

    QCustomSeries operator=(const QCustomSeries&);

    // Executes add series routine.
    void addSeries(const QVector<QVector2D> &inputSeries);
    // Executes add points routine.
    void addPoints(const QVector2D &inputPoints);


    // Executes remove all points routine.
    void removeAllPoints();

    // Sets series color value and updates internal subsystem configuration.
    void setSeriesColor(const QColor &color);
    // Executes series color routine.
    QColor seriesColor() const;

    // Executes series size routine.
    int seriesSize() const;

    // Sets visible value and updates internal subsystem configuration.
    void setVisible(bool visible = true);
    // Retrieves current isible state.
    bool isVisible() const;

    // Sets series name value and updates internal subsystem configuration.
    void setSeriesName(QString name);
    // Retrieves current series name state.
    QString getSeriesName() const;

    // Executes points vector routine.
    QVector<QVector2D> pointsVector() const;

    // Executes min y routine.
    float minY();
    // Executes max y routine.
    float maxY();
    // Executes min x routine.
    float minX();
    // Executes max x routine.
    float maxX();

private:
    QVector<QVector2D>			m_inputSeries;
    QColor						m_seriesColor;
    bool						m_visible;
    QString						m_seriesName;
};
