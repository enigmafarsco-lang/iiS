#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
#include <QVector2D>

class QCustomSeries : public QObject
{
    Q_OBJECT

public:
    QCustomSeries(QObject *parent = nullptr);
    QCustomSeries(const QCustomSeries&);
    QCustomSeries(QCustomSeries*);
    ~QCustomSeries();

    QCustomSeries operator=(const QCustomSeries&);

    void addSeries(const QVector<QVector2D> &inputSeries);
    void addPoints(const QVector2D &inputPoints);


    void removeAllPoints();

    void setSeriesColor(const QColor &color);
    QColor seriesColor() const;

    int seriesSize() const;

    void setVisible(bool visible = true);
    bool isVisible() const;

    void setSeriesName(QString name);
    QString getSeriesName() const;

    QVector<QVector2D> pointsVector() const;

    float minY();
    float maxY();
    float minX();
    float maxX();

private:
    QVector<QVector2D>			m_inputSeries;
    QColor						m_seriesColor;
    bool						m_visible;
    QString						m_seriesName;
};
