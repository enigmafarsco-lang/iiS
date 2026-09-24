#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QByteArray>
#include <QBitArray>
#include <QDebug>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

public:
    static QBitArray convertQByteToQBit(const QByteArray&);
    static QByteArray convertQBitToQBYte(const QBitArray&);
    static char AND_TowBytes(const char &b1,const char &b2);
//    static int AND_TowBytes_int(const int &b1,const int &b2);
    static char OR_TowBytes(const char &b1,const char &b2);
    static char XOR_TowBytes(const char &b1,const char &b2);
    static bool compare_TowBytes(const char &b1,const char &b2);



signals:



};

#endif // UTILS_H
