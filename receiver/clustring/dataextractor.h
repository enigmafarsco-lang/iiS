#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QObject>
#include <QThread>

class DataExtractor :public QThread
{
public:
    DataExtractor();

    // QThread interface
protected:
    void run();

signals:
    void newPatternData(QVector<double>);
};

#endif // DATAEXTRACTOR_H
