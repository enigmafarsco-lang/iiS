/**
 * @file utils.h
 * @brief Bitwise Manipulation & Low-Level Byte Conversion Utilities.
 *
 * Provides static helper routines for bit-level bitmasking, bit-to-byte packing,
 * logical bitwise AND/OR/XOR operations, and raw register checksum validations.
 *
 * @author eLynxSDR Development Team
 * @version 0.0.2
 */

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QByteArray>
#include <QBitArray>
#include <QDebug>

/**
 * @class Utils
 * @brief Static utility functions for binary protocol serialization and bit conversions.
 */
class Utils : public QObject
{
    Q_OBJECT

public:
    explicit Utils(QObject *parent = nullptr);

    /**
     * @brief Converts a QByteArray into a bit-level QBitArray representation.
     * @param bytes Input byte array.
     * @return Resulting bit array.
     */
    static QBitArray convertQByteToQBit(const QByteArray &bytes);

    /**
     * @brief Converts a QBitArray back into a packed QByteArray.
     * @param bits Input bit array.
     * @return Resulting byte array.
     */
    static QByteArray convertQBitToQBYte(const QBitArray &bits);

    /**
     * @brief Performs bitwise AND operation on two 8-bit bytes.
     */
    static char AND_TowBytes(const char &b1, const char &b2);

    /**
     * @brief Performs bitwise OR operation on two 8-bit bytes.
     */
    static char OR_TowBytes(const char &b1, const char &b2);

    /**
     * @brief Performs bitwise XOR operation on two 8-bit bytes.
     */
    static char XOR_TowBytes(const char &b1, const char &b2);

    /**
     * @brief Compares two bytes for equality.
     */
    static bool compare_TowBytes(const char &b1, const char &b2);
};

#endif // UTILS_H
