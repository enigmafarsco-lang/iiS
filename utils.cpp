//In the name of God
#include "utils.h"

Utils::Utils(QObject *parent) : QObject(parent)
{

}

//QBitArray Utils::convertQByteArrayToQBitArray(const QByteArray &bytes)
//{
//    QBitArray bits(bytes.count()*8);
//    for(int i=0;i<bytes.count();++i)
//    {
//        for(int b=0;b<8;b++)
//        {
//            bits.setBit(i*8+b,bytes.at(i)&(1<<(7-b)));
//        }
//    }

//    return bits;
//}
QBitArray Utils::convertQByteToQBit(const QByteArray &bytes)
{
    QBitArray bits(bytes.count()*8);
    for(int i=0;i<bytes.count();++i)
    {
        for(int b=0;b<8;b++)
        {
            bits.setBit(i*8+b,bytes.at(i)&(1<<(7-b)));
        }
    }

    return bits;
}
QByteArray Utils::convertQBitToQBYte(const QBitArray &bits)
{
    QByteArray bytes;

    if(bits.count()%8 !=0)
    {

        qDebug()<<"bits count must be multiple of 8";

        return bytes;//must throw Exception
    }
    bytes.resize(bits.count()/8);
    bytes.fill(0);

    for(int b=0; b<bits.count();++b)
    {
        bytes[b/8]= (bytes.at(b/8) | ((bits[b] ? 1:0)<<(7-(b%8))));
    }

    return bytes;
}

char Utils::AND_TowBytes(const char &b1,const char &b2)
{
    QByteArray bytes_1;
    bytes_1.append(b1);
    QByteArray bytes_2;
    bytes_2.append(b2);

    QBitArray bits1 =  Utils::convertQByteToQBit(bytes_1);
    QBitArray bits2 = Utils::convertQByteToQBit(bytes_2);

    QBitArray bits3 = bits1 & bits2;

    QByteArray result;
    result = Utils::convertQBitToQBYte(bits3);



    return  result[0];

}

//int Utils::AND_TowBytes_int(const int &b1,const int &b2)
//{
////    QByteArray bytes_1;
////    bytes_1.append(b1);
////    QByteArray bytes_2;
////    bytes_2.append(b2);

//    QBitArray bits1 =  Utils::convertQByteToQBit(bytes_1);
//    QBitArray bits2 = Utils::convertQByteToQBit(bytes_2);

//    QBitArray bits3 = bits1 & bits2;

//    QByteArray result;
//    result = Utils::convertQBitToQBYte(bits3);



//    return  result[0];

//}

char Utils::OR_TowBytes(const char &b1,const char &b2)
{
    QByteArray bytes_1;
    bytes_1.append(b1);
    QByteArray bytes_2;
    bytes_2.append(b2);

    QBitArray bits1 =  Utils::convertQByteToQBit(bytes_1);
    QBitArray bits2 = Utils::convertQByteToQBit(bytes_2);

    QBitArray bits3 = bits1 | bits2;

    QByteArray result;
    result = Utils::convertQBitToQBYte(bits3);


    return  result[0];
}

char Utils::XOR_TowBytes(const char &b1,const char &b2)
{
    QByteArray bytes_1;
    bytes_1.append(b1);
    QByteArray bytes_2;
    bytes_2.append(b2);

    QBitArray bits1 =  Utils::convertQByteToQBit(bytes_1);
    QBitArray bits2 = Utils::convertQByteToQBit(bytes_2);

    QBitArray bits3 = bits1 ^ bits2;

    QByteArray result;
    result = Utils::convertQBitToQBYte(bits3);


    return  result[0];
}


bool Utils::compare_TowBytes(const char &b1, const char &b2)
{
    QByteArray bytes_1;
    bytes_1.append(b1);
    QByteArray bytes_2;
    bytes_2.append(b2);
    bool result = false;

    QBitArray bits1 =  Utils::convertQByteToQBit(bytes_1);
    QBitArray bits2 = Utils::convertQByteToQBit(bytes_2);

    if(bits1 == bits2)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return  result;
}


