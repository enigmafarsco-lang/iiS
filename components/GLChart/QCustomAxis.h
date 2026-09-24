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
    QCustomAxis(QObject *parent = nullptr);
    ~QCustomAxis();

    void			setAlignment(const Qt::Alignment &alignment);
    Qt::Alignment	alignment() const;
    void			setOrientation(const Qt::Orientation &orientation);
    Qt::Orientation	orientation() const;

    /***----- Range related functions -----***/

    void			setMax(const double &max);
    void			setMin(const double &min);
    void			setRange(const double &min, const double &max);

    double			getMax() const;
    double			getMin() const;
    QPair<qreal, qreal> getRange() const;

    bool			visibility() const;
    void			setVisible(bool on);
    bool			rangeReversed() const;
    void			setRangeReversed(bool reversed);
    QPair<bool, bool>	isPartialRangeSet() const;

    /***----- Label related functions -----***/

    void			setLabelsAngle(const qint32 &angle);
    void			setLabelsBrush(const QBrush &brush);
    void			setLabelsColor(const QColor &color);
    void			setLabelsFont(const QFont &font);
    void			setLabelsText(const QString &text);
    void			setLabelsVisible(bool visible = true);

    qint32			labelsAngle() const;
    QBrush			labelsBrush() const;
    QColor			labelsColor() const;
    QFont			labelsFont() const;
    QString			labelsText() const;
    bool			labelsVisible() const;

    /***----- Line Pen Setting -----***/

    void			setLinePen(const QPen &pen);
    void			setLinePenColor(const QColor &penColor);
    void			setLineVisible(bool visible = true);

    QPen			linePen() const;
    QColor			linePenColor() const;
    bool			lineVisible() const;

    /***----- Grid related functions -----***/

    void			setGridLinePen(const QPen &pen);
    void			setGridLineColor(const QColor &color);
    void			setGridLineVisible(bool visible = true);
    void			setGridLineCount(int count);

    QPen			gridLinePen() const;
    QColor			gridLineColor() const;
    bool			gridLineVisible() const;
    int				gridLineCount() const;

    void			setMinorGridLinePen(const QPen &pen);
    void			setMinorGridLineColor(const QColor &color);
    void			setMinorGridLineVisible(bool visible = true);

    QPen			minorGridLinePen() const;
    QColor			minorGridLineColor() const;
    bool			minorGridLineVisible() const;

    /***----- Shades related functions -----***/

    void			setShadesBorderColor(const QColor &color);
    void			setShadesBrush(const QBrush &brush);
    void			setShadesColor(const QColor &color);
    void			setShadesPen(const QPen &Pen);
    void			setShadesVisible(bool visible = true);

    QColor			shadesBorderColor() const;
    QBrush			shadesBrush() const;
    QColor			shadesColor() const;
    QPen			shadesPen() const;
    bool			shadesVisible() const;

    /***----- Title related functions -----***/

    void			setTitleBrush(const QBrush &brush);
    void			setTitlesFont(const QFont &font);
    void			setTitleText(const QString &text);
    void			setTitleVisible(bool visible = true); // title position might be added later

    QBrush			titleBrush() const;
    QFont			titleFont() const;
    QString			titleText() const;
    bool			titleVisible() const;

    /***--- Tick related functions -----***/

    void setTicks(bool show);
    void setTickLabels(bool show);
    void setTickLabelPadding(int padding);
    void setTickLabelFont(const QFont &font);
    void setTickLabelColor(const QColor &color);
    void setTickLength(int inside, int outside = 0);
    void setTickLengthIn(int inside);
    void setTickLengthOut(int outside);
    void setTickPen(const QPen &pen);

    bool ticks() const;
    bool ticksLabels() const;
    int tickLabelPadding() const;
    QFont tickLabelFont() const;
    QColor tickLabelColor() const;
    int tickLength() const;
    int tickLengthIn() const;
    int tickLengthOut() const;
    QPen tickPen() const;

    /***----- Sub tick related functions -----***/

    void setSubTicks(bool show);
    void setSubTickLength(int inside, int outside = 0);
    void setSubTickLengthIn(int inside);
    void setSubTickLengthOut(int outside);
    void setSubTickPen(const QPen &pen);

    bool subTicks() const;
    int subTickLength() const;
    int subTickLengthIn() const;
    int subTickLengthOut() const;
    QPen subTickPen() const;

    /***----- Number precision related functions -----***/

    void setNumberPrecision(int precision);
    int numberPrecision() const;

    /***----- Scale Type related functions -----***/

    void setScaleType(ScaleType scaleType);
    ScaleType scaleType() const;

signals:
    void			alignmentChanged(const Qt::Alignment &);
    void			orientationChanged(const Qt::Orientation &);

    void			labelsAngleChanged(qint32);
    void			labelsBrushChanged(QBrush);
    void			labelsColorChanged(QColor);
    void			labelsFontChanged(QFont);
    void			labelsTextChanged(QString);
    void			labelsVisibleChanged(bool);

    void			linePenChanged(QPen);
    void			linePenColorChanged(QColor);
    void			lineVisibleChanged(bool);

    void			gridLinePenChanged(const QPen &);
    void			gridLineColorChanged(const QColor &);
    void			gridLineVisibleChanged(bool);

    void			minorGridLinePenChanged(const QPen &);
    void			minorGridLineColorChanged(const QColor &);
    void			minorGridLineVisibleChanged(bool);

    void			shadesBorderColorChanged(const QColor &);
    void			shadesBrushChanged(const QBrush &);
    void			shadesColorChanged(const QColor &);
    void			shadesPenChanged(const QPen &);
    void			shadesVisibleChanged(bool);

    void			titleBrushChanged(const QBrush &);
    void			titleFontChanged(QFont);
    void			titleTextChanged(QString);
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
