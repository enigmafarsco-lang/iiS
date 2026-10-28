/**
 * @file datastruct.h
 * @brief Subsystem module for joshan in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef DATASTRUCT_H
#define DATASTRUCT_H


#include <iostream>
struct joshanFuncStruct
{
    //servo ------------
    double tilt_speed{};
    double pan_speed{};
    double pan{10};
    double tilt{};

    //GPS ------------
    double longitude{};
    double latitude{};

    //solid
    std::string solid_enable;
    std::string solid_level;
    bool solid_alc;

    //exciter ------------
    int exciter_start_frequency{};
    double exciter_pulse_width{};
    int exciter_stop_frequency{};
    std::string exciter_enable;
    int exciter_sweep_time{};
    int exciter_band_width{};
    std::string exciter_mode;
    int exciter_frequency{};
    double exciter_pri{};
    int exciter_power{};

    //Cooling ------------
    std::string cooling_enable;
    std::string cooling_mode;

    //RFF ------------
    std::string lna_enable;
} ;





struct joshanStatusStruct
{
   //solid ------------
    double solid_amplifier_temperature{};
    double solid_internal_temperature{};
    double solid_driver_temperature{};
    double solid_forward_power{};
    double solid_reverce_power{};
    int solid_internal_lock{};
    bool solid_active{false};
    int solid_serial_link{};
    double solid_current{};
    double solid_voltage{};
    int solid_gate_level{};
    int solid_dc_supply{};
    int solid_vswr{};
    int solid_fan{};

    //cooling ------------
    double cooling_motor_current{};
    double cooling_fan_currenrt{};
    double cooling_compressor{};

    //rff ------------
    bool lna_status{false};

    //permission ------------
    bool remot_control_permission{true};
};







/**
 * @class dataStruct
 * @brief High-level controller class for dataStruct.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class dataStruct
{
public:
    /**
     * @brief Constructs a new dataStruct instance and initializes subsystem components and UI state.
     */
    dataStruct();
};

#endif // DATASTRUCT_H
