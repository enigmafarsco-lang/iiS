// ============================================================================
// File: pancomponent.h
// Subsystem: eLynxSDR Pan
// Description: Subsystem implementation and interface guidance for pancomponent.h.
// ============================================================================

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
    // Constructor: Initializes BPanComponent subsystem components, default parameters, and state.
    explicit BPanComponent(QWidget *parent = nullptr);

    // Initializes init labels for BPanComponent.
    void init_labels();
    // Initializes init double spin boxes for BPanComponent.
    void init_doubleSpinBoxes();
    // Initializes init buttons for BPanComponent.
    void init_buttons();
    // Initializes init progress bar for BPanComponent.
    void init_progressBar();
    // Initializes init pan for BPanComponent.
    void init_pan();
    // Sets components_arrangements value and updates internal subsystem configuration.
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
