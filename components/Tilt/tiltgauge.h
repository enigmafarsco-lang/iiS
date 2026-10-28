// ============================================================================
// File: tiltgauge.h
// Subsystem: eLynxSDR Tilt
// Description: Subsystem implementation and interface guidance for tiltgauge.h.
// ============================================================================

#ifndef TILTGAUGE_H
#define TILTGAUGE_H

#include <Qt>
#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>

namespace Ui {
class BTiltGauge;
}

class QColor;

class BTiltGauge : public QWidget
{
 Q_OBJECT
    Q_ENUMS(errorCode);
    Q_PROPERTY(double value READ value WRITE setValue);
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue);
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue);
    Q_PROPERTY(double threshold READ threshold WRITE setThreshold);
    Q_PROPERTY(int precision READ precision WRITE setPrecision);
    Q_PROPERTY(int precisionNumeric READ precisionNumeric WRITE setPrecisionNumeric);
    Q_PROPERTY(QString units READ units WRITE setUnits);
    Q_PROPERTY(QString label READ label WRITE setLabel);
    Q_PROPERTY(int steps READ steps WRITE setSteps);
    Q_PROPERTY(double startAngle READ startAngle WRITE setStartAngle);
    Q_PROPERTY(double endAngle READ endAngle WRITE setEndAngle);
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground);
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(bool enableThreshold READ enableThreshold WRITE setThresholdEnabled);
    Q_PROPERTY(double beginValidValue READ beginValidValue WRITE setBeginValidValue);
    Q_PROPERTY(double endValidValue READ endValidValue WRITE setEndValidValue);
    Q_PROPERTY(bool enableValidWindow READ enableValidWindow WRITE setEnableValidWindow);
    Q_PROPERTY(double beginWarningValue READ beginWarningValue WRITE setBeginWarningValue);
    Q_PROPERTY(double endWarningValue READ endWarningValue WRITE setEndWarningValue);
    Q_PROPERTY(bool enableWarningWindow READ enableWarningWindow WRITE setEnableWarningWindow);
    Q_PROPERTY(bool enableNumericIndicator READ enableNumericIndicator WRITE setNumericIndicatorEnabled);

public:

    enum ErrorCode {MaxValueError=1,MinValueError,ThresholdError,TargetError,PrecisionError,ColorError,UnitsEmpty,OutOfRange};
    // Constructor: Initializes BTiltGauge subsystem components, default parameters, and state.
    BTiltGauge(QWidget *parent = 0);
    double value() const { return m_value; }
    double minValue() const { return m_minValue; }
    double maxValue() const { return m_maxValue; }
    double threshold() const { return m_threshold; }
    double beginValidValue() const { return m_beginValidValue; }
    double endValidValue() const { return m_endValidValue; }
    double beginWarningValue() const { return m_beginWarningValue; }
    double endWarningValue() const { return m_endWarningValue; }

    int precision() const { return m_precision; }
    int precisionNumeric() const { return m_precisionNumeric; }
    QString units()const { return m_units; }
    QString label()const { return m_label; }
    int steps() const { return m_steps; }
    double startAngle() const { return m_startAngle; }
    double endAngle() const { return m_endAngle; }
    QColor foreground() const { return m_foreground; }
    QColor background() const { return m_background; }
    bool enableThreshold() const { return m_thresholdEnabled; }
    bool enableValidWindow() const { return m_enableValidWindow; }
    bool enableWarningWindow() const { return m_enableWarningWindow; }
    bool enableNumericIndicator() const { return m_numericIndicatorEnabled; }

    // Initializes init pan gauge for BTiltGauge.
    void init_panGauge();

    double pan_degree(){return this->value();}

signals:

    // Executes error signal routine.
    void errorSignal(int);
    // Executes threshold alarm routine.
    void thresholdAlarm(bool);

public slots:

    // Sets value value and updates internal subsystem configuration.
    void setValue(double);
    // Sets value value and updates internal subsystem configuration.
    void setValue(int);
    // Sets min value value and updates internal subsystem configuration.
    void setMinValue(double);
    // Sets min value value and updates internal subsystem configuration.
    void setMinValue(int);
    // Sets max value value and updates internal subsystem configuration.
    void setMaxValue(double);
    // Sets max value value and updates internal subsystem configuration.
    void setMaxValue(int);
    // Sets threshold value and updates internal subsystem configuration.
    void setThreshold(double);
    // Sets threshold value and updates internal subsystem configuration.
    void setThreshold(int);
    // Sets precision value and updates internal subsystem configuration.
    void setPrecision(int);
    // Sets precision numeric value and updates internal subsystem configuration.
    void setPrecisionNumeric(int);
    // Sets units value and updates internal subsystem configuration.
    void setUnits(QString);
    // Sets label value and updates internal subsystem configuration.
    void setLabel(QString);
    // Sets steps value and updates internal subsystem configuration.
    void setSteps(int);
    // Sets start angle value and updates internal subsystem configuration.
    void setStartAngle(double);
    // Sets end angle value and updates internal subsystem configuration.
    void setEndAngle(double);
    // Sets foreground value and updates internal subsystem configuration.
    void setForeground(QColor);
    // Sets background value and updates internal subsystem configuration.
    void setBackground(QColor);
    // Sets threshold enabled value and updates internal subsystem configuration.
    void setThresholdEnabled(bool);
    // Sets numeric indicator enabled value and updates internal subsystem configuration.
    void setNumericIndicatorEnabled(bool);
    // Sets begin valid value value and updates internal subsystem configuration.
    void setBeginValidValue(double);
    // Sets end valid value value and updates internal subsystem configuration.
    void setEndValidValue(double);
    // Sets enable valid window value and updates internal subsystem configuration.
    void setEnableValidWindow(bool);
    // Sets begin warning value value and updates internal subsystem configuration.
    void setBeginWarningValue(double);
    // Sets end warning value value and updates internal subsystem configuration.
    void setEndWarningValue(double);
    // Sets enable warning window value and updates internal subsystem configuration.
    void setEnableWarningWindow(bool);

protected:

    double m_value;
    double m_maxValue, m_minValue;
    int m_precision;
    int m_precisionNumeric;
    // Executes threshold manager routine.
    void thresholdManager();
    // Executes paint event routine.
    void paintEvent(QPaintEvent *event);
    // Executes draw crown routine.
    void drawCrown(QPainter *painter);
    // Executes draw background routine.
    void drawBackground(QPainter *painter);
    // Executes draw ticks routine.
    void drawTicks(QPainter *painter);
    // Executes draw scale routine.
    void drawScale(QPainter *painter);
    // Executes draw units routine.
    void drawUnits(QPainter *painter);
    // Executes draw needle routine.
    void drawNeedle(QPainter *painter);
    // Executes draw threshold line routine.
    void drawThresholdLine(QPainter *painter);
    // Executes draw numeric value routine.
    void drawNumericValue(QPainter *painter);
    // Executes draw cover glass routine.
    void drawCoverGlass(QPainter *painter);
    // Executes draw label routine.
    void drawLabel(QPainter *painter);
    // Executes draw valid window routine.
    void drawValidWindow(QPainter *painter);
    // Executes draw warning window routine.
    void drawWarningWindow(QPainter *painter);

    QString m_units;
    QString m_label;
    int m_steps;
    double m_startAngle,m_endAngle;
    double m_threshold;
    bool m_thresholdFlag;
    bool m_enableValidWindow;
    double m_beginValidValue, m_endValidValue;
    bool m_enableWarningWindow;
    double m_beginWarningValue, m_endWarningValue;
    bool m_thresholdEnabled;
    bool m_numericIndicatorEnabled;
    QColor m_foreground;
    QColor m_background;
    QPropertyAnimation m_valAnimation;
};

#endif // TILTGAUGE_H
