#ifndef TILTCOMPONENT_H
#define TILTCOMPONENT_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QProgressBar>
#include "tiltgauge.h"
#include "../../constants/project_constans.h"

class BTiltComponent : public QWidget
{
    Q_OBJECT
public:
    explicit BTiltComponent(QWidget *parent = nullptr);

    void init_labels();
    void init_doubleSpinBoxes();
    void init_buttons();
    void init_progressBar();
    void init_tilt();
    void set_components_arrangements();

    QGridLayout *grdMainLayout;
    QLabel *lblTiltAngle;
    QLabel *lblTiltSpeed;
    QLabel *lblCurrentTiltAngle;
    QLabel *lblCurrentTiltAngleValue;
    QDoubleSpinBox *dsbTiltAngle;
    QDoubleSpinBox *dsbTiltSpeed;
    QPushButton *btnSetTiltAngle;
    QPushButton *btnSetTiltSpeed;
    QProgressBar* pgbTiltAngle;
    BTiltGauge *tiltGauge;

private:
    QSizePolicy spExpanding = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSizePolicy spFixed = QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
};

#endif // TILTCOMPONENT_H
