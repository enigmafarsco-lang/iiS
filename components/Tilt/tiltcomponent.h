// ============================================================================
// File: tiltcomponent.h
// Subsystem: eLynxSDR Tilt
// Description: Subsystem implementation and interface guidance for tiltcomponent.h.
// ============================================================================

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
    // Constructor: Initializes BTiltComponent subsystem components, default parameters, and state.
    explicit BTiltComponent(QWidget *parent = nullptr);

    // Initializes init labels for BTiltComponent.
    void init_labels();
    // Initializes init double spin boxes for BTiltComponent.
    void init_doubleSpinBoxes();
    // Initializes init buttons for BTiltComponent.
    void init_buttons();
    // Initializes init progress bar for BTiltComponent.
    void init_progressBar();
    // Initializes init tilt for BTiltComponent.
    void init_tilt();
    // Sets components_arrangements value and updates internal subsystem configuration.
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
