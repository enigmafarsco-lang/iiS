/**
 * @file tiltgauge.h
 * @brief Subsystem module for Tilt in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef TILTGAUGE_H
#define TILTGAUGE_H

#include <Qt>
#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>

namespace Ui {
/**
 * @class BTiltGauge
 * @brief High-level controller class for BTiltGauge.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class BTiltGauge;
}

/**
 * @class QColor
 * @brief High-level controller class for QColor.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class QColor;

/**
 * @class BTiltGauge
 * @brief High-level controller class for BTiltGauge.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class BTiltGauge : public QWidget
{
 Q_OBJECT
    /**
     * @brief Executes the q  e n u m s procedure for BTiltGauge.
     * @param errorCode Input parameter specifying errorCode (errorCode).
     */
    Q_ENUMS(errorCode);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setValue Input parameter specifying setValue (double value READ value WRITE).
     */
    Q_PROPERTY(double value READ value WRITE setValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setMinValue Input parameter specifying setMinValue (double minValue READ minValue WRITE).
     */
    Q_PROPERTY(double minValue READ minValue WRITE setMinValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setMaxValue Input parameter specifying setMaxValue (double maxValue READ maxValue WRITE).
     */
    Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setThreshold Input parameter specifying setThreshold (double threshold READ threshold WRITE).
     */
    Q_PROPERTY(double threshold READ threshold WRITE setThreshold);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setPrecision Input parameter specifying setPrecision (int precision READ precision WRITE).
     */
    Q_PROPERTY(int precision READ precision WRITE setPrecision);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setPrecisionNumeric Input parameter specifying setPrecisionNumeric (int precisionNumeric READ precisionNumeric WRITE).
     */
    Q_PROPERTY(int precisionNumeric READ precisionNumeric WRITE setPrecisionNumeric);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setUnits Input parameter specifying setUnits (QString units READ units WRITE).
     */
    Q_PROPERTY(QString units READ units WRITE setUnits);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setLabel Input parameter specifying setLabel (QString label READ label WRITE).
     */
    Q_PROPERTY(QString label READ label WRITE setLabel);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setSteps Input parameter specifying setSteps (int steps READ steps WRITE).
     */
    Q_PROPERTY(int steps READ steps WRITE setSteps);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setStartAngle Input parameter specifying setStartAngle (double startAngle READ startAngle WRITE).
     */
    Q_PROPERTY(double startAngle READ startAngle WRITE setStartAngle);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setEndAngle Input parameter specifying setEndAngle (double endAngle READ endAngle WRITE).
     */
    Q_PROPERTY(double endAngle READ endAngle WRITE setEndAngle);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setForeground Input parameter specifying setForeground (QColor foreground READ foreground WRITE).
     */
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setBackground Input parameter specifying setBackground (QColor background READ background WRITE).
     */
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setThresholdEnabled Input parameter specifying setThresholdEnabled (bool enableThreshold READ enableThreshold WRITE).
     */
    Q_PROPERTY(bool enableThreshold READ enableThreshold WRITE setThresholdEnabled);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setBeginValidValue Input parameter specifying setBeginValidValue (double beginValidValue READ beginValidValue WRITE).
     */
    Q_PROPERTY(double beginValidValue READ beginValidValue WRITE setBeginValidValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setEndValidValue Input parameter specifying setEndValidValue (double endValidValue READ endValidValue WRITE).
     */
    Q_PROPERTY(double endValidValue READ endValidValue WRITE setEndValidValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setEnableValidWindow Input parameter specifying setEnableValidWindow (bool enableValidWindow READ enableValidWindow WRITE).
     */
    Q_PROPERTY(bool enableValidWindow READ enableValidWindow WRITE setEnableValidWindow);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setBeginWarningValue Input parameter specifying setBeginWarningValue (double beginWarningValue READ beginWarningValue WRITE).
     */
    Q_PROPERTY(double beginWarningValue READ beginWarningValue WRITE setBeginWarningValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setEndWarningValue Input parameter specifying setEndWarningValue (double endWarningValue READ endWarningValue WRITE).
     */
    Q_PROPERTY(double endWarningValue READ endWarningValue WRITE setEndWarningValue);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setEnableWarningWindow Input parameter specifying setEnableWarningWindow (bool enableWarningWindow READ enableWarningWindow WRITE).
     */
    Q_PROPERTY(bool enableWarningWindow READ enableWarningWindow WRITE setEnableWarningWindow);
    /**
     * @brief Executes the q  p r o p e r t y procedure for BTiltGauge.
     * @param setNumericIndicatorEnabled Input parameter specifying setNumericIndicatorEnabled (bool enableNumericIndicator READ enableNumericIndicator WRITE).
     */
    Q_PROPERTY(bool enableNumericIndicator READ enableNumericIndicator WRITE setNumericIndicatorEnabled);

public:

    enum ErrorCode {MaxValueError=1,MinValueError,ThresholdError,TargetError,PrecisionError,ColorError,UnitsEmpty,OutOfRange};
    /**
     * @brief Constructs a new BTiltGauge instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
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

    /**
     * @brief Initializes and configures the init pan gauge for the BTiltGauge subsystem.
     */
    void init_panGauge();

    double pan_degree(){return this->value();}

signals:

    /**
     * @brief Executes the error signal procedure for BTiltGauge.
     * @param int Input parameter specifying int (int).
     */
    void errorSignal(int);
    /**
     * @brief Executes the threshold alarm procedure for BTiltGauge.
     * @param bool Input parameter specifying bool (bool).
     */
    void thresholdAlarm(bool);

public slots:

    /**
     * @brief Sets the value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setValue(double);
    /**
     * @brief Sets the value parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setValue(int);
    /**
     * @brief Sets the min value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setMinValue(double);
    /**
     * @brief Sets the min value parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setMinValue(int);
    /**
     * @brief Sets the max value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setMaxValue(double);
    /**
     * @brief Sets the max value parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setMaxValue(int);
    /**
     * @brief Sets the threshold parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setThreshold(double);
    /**
     * @brief Sets the threshold parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setThreshold(int);
    /**
     * @brief Sets the precision parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setPrecision(int);
    /**
     * @brief Sets the precision numeric parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setPrecisionNumeric(int);
    /**
     * @brief Sets the units parameter and updates internal state.
     * @param QString Input parameter specifying QString (QString).
     */
    void setUnits(QString);
    /**
     * @brief Sets the label parameter and updates internal state.
     * @param QString Input parameter specifying QString (QString).
     */
    void setLabel(QString);
    /**
     * @brief Sets the steps parameter and updates internal state.
     * @param int Input parameter specifying int (int).
     */
    void setSteps(int);
    /**
     * @brief Sets the start angle parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setStartAngle(double);
    /**
     * @brief Sets the end angle parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setEndAngle(double);
    /**
     * @brief Sets the foreground parameter and updates internal state.
     * @param QColor Input parameter specifying QColor (QColor).
     */
    void setForeground(QColor);
    /**
     * @brief Sets the background parameter and updates internal state.
     * @param QColor Input parameter specifying QColor (QColor).
     */
    void setBackground(QColor);
    /**
     * @brief Sets the threshold enabled parameter and updates internal state.
     * @param bool Input parameter specifying bool (bool).
     */
    void setThresholdEnabled(bool);
    /**
     * @brief Sets the numeric indicator enabled parameter and updates internal state.
     * @param bool Input parameter specifying bool (bool).
     */
    void setNumericIndicatorEnabled(bool);
    /**
     * @brief Sets the begin valid value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setBeginValidValue(double);
    /**
     * @brief Sets the end valid value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setEndValidValue(double);
    /**
     * @brief Sets the enable valid window parameter and updates internal state.
     * @param bool Input parameter specifying bool (bool).
     */
    void setEnableValidWindow(bool);
    /**
     * @brief Sets the begin warning value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setBeginWarningValue(double);
    /**
     * @brief Sets the end warning value parameter and updates internal state.
     * @param double Input parameter specifying double (double).
     */
    void setEndWarningValue(double);
    /**
     * @brief Sets the enable warning window parameter and updates internal state.
     * @param bool Input parameter specifying bool (bool).
     */
    void setEnableWarningWindow(bool);

protected:

    double m_value;
    double m_maxValue, m_minValue;
    int m_precision;
    int m_precisionNumeric;
    /**
     * @brief Executes the threshold manager procedure for BTiltGauge.
     */
    void thresholdManager();
    /**
     * @brief Executes the paint event procedure for BTiltGauge.
     * @param event Input parameter specifying event (QPaintEvent).
     */
    void paintEvent(QPaintEvent *event);
    /**
     * @brief Executes the draw crown procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawCrown(QPainter *painter);
    /**
     * @brief Executes the draw background procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawBackground(QPainter *painter);
    /**
     * @brief Executes the draw ticks procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawTicks(QPainter *painter);
    /**
     * @brief Executes the draw scale procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawScale(QPainter *painter);
    /**
     * @brief Executes the draw units procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawUnits(QPainter *painter);
    /**
     * @brief Executes the draw needle procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawNeedle(QPainter *painter);
    /**
     * @brief Executes the draw threshold line procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawThresholdLine(QPainter *painter);
    /**
     * @brief Executes the draw numeric value procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawNumericValue(QPainter *painter);
    /**
     * @brief Executes the draw cover glass procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawCoverGlass(QPainter *painter);
    /**
     * @brief Executes the draw label procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawLabel(QPainter *painter);
    /**
     * @brief Executes the draw valid window procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
    void drawValidWindow(QPainter *painter);
    /**
     * @brief Executes the draw warning window procedure for BTiltGauge.
     * @param painter Input parameter specifying painter (QPainter).
     */
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
