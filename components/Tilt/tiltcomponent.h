/**
 * @file tiltcomponent.h
 * @brief Subsystem module for Tilt in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

/**
 * @class BTiltComponent
 * @brief High-level controller class for BTiltComponent.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class BTiltComponent : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new BTiltComponent instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit BTiltComponent(QWidget *parent = nullptr);

    /**
     * @brief Initializes and configures the init labels for the BTiltComponent subsystem.
     */
    void init_labels();
    /**
     * @brief Initializes and configures the init double spin boxes for the BTiltComponent subsystem.
     */
    void init_doubleSpinBoxes();
    /**
     * @brief Initializes and configures the init buttons for the BTiltComponent subsystem.
     */
    void init_buttons();
    /**
     * @brief Initializes and configures the init progress bar for the BTiltComponent subsystem.
     */
    void init_progressBar();
    /**
     * @brief Initializes and configures the init tilt for the BTiltComponent subsystem.
     */
    void init_tilt();
    /**
     * @brief Sets the components_arrangements parameter and updates internal state.
     */
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
