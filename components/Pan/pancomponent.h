/**
 * @file pancomponent.h
 * @brief Subsystem module for Pan in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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

/**
 * @class BPanComponent
 * @brief High-level controller class for BPanComponent.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class BPanComponent : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new BPanComponent instance and initializes subsystem components and UI state.
     * @param parent Input parameter specifying parent (QWidget).
     */
    explicit BPanComponent(QWidget *parent = nullptr);

    /**
     * @brief Initializes and configures the init labels for the BPanComponent subsystem.
     */
    void init_labels();
    /**
     * @brief Initializes and configures the init double spin boxes for the BPanComponent subsystem.
     */
    void init_doubleSpinBoxes();
    /**
     * @brief Initializes and configures the init buttons for the BPanComponent subsystem.
     */
    void init_buttons();
    /**
     * @brief Initializes and configures the init progress bar for the BPanComponent subsystem.
     */
    void init_progressBar();
    /**
     * @brief Initializes and configures the init pan for the BPanComponent subsystem.
     */
    void init_pan();
    /**
     * @brief Sets the components_arrangements parameter and updates internal state.
     */
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
