#ifndef TX_CALIBRATION_H
#define TX_CALIBRATION_H

// ---------------------------------------------------------------------------
// TX calibration - the Pa / Pc / Pb power model.
//
//     Pa = P (attenuation): the value typed into the P (attenuation)
//          controls ("pAttenuation").
//     Pc = P calibration: the "calibrated mismatch power" entered in the
//          receiver's Calibration tab ("pCalibration", default 0 dB).
//     Pb = Pa + Pc: the power actually SENT TO THE BOARD ("board power").
//          The board is commanded with Pb as attenuation (written to the
//          "hardwaregain" attribute as the signed gain -Pb), and Pb is the
//          number shown everywhere the TX power appears in the software.
//
// The constraint is Pb >= 0.  With a negative Pc the user must raise Pa so
// that Pa + Pc stays non-negative (for example Pc = -6 dB needs Pa >= 6 dB);
// the warning states that P must be at least -Pc dB.  The software warns
// once per invalid (Pa, Pc) pair.
// ---------------------------------------------------------------------------

#include <QObject>
#include <QMessageBox>
#include <QWidget>
#include <QtGlobal>
#include <cmath>

namespace TxCalibration
{

// Pc: current P calibration in dB (default 0, kept in sync with the
// dsbTxCalib spinbox of the Calibration tab).
inline double &offsetDb()
{
    static double v = 0.0;
    return v;
}

inline void setOffsetDb(double v)
{
    offsetDb() = v;
}

// Pb = Pa + Pc: the calibrated power sent to the board (and shown wherever
// the TX power appears in the software).
inline double boardP(double pa)
{
    return pa + offsetDb();
}

// Validate one P (attenuation) value against the calibration.  Returns
// false (after showing the maximum-power warning) when Pb = Pa + Pc is
// negative.  The P value mirror-syncs between the exciter panel and the
// TX1/TX2 attenuation spinboxes, so an identical repeated (Pa, Pc) pair
// warns only once; any different pair warns again.
inline bool checkP(double pa, QWidget *parent = nullptr)
{
    static double lastP = qQNaN();
    static double lastCal = qQNaN();
    const double pc = offsetDb();
    if (boardP(pa) >= 0.0)
    {
        // valid pair: arm the warning for the next invalid combination
        lastP = qQNaN();
        lastCal = qQNaN();
        return true;
    }
    if (pa == lastP && pc == lastCal)
        return false; // already warned for this exact combination
    lastP = pa;
    lastCal = pc;
    QMessageBox::warning(
        parent, QObject::tr("TX Calibration"),
        QObject::tr("Maximum power limit: the power sent to the board "
                    "(Pb = P + calibration) must be >= 0. The P "
                    "(attenuation) must be at least %1 dB with the "
                    "current %2 dB TX calibration.")
            .arg(-pc)
            .arg(pc));
    return false;
}

} // namespace TxCalibration

#endif // TX_CALIBRATION_H
