// ============================================================================
// File: QCustomAxis.h
// Subsystem: eLynxSDR GLChart
// Description: Subsystem implementation and interface guidance for QCustomAxis.h.
// ============================================================================

#pragma once

#include <QObject>
#include <QtWidgets/QWidget>
#include <qpen.h>

enum ScaleType
{
    Linear = 0x00,
    Logrithmic = 0x01
};

class QCustomAxis : public QObject
{
    Q_OBJECT

public:
    // Constructor: Initializes QCustomAxis subsystem components, default parameters, and state.
    QCustomAxis(QObject *parent = nullptr);
    // Destructor: Cleans up QCustomAxis resources, timers, and allocated memory.
    ~QCustomAxis();

    // Sets alignment value and updates internal subsystem configuration.
    void			setAlignment(const Qt::Alignment &alignment);
    // Executes alignment routine.
    Qt::Alignment	alignment() const;
    // Sets orientation value and updates internal subsystem configuration.
    void			setOrientation(const Qt::Orientation &orientation);
    // Executes orientation routine.
    Qt::Orientation	orientation() const;

    /***----- Range related functions -----***/

    // Sets max value and updates internal subsystem configuration.
    void			setMax(const double &max);
    // Sets min value and updates internal subsystem configuration.
    void			setMin(const double &min);
    // Sets range value and updates internal subsystem configuration.
    void			setRange(const double &min, const double &max);

    // Retrieves current max state.
    double			getMax() const;
    // Retrieves current min state.
    double			getMin() const;
    // Retrieves current range state.
    QPair<qreal, qreal> getRange() const;

    // Executes visibility routine.
    bool			visibility() const;
    // Sets visible value and updates internal subsystem configuration.
    void			setVisible(bool on);
    // Executes range reversed routine.
    bool			rangeReversed() const;
    // Sets range reversed value and updates internal subsystem configuration.
    void			setRangeReversed(bool reversed);
    // Retrieves current artial range set state.
    QPair<bool, bool>	isPartialRangeSet() const;

    /***----- Label related functions -----***/

    // Sets labels angle value and updates internal subsystem configuration.
    void			setLabelsAngle(const qint32 &angle);
    // Sets labels brush value and updates internal subsystem configuration.
    void			setLabelsBrush(const QBrush &brush);
    // Sets labels color value and updates internal subsystem configuration.
    void			setLabelsColor(const QColor &color);
    // Sets labels font value and updates internal subsystem configuration.
    void			setLabelsFont(const QFont &font);
    // Sets labels text value and updates internal subsystem configuration.
    void			setLabelsText(const QString &text);
    // Sets labels visible value and updates internal subsystem configuration.
    void			setLabelsVisible(bool visible = true);

    // Executes labels angle routine.
    qint32			labelsAngle() const;
    // Executes labels brush routine.
    QBrush			labelsBrush() const;
    // Executes labels color routine.
    QColor			labelsColor() const;
    // Executes labels font routine.
    QFont			labelsFont() const;
    // Executes labels text routine.
    QString			labelsText() const;
    // Executes labels visible routine.
    bool			labelsVisible() const;

    /***----- Line Pen Setting -----***/

    // Sets line pen value and updates internal subsystem configuration.
    void			setLinePen(const QPen &pen);
    // Sets line pen color value and updates internal subsystem configuration.
    void			setLinePenColor(const QColor &penColor);
    // Sets line visible value and updates internal subsystem configuration.
    void			setLineVisible(bool visible = true);

    // Executes line pen routine.
    QPen			linePen() const;
    // Executes line pen color routine.
    QColor			linePenColor() const;
    // Executes line visible routine.
    bool			lineVisible() const;

    /***----- Grid related functions -----***/

    // Sets grid line pen value and updates internal subsystem configuration.
    void			setGridLinePen(const QPen &pen);
    // Sets grid line color value and updates internal subsystem configuration.
    void			setGridLineColor(const QColor &color);
    // Sets grid line visible value and updates internal subsystem configuration.
    void			setGridLineVisible(bool visible = true);
    // Sets grid line count value and updates internal subsystem configuration.
    void			setGridLineCount(int count);

    // Executes grid line pen routine.
    QPen			gridLinePen() const;
    // Executes grid line color routine.
    QColor			gridLineColor() const;
    // Executes grid line visible routine.
    bool			gridLineVisible() const;
    // Executes grid line count routine.
    int				gridLineCount() const;

    // Sets minor grid line pen value and updates internal subsystem configuration.
    void			setMinorGridLinePen(const QPen &pen);
    // Sets minor grid line color value and updates internal subsystem configuration.
    void			setMinorGridLineColor(const QColor &color);
    // Sets minor grid line visible value and updates internal subsystem configuration.
    void			setMinorGridLineVisible(bool visible = true);

    // Executes minor grid line pen routine.
    QPen			minorGridLinePen() const;
    // Executes minor grid line color routine.
    QColor			minorGridLineColor() const;
    // Executes minor grid line visible routine.
    bool			minorGridLineVisible() const;

    /***----- Shades related functions -----***/

    // Sets shades border color value and updates internal subsystem configuration.
    void			setShadesBorderColor(const QColor &color);
    // Sets shades brush value and updates internal subsystem configuration.
    void			setShadesBrush(const QBrush &brush);
    // Sets shades color value and updates internal subsystem configuration.
    void			setShadesColor(const QColor &color);
    // Sets shades pen value and updates internal subsystem configuration.
    void			setShadesPen(const QPen &Pen);
    // Sets shades visible value and updates internal subsystem configuration.
    void			setShadesVisible(bool visible = true);

    // Executes shades border color routine.
    QColor			shadesBorderColor() const;
    // Executes shades brush routine.
    QBrush			shadesBrush() const;
    // Executes shades color routine.
    QColor			shadesColor() const;
    // Executes shades pen routine.
    QPen			shadesPen() const;
    // Executes shades visible routine.
    bool			shadesVisible() const;

    /***----- Title related functions -----***/

    // Sets title brush value and updates internal subsystem configuration.
    void			setTitleBrush(const QBrush &brush);
    // Sets titles font value and updates internal subsystem configuration.
    void			setTitlesFont(const QFont &font);
    // Sets title text value and updates internal subsystem configuration.
    void			setTitleText(const QString &text);
    void			setTitleVisible(bool visible = true); // title position might be added later

    // Executes title brush routine.
    QBrush			titleBrush() const;
    // Executes title font routine.
    QFont			titleFont() const;
    // Executes title text routine.
    QString			titleText() const;
    // Executes title visible routine.
    bool			titleVisible() const;

    /***--- Tick related functions -----***/

    // Sets ticks value and updates internal subsystem configuration.
    void setTicks(bool show);
    // Sets tick labels value and updates internal subsystem configuration.
    void setTickLabels(bool show);
    // Sets tick label padding value and updates internal subsystem configuration.
    void setTickLabelPadding(int padding);
    // Sets tick label font value and updates internal subsystem configuration.
    void setTickLabelFont(const QFont &font);
    // Sets tick label color value and updates internal subsystem configuration.
    void setTickLabelColor(const QColor &color);
    // Sets tick length value and updates internal subsystem configuration.
    void setTickLength(int inside, int outside = 0);
    // Sets tick length in value and updates internal subsystem configuration.
    void setTickLengthIn(int inside);
    // Sets tick length out value and updates internal subsystem configuration.
    void setTickLengthOut(int outside);
    // Sets tick pen value and updates internal subsystem configuration.
    void setTickPen(const QPen &pen);

    // Executes ticks routine.
    bool ticks() const;
    // Executes ticks labels routine.
    bool ticksLabels() const;
    // Executes tick label padding routine.
    int tickLabelPadding() const;
    // Executes tick label font routine.
    QFont tickLabelFont() const;
    // Executes tick label color routine.
    QColor tickLabelColor() const;
    // Executes tick length routine.
    int tickLength() const;
    // Executes tick length in routine.
    int tickLengthIn() const;
    // Executes tick length out routine.
    int tickLengthOut() const;
    // Executes tick pen routine.
    QPen tickPen() const;

    /***----- Sub tick related functions -----***/

    // Sets sub ticks value and updates internal subsystem configuration.
    void setSubTicks(bool show);
    // Sets sub tick length value and updates internal subsystem configuration.
    void setSubTickLength(int inside, int outside = 0);
    // Sets sub tick length in value and updates internal subsystem configuration.
    void setSubTickLengthIn(int inside);
    // Sets sub tick length out value and updates internal subsystem configuration.
    void setSubTickLengthOut(int outside);
    // Sets sub tick pen value and updates internal subsystem configuration.
    void setSubTickPen(const QPen &pen);

    // Executes sub ticks routine.
    bool subTicks() const;
    // Executes sub tick length routine.
    int subTickLength() const;
    // Executes sub tick length in routine.
    int subTickLengthIn() const;
    // Executes sub tick length out routine.
    int subTickLengthOut() const;
    // Executes sub tick pen routine.
    QPen subTickPen() const;

    /***----- Number precision related functions -----***/

    // Sets number precision value and updates internal subsystem configuration.
    void setNumberPrecision(int precision);
    // Executes number precision routine.
    int numberPrecision() const;

    /***----- Scale Type related functions -----***/

    // Sets scale type value and updates internal subsystem configuration.
    void setScaleType(ScaleType scaleType);
    // Executes scale type routine.
    ScaleType scaleType() const;

signals:
    // Executes alignment changed routine.
    void			alignmentChanged(const Qt::Alignment &);
    // Executes orientation changed routine.
    void			orientationChanged(const Qt::Orientation &);

    // Executes labels angle changed routine.
    void			labelsAngleChanged(qint32);
    // Executes labels brush changed routine.
    void			labelsBrushChanged(QBrush);
    // Executes labels color changed routine.
    void			labelsColorChanged(QColor);
    // Executes labels font changed routine.
    void			labelsFontChanged(QFont);
    // Executes labels text changed routine.
    void			labelsTextChanged(QString);
    // Executes labels visible changed routine.
    void			labelsVisibleChanged(bool);

    // Executes line pen changed routine.
    void			linePenChanged(QPen);
    // Executes line pen color changed routine.
    void			linePenColorChanged(QColor);
    // Executes line visible changed routine.
    void			lineVisibleChanged(bool);

    // Executes grid line pen changed routine.
    void			gridLinePenChanged(const QPen &);
    // Executes grid line color changed routine.
    void			gridLineColorChanged(const QColor &);
    // Executes grid line visible changed routine.
    void			gridLineVisibleChanged(bool);

    // Executes minor grid line pen changed routine.
    void			minorGridLinePenChanged(const QPen &);
    // Executes minor grid line color changed routine.
    void			minorGridLineColorChanged(const QColor &);
    // Executes minor grid line visible changed routine.
    void			minorGridLineVisibleChanged(bool);

    // Executes shades border color changed routine.
    void			shadesBorderColorChanged(const QColor &);
    // Executes shades brush changed routine.
    void			shadesBrushChanged(const QBrush &);
    // Executes shades color changed routine.
    void			shadesColorChanged(const QColor &);
    // Executes shades pen changed routine.
    void			shadesPenChanged(const QPen &);
    // Executes shades visible changed routine.
    void			shadesVisibleChanged(bool);

    // Executes title brush changed routine.
    void			titleBrushChanged(const QBrush &);
    // Executes title font changed routine.
    void			titleFontChanged(QFont);
    // Executes title text changed routine.
    void			titleTextChanged(QString);
    // Executes title visible changed routine.
    void			titleVisibleChanged(bool);

private:

    Qt::Alignment	m_alignment;
    Qt::Orientation m_orientation;

    double			m_max;
    double			m_min;
    bool			m_lowerPartialRangeSet;
    bool			m_upperPartialRangeSet;
    bool			m_axisVisibility;
    bool			m_reverseRange;

    qint32			m_labelsAngle;
    QBrush			m_labelsBrush;
    QColor			m_labelsColor;
    QFont			m_labelsFont;
    bool			m_labelsVisibility;
    QString			m_labelsText;

    QPen			m_linePen;
    QColor			m_linePenColor;
    bool			m_lineVisible;

    /***--- Grid related variables -----***/

    QPen			m_gridLinePen;
    QColor			m_gridLineColor;
    bool			m_gridLineVisible;
    int				m_gridLineCount;	// Should be synced with the tick count later

    QPen			m_minorGridLinePen;
    QColor			m_minorGridLineColor;
    bool			m_minorGridLineVisible;

    /***--- Shades related variables -----***/

    QColor			m_shadesBorderColor;
    QBrush			m_shadesBrush;
    QColor			m_shadesColor;
    QPen			m_shadesPen;
    bool			m_shadesVisible;

    QBrush			m_titleBrush;
    QFont			m_titleFont;
    QString			m_titleText;
    bool			m_titleVisible;

    int				m_numberPrecision;

    /***--- Tick related variables -----***/

    bool			m_showTicks;
    bool			m_showTickLabels;
    int				m_tickLabelPadding;
    QFont			m_tickLabelFont;
    QColor			m_tickLabelColor;
    int				m_tickLengthIn;
    int				m_tickLengthOut;
    QPen			m_tickPen;

    /***----- Sub tick related variables -----***/

    bool			m_showSubTicks;
    int				m_subTickLengthIn;
    int				m_subTickLengthOut;
    QPen			m_subTickPen;

    /***----- Scale Type related variables -----***/

    ScaleType		m_scaleType;
};
