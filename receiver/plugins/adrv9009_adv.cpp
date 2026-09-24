#include "adrv9009_adv.h"
#include "ui_adrv9009_adv.h"

#define PHY_DEVICE "adrv9009-phy"
#define DDS_DEVICE "axi-adrv9009-tx-hpc"
#define CAP_DEVICE "axi-adrv9009-rx-hpc"
#define THIS_DRIVER "adrv9009 Advanced"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

//OscPlot *plot_xcorr_4ch;

static bool can_update_widgets;

static gint this_page;
static gboolean plugin_detached;

enum adrv9009adv_wtype {
    CHECKBOX,
    SPINBUTTON,
    COMBOBOX,
    BUTTON,
    CHECKBOX_MASK,
    SPINBUTTON_S8,
    SPINBUTTON_S16,
};

static const char *adrv9009_adv_sr_attribs[] = {

    "debug.adrv9009-phy.adi,jesd204-rx-framer-bank-id",
    "debug.adrv9009-phy.acd..di,jesd204-rx-framer-device-id",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-lane0-id",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-m",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-k",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-scramble",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-external-sysref",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-serializer-lanes-enabled",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-serializer-lane-crossbar",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-serializer-amplitude",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-pre-emphasis",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-invert-lane-polarity",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-lmfc-offset",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-new-sysref-on-relink",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-enable-auto-chan-xbar",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-obs-rx-syncb-select",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-rx-syncb-mode",
    "debug.adrv9009-phy.adi,jesd204-rx-framer-over-sample",

    "debug.adrv9009-phy.adi,jesd204-obs-framer-bank-id",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-device-id",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-lane0-id",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-m",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-k",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-scramble",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-external-sysref",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-serializer-lanes-enabled",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-serializer-lane-crossbar",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-serializer-amplitude",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-pre-emphasis",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-invert-lane-polarity",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-lmfc-offset",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-new-sysref-on-relink",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-enable-auto-chan-xbar",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-obs-rx-syncb-select",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-rx-syncb-mode",
    "debug.adrv9009-phy.adi,jesd204-obs-framer-over-sample",


    "debug.adrv9009-phy.adi,jesd204-deframer-bank-id",
    "debug.adrv9009-phy.adi,jesd204-deframer-device-id",
    "debug.adrv9009-phy.adi,jesd204-deframer-lane0-id",
    "debug.adrv9009-phy.adi,jesd204-deframer-m",
    "debug.adrv9009-phy.adi,jesd204-deframer-k",
    "debug.adrv9009-phy.adi,jesd204-deframer-scramble",
    "debug.adrv9009-phy.adi,jesd204-deframer-external-sysref",
    "debug.adrv9009-phy.adi,jesd204-deframer-deserializer-lanes-enabled",
    "debug.adrv9009-phy.adi,jesd204-deframer-deserializer-lane-crossbar",
    "debug.adrv9009-phy.adi,jesd204-deframer-eq-setting",
    "debug.adrv9009-phy.adi,jesd204-deframer-invert-lane-polarity",
    "debug.adrv9009-phy.adi,jesd204-deframer-lmfc-offset",
    "debug.adrv9009-phy.adi,jesd204-deframer-new-sysref-on-relink",
    "debug.adrv9009-phy.adi,jesd204-deframer-enable-auto-chan-xbar",
    "debug.adrv9009-phy.adi,jesd204-deframer-tx-syncb-mode",

    "debug.adrv9009-phy.adi,rx-gain-mode",
    "debug.adrv9009-phy.adi,rx1-gain-index",
    "debug.adrv9009-phy.adi,rx2-gain-index",
    "debug.adrv9009-phy.adi,rx1-max-gain-index",
    "debug.adrv9009-phy.adi,rx1-min-gain-index",
    "debug.adrv9009-phy.adi,rx2-max-gain-index",
    "debug.adrv9009-phy.adi,rx2-min-gain-index",

    "debug.adrv9009-phy.adi,orx-gain-mode",
    "debug.adrv9009-phy.adi,orx1-gain-index",
    "debug.adrv9009-phy.adi,orx2-gain-index",
    "debug.adrv9009-phy.adi,orx-max-gain-index",
    "debug.adrv9009-phy.adi,orx-min-gain-index",

    "debug.adrv9009-phy.adi,sniffer-gain-mode",
    "debug.adrv9009-phy.adi,sniffer-gain-index",
    "debug.adrv9009-phy.adi,sniffer-max-gain-index",
    "debug.adrv9009-phy.adi,sniffer-min-gain-index",

    "debug.adrv9009-phy.adi,rx-peak-agc-apd-high-thresh",
    "debug.adrv9009-phy.adi,rx-peak-agc-apd-low-thresh",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-high-thresh",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-low-thresh",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-very-low-thresh",
    "debug.adrv9009-phy.adi,rx-peak-agc-apd-high-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,rx-peak-agc-apd-low-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-high-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-low-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-very-low-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,rx-peak-agc-apd-high-gain-step-attack",
    "debug.adrv9009-phy.adi,rx-peak-agc-apd-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-high-gain-step-attack",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-very-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,rx-peak-agc-apd-fast-attack",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-fast-attack",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-overload-detect-enable",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-overload-duration-cnt",
    "debug.adrv9009-phy.adi,rx-peak-agc-hb2-overload-thresh-cnt",

    "debug.adrv9009-phy.adi,obs-peak-agc-apd-high-thresh",
    "debug.adrv9009-phy.adi,obs-peak-agc-apd-low-thresh",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-high-thresh",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-low-thresh",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-very-low-thresh",
    "debug.adrv9009-phy.adi,obs-peak-agc-apd-high-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,obs-peak-agc-apd-low-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-high-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-low-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-very-low-thresh-exceeded-cnt",
    "debug.adrv9009-phy.adi,obs-peak-agc-apd-high-gain-step-attack",
    "debug.adrv9009-phy.adi,obs-peak-agc-apd-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-high-gain-step-attack",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-very-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,obs-peak-agc-apd-fast-attack",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-fast-attack",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-overload-detect-enable",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-overload-duration-cnt",
    "debug.adrv9009-phy.adi,obs-peak-agc-hb2-overload-thresh-cnt",

    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-upper-high-thresh",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-upper-low-thresh",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-lower-high-thresh",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-lower-low-thresh",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-upper-high-gain-step-attack",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-upper-low-gain-step-attack",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-lower-high-gain-step-recovery",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-lower-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-meas-duration",
    "debug.adrv9009-phy.adi,rx-pwr-agc-pmd-meas-config",

    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-upper-high-thresh",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-upper-low-thresh",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-lower-high-thresh",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-lower-low-thresh",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-upper-high-gain-step-attack",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-upper-low-gain-step-attack",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-lower-high-gain-step-recovery",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-lower-low-gain-step-recovery",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-meas-duration",
    "debug.adrv9009-phy.adi,obs-pwr-agc-pmd-meas-config",

    "debug.adrv9009-phy.adi,rx-agc-conf-agc-rx1-max-gain-index",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-rx1-min-gain-index",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-rx2-max-gain-index",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-rx2-min-gain-index",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-peak-threshold-mode",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-low-ths-prevent-gain-increase",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-gain-update-counter",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-slow-loop-settling-delay",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-peak-wait-time",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-reset-on-rx-enable",
    "debug.adrv9009-phy.adi,rx-agc-conf-agc-enable-sync-pulse-for-gain-counter",

    "debug.adrv9009-phy.adi,obs-agc-conf-agc-obs-rx-max-gain-index",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-obs-rx-min-gain-index",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-obs-rx-select",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-peak-threshold-mode",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-low-ths-prevent-gain-increase",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-gain-update-counter",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-slow-loop-settling-delay",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-peak-wait-time",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-reset-on-rx-enable",
    "debug.adrv9009-phy.adi,obs-agc-conf-agc-enable-sync-pulse-for-gain-counter",

    "debug.adrv9009-phy.adi,rx-profile-adc-div",
    "debug.adrv9009-phy.adi,rx-profile-rx-fir-decimation",
    "debug.adrv9009-phy.adi,rx-profile-rx-dec5-decimation",
    "debug.adrv9009-phy.adi,rx-profile-en-high-rej-dec5",
    "debug.adrv9009-phy.adi,rx-profile-rhb1-decimation",
    "debug.adrv9009-phy.adi,rx-profile-iq-rate_khz",
    "debug.adrv9009-phy.adi,rx-profile-rf-bandwidth_hz",
    "debug.adrv9009-phy.adi,rx-profile-rx-bbf-3db-corner_khz",

    "debug.adrv9009-phy.adi,obs-profile-adc-div",
    "debug.adrv9009-phy.adi,obs-profile-rx-fir-decimation",
    "debug.adrv9009-phy.adi,obs-profile-rx-dec5-decimation",
    "debug.adrv9009-phy.adi,obs-profile-en-high-rej-dec5",
    "debug.adrv9009-phy.adi,obs-profile-rhb1-decimation",
    "debug.adrv9009-phy.adi,obs-profile-iq-rate_khz",
    "debug.adrv9009-phy.adi,obs-profile-rf-bandwidth_hz",
    "debug.adrv9009-phy.adi,obs-profile-rx-bbf-3db-corner_khz",

    "debug.adrv9009-phy.adi,sniffer-profile-adc-div",
    "debug.adrv9009-phy.adi,sniffer-profile-rx-fir-decimation",
    "debug.adrv9009-phy.adi,sniffer-profile-rx-dec5-decimation",
    "debug.adrv9009-phy.adi,sniffer-profile-en-high-rej-dec5",
    "debug.adrv9009-phy.adi,sniffer-profile-rhb1-decimation",
    "debug.adrv9009-phy.adi,sniffer-profile-iq-rate_khz",
    "debug.adrv9009-phy.adi,sniffer-profile-rf-bandwidth_hz",
    "debug.adrv9009-phy.adi,sniffer-profile-rx-bbf-3db-corner_khz",

    "debug.adrv9009-phy.adi,tx-profile-dac-div",
    "debug.adrv9009-phy.adi,tx-profile-tx-fir-interpolation",
    "debug.adrv9009-phy.adi,tx-profile-thb1-interpolation",
    "debug.adrv9009-phy.adi,tx-profile-thb2-interpolation",
    "debug.adrv9009-phy.adi,tx-profile-tx-input-hb-interpolation",
    "debug.adrv9009-phy.adi,tx-profile-iq-rate_khz",
    "debug.adrv9009-phy.adi,tx-profile-primary-sig-bandwidth_hz",
    "debug.adrv9009-phy.adi,tx-profile-rf-bandwidth_hz",
    "debug.adrv9009-phy.adi,tx-profile-tx-dac-3db-corner_khz",
    "debug.adrv9009-phy.adi,tx-profile-tx-bbf-3db-corner_khz",

    "debug.adrv9009-phy.adi,clocks-device-clock_khz",
    "debug.adrv9009-phy.adi,clocks-clk-pll-vco-freq_khz",
    "debug.adrv9009-phy.adi,clocks-clk-pll-vco-div",
    "debug.adrv9009-phy.adi,clocks-clk-pll-hs-div",

    "debug.adrv9009-phy.adi,tx-settings-tx-channels-enable",
    "debug.adrv9009-phy.adi,tx-settings-tx-pll-use-external-lo",
    "debug.adrv9009-phy.adi,tx-settings-tx-pll-lo-frequency_hz",
    "debug.adrv9009-phy.adi,tx-settings-tx-atten-step-size",
    "debug.adrv9009-phy.adi,tx-settings-tx1-atten_mdb",
    "debug.adrv9009-phy.adi,tx-settings-tx2-atten_mdb",

    "debug.adrv9009-phy.adi,rx-settings-rx-channels-enable",
    "debug.adrv9009-phy.adi,rx-settings-rx-pll-use-external-lo",
    "debug.adrv9009-phy.adi,rx-settings-rx-pll-lo-frequency_hz",
    "debug.adrv9009-phy.adi,rx-settings-real-if-data",

    "debug.adrv9009-phy.adi,obs-settings-obs-rx-channels-enable",
    "debug.adrv9009-phy.adi,obs-settings-obs-rx-lo-source",
    "debug.adrv9009-phy.adi,obs-settings-sniffer-pll-lo-frequency_hz",
    "debug.adrv9009-phy.adi,obs-settings-real-if-data",
    "debug.adrv9009-phy.adi,obs-settings-default-obs-rx-channel",

    "debug.adrv9009-phy.adi,arm-gpio-use-rx2-enable-pin",
    "debug.adrv9009-phy.adi,arm-gpio-use-tx2-enable-pin",
    "debug.adrv9009-phy.adi,arm-gpio-tx-rx-pin-mode",
    "debug.adrv9009-phy.adi,arm-gpio-orx-pin-mode",
    "debug.adrv9009-phy.adi,arm-gpio-orx-trigger-pin",
    "debug.adrv9009-phy.adi,arm-gpio-orx-mode2-pin",
    "debug.adrv9009-phy.adi,arm-gpio-orx-mode1-pin",
    "debug.adrv9009-phy.adi,arm-gpio-orx-mode0-pin",
    "debug.adrv9009-phy.adi,arm-gpio-rx1-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-rx2-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-tx1-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-tx2-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-orx1-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-orx2-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-srx-enable-ack",
    "debug.adrv9009-phy.adi,arm-gpio-tx-obs-select",
    "debug.adrv9009-phy.adi,arm-gpio-enable-mask",

    "debug.adrv9009-phy.adi,gpio-3v3-oe-mask",
    "debug.adrv9009-phy.adi,gpio-3v3-src-ctrl3_0",
    "debug.adrv9009-phy.adi,gpio-3v3-src-ctrl7_4",
    "debug.adrv9009-phy.adi,gpio-3v3-src-ctrl11_8",

    "debug.adrv9009-phy.adi,gpio-oe-mask",
    "debug.adrv9009-phy.adi,gpio-src-ctrl3_0",
    "debug.adrv9009-phy.adi,gpio-src-ctrl7_4",
    "debug.adrv9009-phy.adi,gpio-src-ctrl11_8",
    "debug.adrv9009-phy.adi,gpio-src-ctrl15_12",
    "debug.adrv9009-phy.adi,gpio-src-ctrl18_16",

    "debug.adrv9009-phy.adi,aux-dac-enable-mask",
    "debug.adrv9009-phy.adi,aux-dac-value0",
    "debug.adrv9009-phy.adi,aux-dac-slope0",
    "debug.adrv9009-phy.adi,aux-dac-vref0",
    "debug.adrv9009-phy.adi,aux-dac-value1",
    "debug.adrv9009-phy.adi,aux-dac-slope1",
    "debug.adrv9009-phy.adi,aux-dac-vref1",
    "debug.adrv9009-phy.adi,aux-dac-value2",
    "debug.adrv9009-phy.adi,aux-dac-slope2",
    "debug.adrv9009-phy.adi,aux-dac-vref2",
    "debug.adrv9009-phy.adi,aux-dac-value3",
    "debug.adrv9009-phy.adi,aux-dac-slope3",
    "debug.adrv9009-phy.adi,aux-dac-vref3",
    "debug.adrv9009-phy.adi,aux-dac-value4",
    "debug.adrv9009-phy.adi,aux-dac-slope4",
    "debug.adrv9009-phy.adi,aux-dac-vref4",
    "debug.adrv9009-phy.adi,aux-dac-value5",
    "debug.adrv9009-phy.adi,aux-dac-slope5",
    "debug.adrv9009-phy.adi,aux-dac-vref5",
    "debug.adrv9009-phy.adi,aux-dac-value6",
    "debug.adrv9009-phy.adi,aux-dac-slope6",
    "debug.adrv9009-phy.adi,aux-dac-vref6",
    "debug.adrv9009-phy.adi,aux-dac-value7",
    "debug.adrv9009-phy.adi,aux-dac-slope7",
    "debug.adrv9009-phy.adi,aux-dac-vref7",
    "debug.adrv9009-phy.adi,aux-dac-value8",
    "debug.adrv9009-phy.adi,aux-dac-slope8",
    "debug.adrv9009-phy.adi,aux-dac-vref8",
    "debug.adrv9009-phy.adi,aux-dac-value9",
    "debug.adrv9009-phy.adi,aux-dac-slope9",
    "debug.adrv9009-phy.adi,aux-dac-vref9",

    "debug.adi,default-initial-calibrations-mask",

    "debug.adrv9009-phy.adi,dpd-damping",
    "debug.adrv9009-phy.adi,dpd-num-weights",
    "debug.adrv9009-phy.adi,dpd-model-version",
    "debug.adrv9009-phy.adi,dpd-high-power-model-update",
    "debug.adrv9009-phy.adi,dpd-model-prior-weight",
    "debug.adrv9009-phy.adi,dpd-robust-modeling",
    "debug.adrv9009-phy.adi,dpd-samples",
    "debug.adrv9009-phy.adi,dpd-outlier-threshold",
    "debug.adrv9009-phy.adi,dpd-additional-delay-offset",
    "debug.adrv9009-phy.adi,dpd-path-delay-pn-seq-level",
    "debug.adrv9009-phy.adi,dpd-weights0-real",
    "debug.adrv9009-phy.adi,dpd-weights0-imag",
    "debug.adrv9009-phy.adi,dpd-weights1-real",
    "debug.adrv9009-phy.adi,dpd-weights1-imag",
    "debug.adrv9009-phy.adi,dpd-weights2-real",
    "debug.adrv9009-phy.adi,dpd-weights2-imag",

    "debug.adrv9009-phy.adi,clgc-tx1-desired-gain",
    "debug.adrv9009-phy.adi,clgc-tx2-desired-gain",
    "debug.adrv9009-phy.adi,clgc-tx1-atten-limit",
    "debug.adrv9009-phy.adi,clgc-tx2-atten-limit",
    "debug.adrv9009-phy.adi,clgc-tx1-control-ratio",
    "debug.adrv9009-phy.adi,clgc-tx2-control-ratio",
    "debug.adrv9009-phy.adi,clgc-allow-tx1-atten-updates",
    "debug.adrv9009-phy.adi,clgc-allow-tx2-atten-updates",
    "debug.adrv9009-phy.adi,clgc-additional-delay-offset",
    "debug.adrv9009-phy.adi,clgc-path-delay-pn-seq-level",
    "debug.adrv9009-phy.adi,clgc-tx1-rel-threshold",
    "debug.adrv9009-phy.adi,clgc-tx2-rel-threshold",
    "debug.adrv9009-phy.adi,clgc-tx1-rel-threshold-en",
    "debug.adrv9009-phy.adi,clgc-tx2-rel-threshold-en",

    "debug.adrv9009-phy.adi,vswr-additional-delay-offset",
    "debug.adrv9009-phy.adi,vswr-path-delay-pn-seq-level",
    "debug.adrv9009-phy.adi,vswr-tx1-vswr-switch-gpio3p3-pin",
    "debug.adrv9009-phy.adi,vswr-tx2-vswr-switch-gpio3p3-pin",
    "debug.adrv9009-phy.adi,vswr-tx1-vswr-switch-polarity",
    "debug.adrv9009-phy.adi,vswr-tx2-vswr-switch-polarity",
    "debug.adrv9009-phy.adi,vswr-tx1-vswr-switch-delay_us",
    "debug.adrv9009-phy.adi,vswr-tx2-vswr-switch-delay_us",
};

adrv9009_adv::adrv9009_adv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adrv9009_adv)
{
    ui->setupUi(this);

    ui->mainTabContainer->setTabPosition(QTabWidget::West);


    //ui->fhm_chk_trigger_mode->setChecked(true);

    //saeid raziani
    chk_trigger_mode   = ui->fhm_chk_trigger_mode;
    chk_exit_mode      = ui->fhm_chk_exit_mode;
    chk_msc_sync       = ui->fhm_chk_msc_sync;
    chk_enable         = ui->fhm_chk_enable;

    txt_init_frequency = ui->fhm_txt_init_frequency;
    txt_gpio_pin       = ui->fhm_txt_gpio_pin;
    txt_min_freq       = ui->fhm_txt_min_freq;
    txt_max_freq       = ui->fhm_txt_max_freq;
    save_settings      = ui->btn_save_settings;

    connect(ui->fhm_chk_trigger_mode, &QCheckBox::stateChanged,[&](int state)
    {
//        if (state == 0)
//        {
//            ui->fhm_chk_trigger_mode->setChecked(true);
//            ui->fhm_chk_trigger_mode->stateChanged(1);
//        }
    });



    connect(ui->fhm_chk_exit_mode, &QCheckBox::stateChanged,
            [&](int state){
        if (state == 0) ui->fhm_chk_exit_mode->setChecked(true);
    });

    connect(ui->fhm_chk_msc_sync, &QCheckBox::stateChanged,
            [&](int state){
        if (state == 0) ui->fhm_chk_msc_sync->setChecked(true);
    });

    connect(ui->fhm_chk_enable, &QCheckBox::stateChanged,
            [&](int state){
        if (state == 0) ui->fhm_chk_enable->setChecked(true);
    });

    //end
}

adrv9009_adv::~adrv9009_adv()
{
    delete ui;
}

/**
 * @brief adrv9009_adv::AddWidgetToList
 * @param widget
 * @param name
 * @param lut
 * @param lut_len
 */
void adrv9009_adv::AddWidgetToList(QWidget *widget, char *name, unsigned char * const lut,  unsigned char lut_len)
{
    w_info *temp=new w_info();

    temp->widget=widget;
    temp->name=name;
    temp->lut=lut;
    temp->lut_len=lut_len;

    globals::attrs->append(temp);
}

/**
 * @brief adrv9009_adv::InitializeWidgets
 */
void adrv9009_adv::InitializeWidgets(){

#pragma region CLK Settings {

    AddWidgetToList(ui->clk_settings_txt_device_clock, "adi,dig-clocks-device-clock_khz", NULL, 0);
    AddWidgetToList(ui->clk_settings_txt_clk_pll_vco_freq, "adi,dig-clocks-clk-pll-vco-freq_khz", NULL, 0);
    AddWidgetToList(ui->clk_settings_cmb_pll_vco_div, "adi,dig-clocks-clk-pll-hs-div", new (unsigned char[5]){0,1,2,3,4}, 5);
    AddWidgetToList(ui->clk_settings_cmb_clk_pll_phase_sync_mode, "adi,dig-clocks-rf-pll-phase-sync-mode", new(unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->clk_settings_chk_rf_pll_use_external_lo, "adi,dig-clocks-rf-pll-use-external-lo", NULL, 0);

#pragma endregion }

#pragma region Calibration {
    AddWidgetToList(ui->calibration_chk_tx_lol, "adi,default-initial-calibrations-mask#8", NULL, 0);
    AddWidgetToList(ui->calibration_chk_external_tx_lol, "adi,default-initial-calibrations-mask#9", NULL, 0);
    AddWidgetToList(ui->calibration_chk_tx_qec, "adi,default-initial-calibrations-mask#10", NULL, 0);
    AddWidgetToList(ui->calibration_chk_rx_qec, "adi,default-initial-calibrations-mask#14", NULL, 0);
    AddWidgetToList(ui->calibration_chk_rx_phase_correction, "adi,default-initial-calibrations-mask#15", NULL, 0);
    AddWidgetToList(ui->calibration_chk_fhm, "adi,default-initial-calibrations-mask#23", NULL, 0);
#pragma endregion }

#pragma region TX Settings {
    AddWidgetToList(ui->tx_settings_cmb_dac_div, "adi,tx-profile-dac-div",new (unsigned char[2]){1,2}, 2);
    AddWidgetToList(ui->tx_settings_cmb_tx_fir, "adi,tx-profile-tx-fir-interpolation",new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->tx_settings_cmb_tbh1, "adi,tx-profile-thb1-interpolation",new (unsigned char[2]){1,2}, 2);
    AddWidgetToList(ui->tx_settings_cmb_tbh2, "adi,tx-profile-thb2-interpolation",new (unsigned char[2]){1,2}, 2);
    AddWidgetToList(ui->tx_settings_cmb_tbh3, "adi,tx-profile-thb3-interpolation",new (unsigned char[2]){1,2}, 2);
    AddWidgetToList(ui->tx_settings_cmb_tx_int5, "adi,tx-profile-tx-int5-interpolation",new (unsigned char[2]){1,5}, 2);
    AddWidgetToList(ui->tx_settings_txt_tx_input_rate, "adi,tx-profile-tx-input-rate_khz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_primary_sig_bandwidth_hz, "adi,tx-profile-primary-sig-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_rf_bandwidth_hz, "adi,tx-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx_dac_3db_corner_khz, "adi,tx-profile-tx-dac3d-bcorner_khz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx_bbf_3db_corner_khz, "adi,tx-profile-tx-bbf3d-bcorner_khz", NULL, 0);
    AddWidgetToList(ui->tx_settings_cmb_deframer_sel, "adi,tx-settings-deframer-sel",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->tx_settings_cmb_channel_enable, "adi,tx-settings-tx-channels",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->tx_settings_cmb_step_size, "adi,tx-settings-tx-atten-step-size",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->tx_settings_txt_tx1_atten, "adi,tx-settings-tx1-atten_md-b", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx2_atten, "adi,tx-settings-tx2-atten_md-b", NULL, 0);
    AddWidgetToList(ui->tx_settings_cmb_data_if_pll_unlock, "adi,tx-settings-dis-tx-data-if-pll-unlock",new (unsigned char[3]){0,1,2}, 3);

    AddWidgetToList(ui->tx_settings_txt_tx1_step_size, "adi,tx1-atten-ctrl-pin-step-size", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx1_inc_pin, "adi,tx1-atten-ctrl-pin-tx-atten-inc-pin",new (unsigned char[2]){4,12}, 2);
    AddWidgetToList(ui->tx_settings_txt_tx1_dec_pin, "adi,tx1-atten-ctrl-pin-tx-atten-dec-pin",new (unsigned char[2]){5,13}, 2);
    AddWidgetToList(ui->tx_settings_chk_tx1_pin_enable, "adi,tx1-atten-ctrl-pin-enable", NULL, 0);

    AddWidgetToList(ui->tx_settings_txt_tx2_step_size, "adi,tx2-atten-ctrl-pin-step-size", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx2_inc_pin, "adi,tx2-atten-ctrl-pin-tx-atten-inc-pin",new (unsigned char[2]){6,14}, 2);
    AddWidgetToList(ui->tx_settings_txt_tx2_dec_pin, "adi,tx2-atten-ctrl-pin-tx-atten-dec-pin",new (unsigned char[2]){7,15}, 2);
    AddWidgetToList(ui->tx_settings_chk_tx2_pin_enable, "adi,tx2-atten-ctrl-pin-enable", NULL, 0);
#pragma endregion }

#pragma region RX Settings {

    AddWidgetToList(ui->rx_settings_cmb_channel_enable, "adi,rx-settings-rx-channels",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->rx_settings_cmb_jesd204_framer_sel, "adi,rx-settings-framer-sel",new (unsigned char[3]){0,1,2}, 3);

    AddWidgetToList(ui->rx_settings_profile_cmb_rx_fir, "adi,rx-profile-rx-fir-decimation",new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->rx_settings_profile_cmb_rx_dec, "adi,rx-profile-rx-dec5-decimation",new (unsigned char[2]){4,5}, 2);
    AddWidgetToList(ui->rx_settings_profile_cmb_rhb1, "adi,rx-profile-rhb1-decimation",new (unsigned char[2]){1,2}, 2);
    AddWidgetToList(ui->rx_settings_profile_txt_rx_output, "adi,rx-profile-rx-output-rate_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_profile_txt_rf_bandwidth, "adi,rx-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->rx_settings_profile_txt_rx_bbf_3db_corner_khz, "adi,rx-profile-rx-bbf3d-bcorner_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_profile_cmb_rx_ddc, "adi,rx-profile-rx-ddc-mode",new (unsigned char[8]){0,1,2,3,4,5,6,7}, 8);

    AddWidgetToList(ui->rx_settings_shifter_txt_a_input_band, "adi,rx-nco-shifter-band-a-input-band-width_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_a_input_center, "adi,rx-nco-shifter-band-a-input-center-freq_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_a_nco_1, "adi,rx-nco-shifter-band-a-nco1-freq_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_a_nco_2, "adi,rx-nco-shifter-band-a-nco2-freq_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_b_input_band, "adi,rx-nco-shifter-band-binput-band-width_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_b_input_center, "adi,rx-nco-shifter-band-binput-center-freq_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_b_nco_1, "adi,rx-nco-shifter-band-bnco1-freq_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_shifter_txt_b_nco_2, "adi,rx-nco-shifter-band-bnco2-freq_khz", NULL, 0);

    AddWidgetToList(ui->rx_settings_gpio_txt_rx1_inc_step, "adi,rx1-gain-ctrl-pin-inc-step", NULL, 0);
    AddWidgetToList(ui->rx_settings_gpio_txt_rx1_dec_step, "adi,rx1-gain-ctrl-pin-dec-step", NULL, 0);
    AddWidgetToList(ui->rx_settings_gpio_cmb_rx1_gain_inc, "adi,rx1-gain-ctrl-pin-rx-gain-inc-pin",new (unsigned char[2]){0,10}, 2);
    AddWidgetToList(ui->rx_settings_gpio_cmb_rx1_gain_dec, "adi,rx1-gain-ctrl-pin-rx-gain-dec-pin",new (unsigned char[2]){1,11}, 2);
    AddWidgetToList(ui->rx_settings_gpio_chk_rx1_enable, "adi,rx1-gain-ctrl-pin-enable", NULL, 0);

    AddWidgetToList(ui->rx_settings_gpio_txt_rx2_inc_step, "adi,rx2-gain-ctrl-pin-inc-step", NULL, 0);
    AddWidgetToList(ui->rx_settings_gpio_txt_rx2_dec_step, "adi,rx2-gain-ctrl-pin-dec-step", NULL, 0);
    AddWidgetToList(ui->rx_settings_gpio_cmb_rx2_gain_inc, "adi,rx2-gain-ctrl-pin-rx-gain-inc-pin",new (unsigned char[2]){3,13}, 2);
    AddWidgetToList(ui->rx_settings_gpio_cmb_rx2_gain_dec, "adi,rx2-gain-ctrl-pin-rx-gain-dec-pin",new (unsigned char[2]){4,14}, 2);
    AddWidgetToList(ui->rx_settings_gpio_chk_rx2_enable, "adi,rx2-gain-ctrl-pin-enable", NULL, 0);

#pragma endregion }

#pragma region ORX Settings {

    AddWidgetToList(ui->obs_settings_cmb_framer_sel,"adi,obs-settings-framer-sel",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->obs_settings_cmb_channel_enable, "adi,obs-settings-obs-rx-channels-enable",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->obs_settings_cmb_lo_source, "adi,obs-settings-obs-rx-lo-source",new (unsigned char[2]){0,1}, 2);

    AddWidgetToList(ui->obs_settings_cmb_rx_fir, "adi,orx-profile-rx-fir-decimation",new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->obs_settings_cmb_dec5, "adi,orx-profile-rx-dec5-decimation",new (unsigned char[2]){4,5}, 2);
    AddWidgetToList(ui->obs_settings_cmb_rhb1, "adi,orx-profile-rhb1-decimation",new (unsigned char[2]){1,2}, 2);
    AddWidgetToList(ui->obs_settings_txt_orx_output, "adi,orx-profile-orx-output-rate_khz", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_rf_bandwidth, "adi,orx-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_bbf_3db, "adi,orx-profile-rx-bbf3d-bcorner_khz", NULL, 0);
    AddWidgetToList(ui->obs_settings_cmb_orx_ddc, "adi,orx-profile-orx-ddc-mode",new (unsigned char[1]){7}, 1);

    AddWidgetToList(ui->obs_settings_chk_aux_pll_relocking, "adi,orx-lo-cfg-disable-aux-pll-relocking", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_aux_pll, "adi,orx-lo-cfg-gpio-select", NULL, 0);

#pragma endregion }

#pragma region FHM Mode {

    AddWidgetToList(ui->fhm_txt_gpio_pin, "adi,fhm-config-fhm-gpio-pin", NULL, 0);
    AddWidgetToList(ui->fhm_txt_min_freq, "adi,fhm-config-fhm-min-freq_mhz", NULL, 0);
    AddWidgetToList(ui->fhm_txt_max_freq, "adi,fhm-config-fhm-max-freq_mhz", NULL, 0);
    AddWidgetToList(ui->fhm_chk_enable, "adi,fhm-mode-fhm-enable", NULL, 0);
    AddWidgetToList(ui->fhm_chk_msc_sync, "adi,fhm-mode-enable-mcs-sync", NULL, 0);
    AddWidgetToList(ui->fhm_chk_trigger_mode, "adi,fhm-mode-fhm-trigger-mode", NULL, 0);
    AddWidgetToList(ui->fhm_chk_exit_mode, "adi,fhm-mode-fhm-exit-mode", NULL, 0);
    AddWidgetToList(ui->fhm_txt_init_frequency, "adi,fhm-mode-fhm-init-frequency_hz", NULL, 0);

#pragma endregion }

#pragma region PA Protection {
    AddWidgetToList(ui->pa_protection_txt_avg_duration, "adi,tx-pa-protection-avg-duration", NULL, 0);
    AddWidgetToList(ui->pa_protection_txt_tx_atten_step, "adi,tx-pa-protection-tx-atten-step", NULL, 0);
    AddWidgetToList(ui->pa_protection_txt_tx1_power, "adi,tx-pa-protection-tx1-power-threshold", NULL, 0);
    AddWidgetToList(ui->pa_protection_txt_tx2_power, "adi,tx-pa-protection-tx2-power-threshold", NULL, 0);
    AddWidgetToList(ui->pa_protection_txt_peak_count, "adi,tx-pa-protection-peak-count", NULL, 0);
    AddWidgetToList(ui->pa_protection_txt_tx1_peak, "adi,tx-pa-protection-tx1-peak-threshold", NULL, 0);
    AddWidgetToList(ui->pa_protection_txt_tx2_peak, "adi,tx-pa-protection-tx2-peak-threshold", NULL, 0);
#pragma endregion }

#pragma region GAIN Setup {

    AddWidgetToList(ui->gain_setup_cmb_rx_gain_control_mode, "adi,rx-gain-control-gain-mode",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->gain_setup_txt_rx1_gain_index, "adi,rx-gain-control-rx1-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx2_gain_index, "adi,rx-gain-control-rx2-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx1_max_gain_index, "adi,rx-gain-control-rx1-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx1_min_gain_index, "adi,rx-gain-control-rx1-min-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx2_max_gain_index, "adi,rx-gain-control-rx2-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx2_min_gain_index, "adi,rx-gain-control-rx2-min-gain-index", NULL, 0);

    AddWidgetToList(ui->gain_setup_cmb_observation_gain_control_mode, "adi,orx-gain-control-gain-mode",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->gain_setup_txt_observation_orx1_gain_index, "adi,orx-gain-control-orx1-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_orx2_gain_index, "adi,orx-gain-control-orx2-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_orx1_max_gain_index, "adi,orx-gain-control-orx1-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_orx1_min_gain_index, "adi,orx-gain-control-orx1-min-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_orx2_max_gain_index, "adi,orx-gain-control-orx2-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_orx2_min_gain_index, "adi,orx-gain-control-orx2-min-gain-index", NULL, 0);

#pragma endregion }

#pragma region AGC Setup {

    AddWidgetToList(ui->agc_setup_txt_peak_wait_time, "adi,rxagc-agc-peak-wait-time", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_max_gain_index, "adi,rxagc-agc-rx1-max-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_min_gain_index, "adi,rxagc-agc-rx1-min-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx2_max_gain_index, "adi,rxagc-agc-rx2-max-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx2_min_gain_index, "adi,rxagc-agc-rx2-min-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_gain_update_counter, "adi,rxagc-agc-gain-update-counter_us", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_attack_delay, "adi,rxagc-agc-rx1-attack-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx2_attack_delay, "adi,rxagc-agc-rx2-attack-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_slow_loop_delay, "adi,rxagc-agc-slow-loop-settling-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_low_threshold_prevent, "adi,rxagc-agc-low-thresh-prevent-gain", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_change_gain_if_thresh, "adi,rxagc-agc-change-gain-if-thresh-high", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_peak_threshold_gain_mode, "adi,rxagc-agc-peak-thresh-gain-control-mode", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_reset_on_rx_on, "adi,rxagc-agc-reset-on-rxon", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_enable_sync_pulse_for_gain, "adi,rxagc-agc-enable-sync-pulse-for-gain-counter", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_enable_ip3_optimization, "adi,rxagc-agc-enable-ip3-optimization-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_ip3_over_range, "adi,rxagc-ip3-over-range-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_ip3_over_range_index, "adi,rxagc-ip3-over-range-thresh-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_ip3_peak_exceeded, "adi,rxagc-ip3-peak-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_enable_fast_recovery, "adi,rxagc-agc-enable-fast-recovery-loop", NULL, 0);

    AddWidgetToList(ui->agc_setup_txt_apd_under_range_low, "adi,rxagc-peak-agc-under-range-low-interval_ns", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_under_range_mid, "adi,rxagc-peak-agc-under-range-mid-interval", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_under_range_high, "adi,rxagc-peak-agc-under-range-high-interval", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_high_thresh, "adi,rxagc-peak-apd-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_gain_mode_high_thresh, "adi,rxagc-peak-apd-low-gain-mode-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_thresh, "adi,rxagc-peak-apd-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_gain_mode_low_thresh, "adi,rxagc-peak-apd-low-gain-mode-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_upper_thresh_peak, "adi,rxagc-peak-apd-upper-thresh-peak-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_lower_thresh_peak, "adi,rxagc-peak-apd-lower-thresh-peak-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_gain_step_attack, "adi,rxagc-peak-apd-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_gain_step_recovery, "adi,rxagc-peak-apd-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_apd_enable_hb2_overload, "adi,rxagc-peak-enable-hb2-overload", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_overload_duration, "adi,rxagc-peak-hb2-overload-duration-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_overload_thresh, "adi,rxagc-peak-hb2-overload-thresh-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_hig_thresh, "adi,rxagc-peak-hb2-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_under_range_low, "adi,rxagc-peak-hb2-under-range-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_under_range_mid, "adi,rxagc-peak-hb2-under-range-mid-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_under_range_high, "adi,rxagc-peak-hb2-under-range-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_upper_thesh_peak, "adi,rxagc-peak-hb2-upper-thresh-peak-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_lower_thesh_peak, "adi,rxagc-peak-hb2-lower-thresh-peak-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_gain_step_high, "adi,rxagc-peak-hb2-gain-step-high-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_gain_step_low, "adi,rxagc-peak-hb2-gain-step-low-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_hb2_gain_step_mid, "adi,rxagc-peak-hb2-gain-step-mid-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_gain_step_attack, "adi,rxagc-peak-hb2-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_apd_hb2_overload_power_mode, "adi,rxagc-peak-hb2-overload-power-mode", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_apd_hb2_overg_sel, "adi,rxagc-peak-hb2-ovrg-sel", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_apd_hb2_thresh_config, "adi,rxagc-peak-hb2-thresh-config", NULL, 0);

    AddWidgetToList(ui->agc_setup_chk_power_enable_meas, "adi,rxagc-power-power-enable-measurement", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_power_use_rfir_out, "adi,rxagc-power-power-use-rfir-out", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_power_use_bbdc2, "adi,rxagc-power-power-use-bbdc2", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_under_range_high_power, "adi,rxagc-power-under-range-high-power-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_under_range_low_power, "adi,rxagc-power-under-range-low-power-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_under_range_high_power_gain_step_recover, "adi,rxagc-power-under-range-high-power-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_under_range_low_power_gain_step_recover, "adi,rxagc-power-under-range-low-power-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_power_meas_duration, "adi,rxagc-power-power-measurement-duration", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_tdd_power_meas_duration, "adi,rxagc-power-rx1-tdd-power-meas-duration", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_tdd_power_meas_delay, "adi,rxagc-power-rx1-tdd-power-meas-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_tdd_power_meas_duration_2, "adi,rxagc-power-rx2-tdd-power-meas-duration", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx1_tdd_power_meas_delay_2, "adi,rxagc-power-rx2-tdd-power-meas-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_upper0_power_thresh, "adi,rxagc-power-upper0-power-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_upper1_power_thresh, "adi,rxagc-power-upper1-power-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_power_log_shift, "adi,rxagc-power-power-log-shift", NULL, 0);

#pragma endregion }

#pragma region ARM GPIO {

    AddWidgetToList(ui->arm_gpio_chk_orx1_tx_enable_sel_0, "adi,arm-gpio-config-orx1-tx-sel0-pin-gpio-pin-sel", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx1_tx_sel0_gpio_pin, "adi,arm-gpio-config-orx1-tx-sel0-pin-polarity", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_orx1_tx_sel_0_polarity, "adi,arm-gpio-config-orx1-tx-sel0-pin-enable", NULL, 0);

    AddWidgetToList(ui->arm_gpio_chk_orx1_tx_enable_sel_1, "adi,arm-gpio-config-orx1-tx-sel1-pin-gpio-pin-sel", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx1_tx_sel1_gpio_pin, "adi,arm-gpio-config-orx1-tx-sel1-pin-polarity", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_orx1_tx_sel_1_polarity, "adi,arm-gpio-config-orx1-tx-sel1-pin-enable", NULL, 0);

    AddWidgetToList(ui->arm_gpio_chk_orx2_tx_enable_sel_0, "adi,arm-gpio-config-orx2-tx-sel0-pin-gpio-pin-sel", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx2_tx_sel0_gpio_pin, "adi,arm-gpio-config-orx2-tx-sel0-pin-polarity", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_orx2_tx_sel_0_polarity, "adi,arm-gpio-config-orx2-tx-sel0-pin-enable", NULL, 0);

    AddWidgetToList(ui->arm_gpio_chk_orx2_tx_enable_sel_1, "adi,arm-gpio-config-orx2-tx-sel1-pin-gpio-pin-sel", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx2_tx_sel1_gpio_pin, "adi,arm-gpio-config-orx2-tx-sel1-pin-polarity", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_orx2_tx_sel_1_polarity, "adi,arm-gpio-config-orx2-tx-sel1-pin-enable", NULL, 0);

    AddWidgetToList(ui->arm_gpio_chk_tracking_enable, "adi,arm-gpio-config-en-tx-tracking-cals-gpio-pin-sel", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_tracking_gpio_pin, "adi,arm-gpio-config-en-tx-tracking-cals-polarity", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_tracking_polarity, "adi,arm-gpio-config-en-tx-tracking-cals-enable", NULL, 0);

#pragma endregion }

#pragma region AUX DAC {
    AddWidgetToList(ui->aux_dac_chk_enable0, "adi,aux-dac-enables#0", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable1, "adi,aux-dac-enables#1", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable2, "adi,aux-dac-enables#2", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable3, "adi,aux-dac-enables#3", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable4, "adi,aux-dac-enables#4", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable5, "adi,aux-dac-enables#5", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable6, "adi,aux-dac-enables#6", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable7, "adi,aux-dac-enables#7", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable8, "adi,aux-dac-enables#8", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable9, "adi,aux-dac-enables#9", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable10, "adi,aux-dac-enables#10", NULL, 0);
    AddWidgetToList(ui->aux_dac_chk_enable11, "adi,aux-dac-enables#11", NULL, 0);

    AddWidgetToList(ui->aux_dac_cmb_dac0_vref, "adi,aux-dac-vref0",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac0_res, "adi,aux-dac-resolution0",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac0, "adi,aux-dac-values0", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac1_vref, "adi,aux-dac-vref1",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac1_res, "adi,aux-dac-resolution1",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac1, "adi,aux-dac-values1", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac2_vref, "adi,aux-dac-vref2",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac2_res, "adi,aux-dac-resolution2",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac2, "adi,aux-dac-values2", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac3_vref, "adi,aux-dac-vref3",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac3_res, "adi,aux-dac-resolution3",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac3, "adi,aux-dac-values3", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac4_vref, "adi,aux-dac-vref4",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac4_res, "adi,aux-dac-resolution4",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac4, "adi,aux-dac-values4", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac5_vref, "adi,aux-dac-vref5",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac5_res, "adi,aux-dac-resolution5",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac5, "adi,aux-dac-values5", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac6_vref, "adi,aux-dac-vref6",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac6_res, "adi,aux-dac-resolution6",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac6, "adi,aux-dac-values6", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac7_vref, "adi,aux-dac-vref7",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac7_res, "adi,aux-dac-resolution7",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac7, "adi,aux-dac-values7", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac8_vref, "adi,aux-dac-vref8",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac8_res, "adi,aux-dac-resolution8",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac8, "adi,aux-dac-values8", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac9_vref, "adi,aux-dac-vref9",new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_cmb_dac9_res, "adi,aux-dac-resolution9",new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->aux_dac_txt_dac9, "adi,aux-dac-values9", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac10, "adi,aux-dac-values10", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac11, "adi,aux-dac-values11", NULL, 0);
#pragma endregion }

#pragma region JESD Settings {

    AddWidgetToList(ui->jesd_txt_ser_amplitude, "adi,jesd204-ser-amplitude", NULL, 0);
    AddWidgetToList(ui->jesd_txt_ser_pre_emphasis, "adi,jesd204-ser-pre-emphasis", NULL, 0);
    AddWidgetToList(ui->jesd_chk_serializer_lane0, "adi,jesd204-ser-invert-lane-polarity#0", NULL, 0);
    AddWidgetToList(ui->jesd_chk_serializer_lane1, "adi,jesd204-ser-invert-lane-polarity#1", NULL, 0);
    AddWidgetToList(ui->jesd_chk_serializer_lane2, "adi,jesd204-ser-invert-lane-polarity#2", NULL, 0);
    AddWidgetToList(ui->jesd_chk_serializer_lane3, "adi,jesd204-ser-invert-lane-polarity#3", NULL, 0);

    AddWidgetToList(ui->jesd_chk_deserializer_lane0, "adi,jesd204-des-invert-lane-polarity#0", NULL, 0);
    AddWidgetToList(ui->jesd_chk_deserializer_lane1, "adi,jesd204-des-invert-lane-polarity#1", NULL, 0);
    AddWidgetToList(ui->jesd_chk_deserializer_lane2, "adi,jesd204-des-invert-lane-polarity#2", NULL, 0);
    AddWidgetToList(ui->jesd_chk_deserializer_lane3, "adi,jesd204-des-invert-lane-polarity#3", NULL, 0);

    AddWidgetToList(ui->jesd_txt_des_eq, "adi,jesd204-des-eq-setting", NULL, 0);
    AddWidgetToList(ui->jesd_chk_sys_ref_lvds_mode, "adi,jesd204-sysref-lvds-mode", NULL, 0);
    AddWidgetToList(ui->jesd_chk_sys_ref_ldvs_pn_invert, "adi,jesd204-sysref-lvds-pn-invert", NULL, 0);

#pragma endregion }

#pragma region JESD Framer {

    AddWidgetToList(ui->jesd_framer_txt_bank_id_rx, "adi,jesd204-framer-a-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_device_id_rx, "adi,jesd204-framer-a-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lane0_rx, "adi,jesd204-framer-a-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_cmb_m_rx, "adi,jesd204-framer-a-m", new (unsigned char[3]){0,2,4}, 3);
    AddWidgetToList(ui->jesd_framer_txt_k_rx, "adi,jesd204-framer-a-k", NULL, 0);
    AddWidgetToList(ui->jesd_framer_cmb_f_rx, "adi,jesd204-framer-a-f",new (unsigned char[6]){1, 2, 3, 4, 6, 8}, 6);
    AddWidgetToList(ui->jesd_framer_cmb_np_rx, "adi,jesd204-framer-a-np",new  (unsigned char[3]){12,16,24}, 3);
    AddWidgetToList(ui->jesd_framer_chk_scramble_rx, "adi,jesd204-framer-a-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_external_sysref_rx, "adi,jesd204-framer-a-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane0_rx,"adi,jesd204-framer-a-serializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane1_rx, "adi,jesd204-framer-a-serializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane2_rx, "adi,jesd204-framer-a-serializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane3_rx, "adi,jesd204-framer-a-serializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_serializer_lane_crossbar_rx, "adi,jesd204-framer-a-serializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lmdc_offset_rx, "adi,jesd204-framer-a-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_new_sysref_on_relink_rx, "adi,jesd204-framer-a-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_syncb_in_lvds_pn, "adi,jesd204-framer-a-syncb-in-select", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_over_sample_rx, "adi,jesd204-framer-a-over-sample", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_syncb_in_lvds_pn, "adi,jesd204-framer-a-syncb-in-lvds-mode", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_syncb_in_lvds_pn, "adi,jesd204-framer-a-syncb-in-lvds-pn-invert", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_rx_enable_manual_lane, "adi,jesd204-framer-a-enable-manual-lane-xbar", NULL, 0);

    AddWidgetToList(ui->jesd_framer_txt_bank_id_observation, "adi,jesd204-framer-b-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_device_id_observation, "adi,jesd204-framer-b-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lane0_observation, "adi,jesd204-framer-b-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_cmb_m_observation, "adi,jesd204-framer-b-m",new (unsigned char[3]){0,2,4}, 3);
    AddWidgetToList(ui->jesd_framer_txt_k_observation, "adi,jesd204-framer-b-k", NULL, 0);
    AddWidgetToList(ui->jesd_framer_cmb_f_observation, "adi,jesd204-framer-b-f",new (unsigned char[6]){1, 2, 3, 4, 6, 8}, 6);
    AddWidgetToList(ui->jesd_framer_cmb_np_observation, "adi,jesd204-framer-b-np",new  (unsigned char[3]){12,16,24}, 3);
    AddWidgetToList(ui->jesd_framer_chk_scramble_observation, "adi,jesd204-framer-b-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_external_sysref_observation, "adi,jesd204-framer-b-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane0_observation, "adi,jesd204-framer-b-serializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane1_observation, "adi,jesd204-framer-b-serializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane2_observation, "adi,jesd204-framer-b-serializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane3_observation, "adi,jesd204-framer-b-serializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_serializer_lane_crossbar_observation, "adi,jesd204-framer-b-serializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lmdc_offset_observation, "adi,jesd204-framer-b-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_new_sysref_on_relink_observation, "adi,jesd204-framer-b-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_obs_rx_syncb_select_rx, "adi,jesd204-framer-b-syncb-in-select", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_over_sample_rx_obs, "adi,jesd204-framer-b-over-sample", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_rx_syncb_in_lvds_obs, "adi,jesd204-framer-b-syncb-in-lvds-mode", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_syncb_in_lvds_pn_obs, "adi,jesd204-framer-b-syncb-in-lvds-pn-invert", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_rx_enable_manual_lane_obs, "adi,jesd204-framer-b-enable-manual-lane-xbar", NULL, 0);

#pragma endregion }

#pragma region JESD DeFramer {

    AddWidgetToList(ui->jesd_deframer_txt_bank_id_rx, "adi,jesd204-deframer-a-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_device_id_rx, "adi,jesd204-deframer-a-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_lane0_id_rx, "adi,jesd204-deframer-a-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_m_rx, "adi,jesd204-deframer-a-m",new  (unsigned char[3]){0,2,4}, 3);
    AddWidgetToList(ui->jesd_deframer_txt_k_rx, "adi,jesd204-deframer-a-k", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_scramble, "adi,jesd204-deframer-a-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_external_sysref, "adi,jesd204-deframer-a-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane0, "adi,jesd204-deframer-a-deserializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane1, "adi,jesd204-deframer-a-deserializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane2, "adi,jesd204-deframer-a-deserializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane3, "adi,jesd204-deframer-a-deserializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_deserializer_lane_crossbar, "adi,jesd204-deframer-a-deserializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_lmfc_offset, "adi,jesd204-deframer-a-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_new_sysref_on, "adi,jesd204-deframer-a-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_select, "adi,jesd204-deframer-a-syncb-out-select", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_cmb_np, "adi,jesd204-deframer-a-np",new (unsigned char[2]){12, 16}, 2);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_lvds, "adi,jesd204-deframer-a-syncb-out-lvds-mode", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_lvds_pn, "adi,jesd204-deframer-a-syncb-out-lvds-pn-invert", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_syncb_out_cmos, "adi,jesd204-deframer-a-syncb-out-cmos-slew-rate", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_cmos_drive, "adi,jesd204-deframer-a-syncb-out-cmos-drive-level", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_enable_manual_lane, "adi,jesd204-deframer-a-enable-manual-lane-xbar", NULL, 0);

    AddWidgetToList(ui->jesd_deframer_txt_bank_id_observation, "adi,jesd204-deframer-b-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_device_id_observation, "adi,jesd204-deframer-b-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_lane0_id_observation, "adi,jesd204-deframer-b-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_m_observation, "adi,jesd204-deframer-b-m",new  (unsigned char[3]){0,2,4}, 3);
    AddWidgetToList(ui->jesd_deframer_txt_k_observation, "adi,jesd204-deframer-b-k", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_scramble_2 , "adi,jesd204-deframer-b-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_external_sysref_2, "adi,jesd204-deframer-b-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane0_2, "adi,jesd204-deframer-b-deserializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane1_2, "adi,jesd204-deframer-b-deserializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane2_2, "adi,jesd204-deframer-b-deserializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane3_2, "adi,jesd204-deframer-b-deserializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_deserializer_lane_crossbar_2, "adi,jesd204-deframer-b-deserializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_lmfc_offset_2, "adi,jesd204-deframer-b-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_new_sysref_on_obs, "adi,jesd204-deframer-b-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_select_obs, "adi,jesd204-deframer-b-syncb-out-select", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_cmb_np_obs, "adi,jesd204-deframer-b-np",new (unsigned char[2]){12, 16}, 2);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_lvds_obs, "adi,jesd204-deframer-b-syncb-out-lvds-mode", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_lvds_pn_obs, "adi,jesd204-deframer-b-syncb-out-lvds-pn-invert", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_syncb_out_cmos_obs, "adi,jesd204-deframer-b-syncb-out-cmos-slew-rate", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_syncb_out_cmos_drive_obs, "adi,jesd204-deframer-b-syncb-out-cmos-drive-level", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_enable_manual_lane_obs, "adi,jesd204-deframer-b-enable-manual-lane-xbar", NULL, 0);
#pragma endregion }

#pragma region BIST {

    AddWidgetToList(ui->bist_cmb_prbs_rx, "bist_framer_a_prbs",new (unsigned char[11]){0, 1, 2, 3, 4, 5, 6, 7, 8, 14, 15}, 11);
    AddWidgetToList(ui->bist_cmb_prbs_rx_2, "bist_framer_b_prbs",new (unsigned char[11]){0, 1, 2, 3, 4, 5, 6, 7, 8, 14, 15}, 11);

#pragma endregion }

    AddWidgetToList(ui->btn_save_settings, "initialize", NULL, 0);

}

/**
 * @brief adrv9009_adv::init
 * @return
 */
QWidget * adrv9009_adv::init()
{



    if (!globals::ctx)
        return NULL;

    // Enhance Vertical TabBar
    QTabBar *tabBar=ui->mainTabContainer->tabBar();
    for(int i=0;i<ui->mainTabContainer->count();i++)
    {
        tabBar->setTabButton(i,QTabBar::LeftSide,new QLabel(ui->mainTabContainer->tabText(i)));
        ui->mainTabContainer->setTabText(i,"");
    }

    globals::attrs=new QList<w_info*>();

    InitializeWidgets();

    globals::dev = iio_context_find_device(globals::ctx, PHY_DEVICE);

    globals::connect_widgets(new QWidget());

    QObject::connect(ui->bist_txt_tx1_tone_mhz,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](double value){
        bist_tone_cb();
    });

    QObject::connect(ui->bist_txt_tx2_tone_mhz,
                     static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),[=](double value){
        bist_tone_cb();
    });

    QObject::connect(ui->bist_chk_tx_nco,
                     &QCheckBox::stateChanged,[=](int state){
        bist_tone_cb();
    });

    QObject::connect(tabBar,
                     &QTabBar::currentChanged,[=](int page){

        // Hide save setting button on BIST Tab
        ui->btn_save_settings->setVisible(page!=15);
    });

    // Falling off the end of this QWidget*-returning function is undefined
    // behavior: with optimization GCC emits no ret here and execution runs
    // into the next function's bytes, killing the app with SIGILL.
    return NULL;
}

/**
 * @brief adrv9009_adv::on_btn_save_settings_clicked
 */
void adrv9009_adv::on_btn_save_settings_clicked()
{

    //    QWidget * d = new QWidget();

    //    QVBoxLayout * s = new QVBoxLayout;

    //    QLabel * lbl = new QLabel;
    //    lbl->setText("hji");
    //    s->addWidget(lbl);
    //    d->setLayout(s);
    //    d->show();



    //    newProg->setMaximum(100);
    //  newProg->setMinimum(100);
    //  newProg->setValue(0);

    //  newProg->setVisible(true);
    //  s->addWidget(newProg);
    //  d->setMinimumSize(500,500);
    //  d->setLayout(s);
    //  d->move(300,300);
    ////    d->he
    //  d->show();

    //  QTimer::singleShot(0,[&]{
    //          QMessageBox p;
    //          p.setIcon(QMessageBox::Warning);
    //          p.setText("File is loading, please wait...");
    //          p.exec();
    // });

    //    QMessageBox p;
    //    p.setIcon(QMessageBox::Warning);
    //    p.setText("File is loading, please wait...");
    //    p.exec();

    iio_device_debug_attr_write_longlong(globals::dev, "initialize", 1);
    //saeid raziani
    //when this btn is clicked, some parameters' tick is uncheked, we emit this signal in order to back those states to previos.



    QTimer::singleShot(1000,[&]{
        emit saveSettingSignal();
        QMessageBox p;
        p.setIcon(QMessageBox::Information);
        p.setText("Loading file is finished");
        p.exec();});

    //    newProg->setVisible(false);
    //    d->setVisible(false);
}

/**
 * @brief adrv9009_adv::bist_tone_cb
 */
void adrv9009_adv::bist_tone_cb()
{
    unsigned enable, tx1_freq, tx2_freq;
    char temp[40];

    tx1_freq = ui->bist_txt_tx1_tone_mhz->value() * 1000;
    tx2_freq = ui->bist_txt_tx2_tone_mhz->value() * 1000;

    enable = ui->bist_chk_tx_nco->isChecked();

    sprintf(temp, "%u %u %u", enable, tx1_freq, tx2_freq);

    iio_device_debug_attr_write(globals::dev, "bist_tone", "0 0 0");
    iio_device_debug_attr_write(globals::dev, "bist_tone", temp);
}



void adrv9009_adv::on_fhm_chk_trigger_mode_stateChanged(int arg1)
{

    int f = arg1;

}
