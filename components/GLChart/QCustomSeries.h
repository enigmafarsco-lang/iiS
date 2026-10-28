/**
 * @file QCustomSeries.h
 * @brief Subsystem module for GLChart in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
#include <QVector2D>

/**
 * @class QCustomSeries
 * @brief High-level controller class for QCustomSeries.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class QCustomSeries : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new QCustomSeries instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QObject).
     */
    QCustomSeries(QObject *parent = nullptr);
    /**
     * @brief Constructs a new QCustomSeries instance and initializes subsystem components and UI state.
     * @param QCustomSeries& Input parameter specifying QCustomSeries& (const).
     */
    QCustomSeries(const QCustomSeries&);
    /**
     * @brief Constructs a new QCustomSeries instance and initializes subsystem components and UI state.
     * @param QCustomSeries* Input parameter specifying QCustomSeries* (QCustomSeries*).
     */
    QCustomSeries(QCustomSeries*);
    /**
     * @brief Destructor for QCustomSeries, releasing allocated hardware resources, timers, and buffers.
     */
    ~QCustomSeries();

    QCustomSeries operator=(const QCustomSeries&);

    /**
     * @brief Executes the add series procedure for QCustomSeries.
     * @param inputSeries Input parameter specifying inputSeries (const QVector<QVector2D>).
     */
    void addSeries(const QVector<QVector2D> &inputSeries);
    /**
     * @brief Executes the add points procedure for QCustomSeries.
     * @param inputPoints Input parameter specifying inputPoints (const QVector2D).
     */
    void addPoints(const QVector2D &inputPoints);


    /**
     * @brief Executes the remove all points procedure for QCustomSeries.
     */
    void removeAllPoints();

    /**
     * @brief Sets the series color parameter and updates internal state.
     * @param color Input parameter specifying color (const QColor).
     */
    void setSeriesColor(const QColor &color);
    /**
     * @brief Executes the series color procedure for QCustomSeries.
     * @return Output value of type QColor.
     */
    QColor seriesColor() const;

    /**
     * @brief Executes the series size procedure for QCustomSeries.
     * @return Output value of type int.
     */
    int seriesSize() const;

    /**
     * @brief Sets the visible parameter and updates internal state.
     * @param visible Input parameter specifying visible (bool).
     */
    void setVisible(bool visible = true);
    /**
     * @brief Retrieves the current isible value.
     * @return Output value of type bool.
     */
    bool isVisible() const;

    /**
     * @brief Sets the series name parameter and updates internal state.
     * @param name Input parameter specifying name (QString).
     */
    void setSeriesName(QString name);
    /**
     * @brief Retrieves the current series name value.
     * @return Output value of type QString.
     */
    QString getSeriesName() const;

    /**
     * @brief Executes the points vector procedure for QCustomSeries.
     * @return Output value of type QVector<QVector2D>.
     */
    QVector<QVector2D> pointsVector() const;

    /**
     * @brief Executes the min y procedure for QCustomSeries.
     * @return Output value of type float.
     */
    float minY();
    /**
     * @brief Executes the max y procedure for QCustomSeries.
     * @return Output value of type float.
     */
    float maxY();
    /**
     * @brief Executes the min x procedure for QCustomSeries.
     * @return Output value of type float.
     */
    float minX();
    /**
     * @brief Executes the max x procedure for QCustomSeries.
     * @return Output value of type float.
     */
    float maxX();

private:
    QVector<QVector2D>			m_inputSeries;
    QColor						m_seriesColor;
    bool						m_visible;
    QString						m_seriesName;
};
