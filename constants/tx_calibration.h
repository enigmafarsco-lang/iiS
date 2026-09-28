#ifndef TX_CALIBRATION_H
#define TX_CALIBRATION_H

// ---------------------------------------------------------------------------
// Phase 6: TX calibration - "calibrated mismatch power" typed in the TX
// Calibration section of the receiver's Calibration tab (default -6 dB).
//
// The number is ADDED to the P (attenuation) value everywhere the TX power
// is commanded or shown in the software:
//
//     effective P = P + TX calibration
//
// The effective P must stay >= 0.  When it would go negative (for example
// calibration -6 dB and P 0) the software warns that the P (attenuation)
// must be at least -calibration dB (6 dB in the example): that is the P
// value giving the maximum TX power with the current calibration.
// ---------------------------------------------------------------------------

#include <QObject>
#include <QMessageBox>
#include <QWidget>
#include <QtGlobal>
#include <cmath>

namespace TxCalibration
{

// Current calibration offset in dB (default -6, kept in sync with the
// dsbTxCalib spinbox of the Calibration tab).
inline double &offsetDb()
{
    static double v = -6.0;
    return v;
}

inline void setOffsetDb(double v)
{
    offsetDb() = v;
}

// Calibrated P (attenuation): what the TX power really is.
inline double effective(double pDb)
{
    return pDb + offsetDb();
}

// Validate one P (attenuation) value against the calibration.  Returns
// false (after showing the maximum-power warning) when P + calibration is
// negative.  The P value mirror-syncs between the exciter panel and the
// TX1/TX2 attenuation spinboxes, so an identical repeated (P, calibration)
// pair warns only once; any different pair warns again.
inline bool checkP(double pDb, QWidget *parent = nullptr)
{
    static double lastP = qQNaN();
    static double lastCal = qQNaN();
    const double cal = offsetDb();
    if (pDb + cal >= 0.0)
    {
        // valid pair: arm the warning for the next invalid combination
        lastP = qQNaN();
        lastCal = qQNaN();
        return true;
    }
    if (pDb == lastP && cal == lastCal)
        return false; // already warned for this exact combination
    lastP = pDb;
    lastCal = cal;
    QMessageBox::warning(
        parent, QObject::tr("TX Calibration"),
        QObject::tr("Maximum power limit: the P (attenuation) must be at "
                    "least %1 dB with the current %2 dB TX calibration "
                    "(P + calibration must be >= 0).")
            .arg(-cal)
            .arg(cal));
    return false;
}

} // namespace TxCalibration

#endif // TX_CALIBRATION_H
