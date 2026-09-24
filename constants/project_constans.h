#ifndef PROJECT_CONSTANS_H
#define PROJECT_CONSTANS_H

#endif // PROJECT_CONSTANS_H



#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

// Added/ by s.raziani
//#define DC_6_UPTO_8_12 13598.6
#define DC_6_UPTO_8_12 0
#define btnJog "btnJog"
#define PI 3.141592653589793238
#define SERVO_DEVICE_NAME "SERVO"
#define RFF_DEVICE_NAME "RFF"
#define EXCITER_DEVICE_NAME "exciter"
#define TUNER_DEVICE_NAME "TUNER"
#define SOLID_DEVICE_NAME "SOLID"
#define JOSHAN_DEVICE_NAME "JOSHAN"
#define COOLING_DEVICE_NAME "COOLING"
#define COLIBR_DEVICE_NAME "COLIBR"
#define OUTER_NODE "OUTER_NODE"
//joshan constant
#define FUNCTIONAL_REPORT "functional"
#define STATUS_REPORT "status"


#define HUMMID_WARNING_THRESHOLD   50
#define HUMMID_CRITICAL_THRESHOLD  80
#define TEMP_WARNING_THRESHOLD     55
#define TEMP_CRITICAL_THRESHOLD    70

// Added/ by s.raziani

// Exciter Constants
#define MIN_FREQUENCY 6000
#define MAX_FREQUENCY 18000
#define MIN_POWER 0
#define MAX_POWER 10
#define MIN_STEP_FREQUENCY 0.02
#define MAX_STEP_FREQUENCY 100
#define MIN_STEP_TIME 0.01
#define MAX_STEP_TIME 1000
#define POWER_UNIT "dBm"
#define FREQUENCY_RANGE_GUIDE "6000 - 18000 MHz"
#define FREQUENCY_STEP_GUIDE "0.02 - 100 MHz"
#define STEP_TIME_GUIDE "0.01 - 1000 ms"
#define BANDWIDTH_GUIDE "2 - 100 MHz"
#define BANDWIDTH_LFM_GUIDE "0.001 - 1100 MHz"
#define LIFETIME_GUIDE "0.000001 - 1000 ms"
#define CENTER_FREQUENCY_GUIDE "6000 - 18000 MHz"

// Colors
#define CORNSILK     "rgb(248, 244, 216)"
#define GREEN_LIGHT  "rgb(61, 235, 13)"
#define PURPLE_DARK  "rgb(201, 175, 239)"
#define PURPLE_LIGHT "rgb(237, 203, 247)"
#define BLUE_DARK    "rgb(0, 93, 228)"
#define BLUE_LIGHT   "rgb(219, 233, 255)"
#define GRAY_LIGHT   "rgb(211, 215, 207)"
#define YELLOW_LIGHT   "rgb(255, 255, 0)"
#define RED_ERROR    "rgb(255, 98, 98)"
#define GREEN_        "#186a3b"

//saeid raziani
#define DARK_OLIVE_GREEN    "rgb(85, 107, 47)"
#define CRIMSON    "rgb(220, 20, 60)"
#define YELLOW_GREEN    "rgb(0, 255, 0)"
#define RED    "rgb(255, 0, 0)"
#define BLACK    "rgb(0, 0, 0)"
#define GE    "rgb(245, 176, 65)"

// Fonts
#define TITLE_FONT "Cantarell"
#define LABEL_FONT "Cantarell"
#define NUMBER_FONT "Monospace"
#define TITLE_FONT_SIZE 11
#define LABEL_FONT_SIZE 10
#define BUTTON_FONT_SIZE 10
#define SPINBOX_FONT_SIZE 11
#define TABLE_FONT_SIZE 9

// Sizes
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TITLE_HEIGHT 20
#define PLOT_HEIGHT 160
#define ROW_HEIGHT 25
#define ELEMENT_HEIGHT 30
#define ELEMENT_WIDTH 110



#endif // PROJECT_CONSTANTS_H
