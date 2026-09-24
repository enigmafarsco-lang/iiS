#ifndef PANCOMPONENT_H
#define PANCOMPONENT_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QProgressBar>
#include <QSizePolicy>
#include "pangauge.h"
#include "constants/project_constans.h"

class BPanComponent : public QWidget
{
    Q_OBJECT
public:
    explicit BPanComponent(QWidget *parent = nullptr);

    void init_labels();
    void init_doubleSpinBoxes();
    void init_buttons();
    void init_progressBar();
    void init_pan();
    void set_components_arrangements();

    QGridLayout *grdMainLayout;
    QLabel *lblPanAngle;
    QLabel *lblPanSpeed;
    QLabel *lblCurrentPanAngle;
    QLabel *lblCurrentPanAngleValue;
    QDoubleSpinBox *dsbPanAngle;
    QDoubleSpinBox *dsbPanSpeed;
    QPushButton *btnSetPanAngle;
    QPushButton *btnSetPanSpeed;
    QProgressBar* pgbPanAngle;
    BPanGauge *panGauge;

private:
    QSizePolicy spExpanding = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSizePolicy spFixed = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

};

#endif // PANCOMPONENT_H
