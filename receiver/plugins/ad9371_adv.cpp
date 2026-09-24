#include "ad9371_adv.h"
#include "ui_ad9371_adv.h"


#define PHY_DEVICE "ad9371-phy"
#define DDS_DEVICE "axi-ad9371-tx-hpc"
#define CAP_DEVICE "axi-ad9371-rx-hpc"
#define THIS_DRIVER "AD9371 Advanced"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


    //OscPlot *plot_xcorr_4ch;

    static bool can_update_widgets;

    static gint this_page;
static gboolean plugin_detached;

    enum ad9371adv_wtype {
        CHECKBOX,
        SPINBUTTON,
        COMBOBOX,
        BUTTON,
        CHECKBOX_MASK,
        SPINBUTTON_S8,
        SPINBUTTON_S16,
        };



    static const char *ad9371_adv_sr_attribs[] = {
        "debug.ad9371-phy.adi,jesd204-rx-framer-bank-id",
        "debug.ad9371-phy.adi,jesd204-rx-framer-device-id",
        "debug.ad9371-phy.adi,jesd204-rx-framer-lane0-id",
        "debug.ad9371-phy.adi,jesd204-rx-framer-m",
        "debug.ad9371-phy.adi,jesd204-rx-framer-k",
        "debug.ad9371-phy.adi,jesd204-rx-framer-scramble",
        "debug.ad9371-phy.adi,jesd204-rx-framer-external-sysref",
        "debug.ad9371-phy.adi,jesd204-rx-framer-serializer-lanes-enabled",
        "debug.ad9371-phy.adi,jesd204-rx-framer-serializer-lane-crossbar",
        "debug.ad9371-phy.adi,jesd204-rx-framer-serializer-amplitude",
        "debug.ad9371-phy.adi,jesd204-rx-framer-pre-emphasis",
        "debug.ad9371-phy.adi,jesd204-rx-framer-invert-lane-polarity",
        "debug.ad9371-phy.adi,jesd204-rx-framer-lmfc-offset",
        "debug.ad9371-phy.adi,jesd204-rx-framer-new-sysref-on-relink",
        "debug.ad9371-phy.adi,jesd204-rx-framer-enable-auto-chan-xbar",
        "debug.ad9371-phy.adi,jesd204-rx-framer-obs-rx-syncb-select",
        "debug.ad9371-phy.adi,jesd204-rx-framer-rx-syncb-mode",
        "debug.ad9371-phy.adi,jesd204-rx-framer-over-sample",

        "debug.ad9371-phy.adi,jesd204-obs-framer-bank-id",
        "debug.ad9371-phy.adi,jesd204-obs-framer-device-id",
        "debug.ad9371-phy.adi,jesd204-obs-framer-lane0-id",
        "debug.ad9371-phy.adi,jesd204-obs-framer-m",
        "debug.ad9371-phy.adi,jesd204-obs-framer-k",
        "debug.ad9371-phy.adi,jesd204-obs-framer-scramble",
        "debug.ad9371-phy.adi,jesd204-obs-framer-external-sysref",
        "debug.ad9371-phy.adi,jesd204-obs-framer-serializer-lanes-enabled",
        "debug.ad9371-phy.adi,jesd204-obs-framer-serializer-lane-crossbar",
        "debug.ad9371-phy.adi,jesd204-obs-framer-serializer-amplitude",
        "debug.ad9371-phy.adi,jesd204-obs-framer-pre-emphasis",
        "debug.ad9371-phy.adi,jesd204-obs-framer-invert-lane-polarity",
        "debug.ad9371-phy.adi,jesd204-obs-framer-lmfc-offset",
        "debug.ad9371-phy.adi,jesd204-obs-framer-new-sysref-on-relink",
        "debug.ad9371-phy.adi,jesd204-obs-framer-enable-auto-chan-xbar",
        "debug.ad9371-phy.adi,jesd204-obs-framer-obs-rx-syncb-select",
        "debug.ad9371-phy.adi,jesd204-obs-framer-rx-syncb-mode",
        "debug.ad9371-phy.adi,jesd204-obs-framer-over-sample",


        "debug.ad9371-phy.adi,jesd204-deframer-bank-id",
        "debug.ad9371-phy.adi,jesd204-deframer-device-id",
        "debug.ad9371-phy.adi,jesd204-deframer-lane0-id",
        "debug.ad9371-phy.adi,jesd204-deframer-m",
        "debug.ad9371-phy.adi,jesd204-deframer-k",
        "debug.ad9371-phy.adi,jesd204-deframer-scramble",
        "debug.ad9371-phy.adi,jesd204-deframer-external-sysref",
        "debug.ad9371-phy.adi,jesd204-deframer-deserializer-lanes-enabled",
        "debug.ad9371-phy.adi,jesd204-deframer-deserializer-lane-crossbar",
        "debug.ad9371-phy.adi,jesd204-deframer-eq-setting",
        "debug.ad9371-phy.adi,jesd204-deframer-invert-lane-polarity",
        "debug.ad9371-phy.adi,jesd204-deframer-lmfc-offset",
        "debug.ad9371-phy.adi,jesd204-deframer-new-sysref-on-relink",
        "debug.ad9371-phy.adi,jesd204-deframer-enable-auto-chan-xbar",
        "debug.ad9371-phy.adi,jesd204-deframer-tx-syncb-mode",

        "debug.ad9371-phy.adi,rx-gain-mode",
        "debug.ad9371-phy.adi,rx1-gain-index",
        "debug.ad9371-phy.adi,rx2-gain-index",
        "debug.ad9371-phy.adi,rx1-max-gain-index",
        "debug.ad9371-phy.adi,rx1-min-gain-index",
        "debug.ad9371-phy.adi,rx2-max-gain-index",
        "debug.ad9371-phy.adi,rx2-min-gain-index",

        "debug.ad9371-phy.adi,orx-gain-mode",
        "debug.ad9371-phy.adi,orx1-gain-index",
        "debug.ad9371-phy.adi,orx2-gain-index",
        "debug.ad9371-phy.adi,orx-max-gain-index",
        "debug.ad9371-phy.adi,orx-min-gain-index",

        "debug.ad9371-phy.adi,sniffer-gain-mode",
        "debug.ad9371-phy.adi,sniffer-gain-index",
        "debug.ad9371-phy.adi,sniffer-max-gain-index",
        "debug.ad9371-phy.adi,sniffer-min-gain-index",

        "debug.ad9371-phy.adi,rx-peak-agc-apd-high-thresh",
        "debug.ad9371-phy.adi,rx-peak-agc-apd-low-thresh",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-high-thresh",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-low-thresh",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-very-low-thresh",
        "debug.ad9371-phy.adi,rx-peak-agc-apd-high-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,rx-peak-agc-apd-low-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-high-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-low-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-very-low-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,rx-peak-agc-apd-high-gain-step-attack",
        "debug.ad9371-phy.adi,rx-peak-agc-apd-low-gain-step-recovery",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-high-gain-step-attack",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-low-gain-step-recovery",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-very-low-gain-step-recovery",
        "debug.ad9371-phy.adi,rx-peak-agc-apd-fast-attack",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-fast-attack",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-overload-detect-enable",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-overload-duration-cnt",
        "debug.ad9371-phy.adi,rx-peak-agc-hb2-overload-thresh-cnt",

        "debug.ad9371-phy.adi,obs-peak-agc-apd-high-thresh",
        "debug.ad9371-phy.adi,obs-peak-agc-apd-low-thresh",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-high-thresh",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-low-thresh",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-very-low-thresh",
        "debug.ad9371-phy.adi,obs-peak-agc-apd-high-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,obs-peak-agc-apd-low-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-high-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-low-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-very-low-thresh-exceeded-cnt",
        "debug.ad9371-phy.adi,obs-peak-agc-apd-high-gain-step-attack",
        "debug.ad9371-phy.adi,obs-peak-agc-apd-low-gain-step-recovery",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-high-gain-step-attack",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-low-gain-step-recovery",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-very-low-gain-step-recovery",
        "debug.ad9371-phy.adi,obs-peak-agc-apd-fast-attack",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-fast-attack",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-overload-detect-enable",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-overload-duration-cnt",
        "debug.ad9371-phy.adi,obs-peak-agc-hb2-overload-thresh-cnt",

        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-upper-high-thresh",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-upper-low-thresh",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-lower-high-thresh",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-lower-low-thresh",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-upper-high-gain-step-attack",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-upper-low-gain-step-attack",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-lower-high-gain-step-recovery",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-lower-low-gain-step-recovery",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-meas-duration",
        "debug.ad9371-phy.adi,rx-pwr-agc-pmd-meas-config",

        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-upper-high-thresh",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-upper-low-thresh",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-lower-high-thresh",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-lower-low-thresh",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-upper-high-gain-step-attack",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-upper-low-gain-step-attack",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-lower-high-gain-step-recovery",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-lower-low-gain-step-recovery",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-meas-duration",
        "debug.ad9371-phy.adi,obs-pwr-agc-pmd-meas-config",

        "debug.ad9371-phy.adi,rx-agc-conf-agc-rx1-max-gain-index",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-rx1-min-gain-index",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-rx2-max-gain-index",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-rx2-min-gain-index",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-peak-threshold-mode",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-low-ths-prevent-gain-increase",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-gain-update-counter",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-slow-loop-settling-delay",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-peak-wait-time",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-reset-on-rx-enable",
        "debug.ad9371-phy.adi,rx-agc-conf-agc-enable-sync-pulse-for-gain-counter",

        "debug.ad9371-phy.adi,obs-agc-conf-agc-obs-rx-max-gain-index",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-obs-rx-min-gain-index",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-obs-rx-select",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-peak-threshold-mode",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-low-ths-prevent-gain-increase",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-gain-update-counter",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-slow-loop-settling-delay",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-peak-wait-time",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-reset-on-rx-enable",
        "debug.ad9371-phy.adi,obs-agc-conf-agc-enable-sync-pulse-for-gain-counter",

        "debug.ad9371-phy.adi,rx-profile-adc-div",
        "debug.ad9371-phy.adi,rx-profile-rx-fir-decimation",
        "debug.ad9371-phy.adi,rx-profile-rx-dec5-decimation",
        "debug.ad9371-phy.adi,rx-profile-en-high-rej-dec5",
        "debug.ad9371-phy.adi,rx-profile-rhb1-decimation",
        "debug.ad9371-phy.adi,rx-profile-iq-rate_khz",
        "debug.ad9371-phy.adi,rx-profile-rf-bandwidth_hz",
        "debug.ad9371-phy.adi,rx-profile-rx-bbf-3db-corner_khz",

        "debug.ad9371-phy.adi,obs-profile-adc-div",
        "debug.ad9371-phy.adi,obs-profile-rx-fir-decimation",
        "debug.ad9371-phy.adi,obs-profile-rx-dec5-decimation",
        "debug.ad9371-phy.adi,obs-profile-en-high-rej-dec5",
        "debug.ad9371-phy.adi,obs-profile-rhb1-decimation",
        "debug.ad9371-phy.adi,obs-profile-iq-rate_khz",
        "debug.ad9371-phy.adi,obs-profile-rf-bandwidth_hz",
        "debug.ad9371-phy.adi,obs-profile-rx-bbf-3db-corner_khz",

        "debug.ad9371-phy.adi,sniffer-profile-adc-div",
        "debug.ad9371-phy.adi,sniffer-profile-rx-fir-decimation",
        "debug.ad9371-phy.adi,sniffer-profile-rx-dec5-decimation",
        "debug.ad9371-phy.adi,sniffer-profile-en-high-rej-dec5",
        "debug.ad9371-phy.adi,sniffer-profile-rhb1-decimation",
        "debug.ad9371-phy.adi,sniffer-profile-iq-rate_khz",
        "debug.ad9371-phy.adi,sniffer-profile-rf-bandwidth_hz",
        "debug.ad9371-phy.adi,sniffer-profile-rx-bbf-3db-corner_khz",

        "debug.ad9371-phy.adi,tx-profile-dac-div",
        "debug.ad9371-phy.adi,tx-profile-tx-fir-interpolation",
        "debug.ad9371-phy.adi,tx-profile-thb1-interpolation",
        "debug.ad9371-phy.adi,tx-profile-thb2-interpolation",
        "debug.ad9371-phy.adi,tx-profile-tx-input-hb-interpolation",
        "debug.ad9371-phy.adi,tx-profile-iq-rate_khz",
        "debug.ad9371-phy.adi,tx-profile-primary-sig-bandwidth_hz",
        "debug.ad9371-phy.adi,tx-profile-rf-bandwidth_hz",
        "debug.ad9371-phy.adi,tx-profile-tx-dac-3db-corner_khz",
        "debug.ad9371-phy.adi,tx-profile-tx-bbf-3db-corner_khz",

        "debug.ad9371-phy.adi,clocks-device-clock_khz",
        "debug.ad9371-phy.adi,clocks-clk-pll-vco-freq_khz",
        "debug.ad9371-phy.adi,clocks-clk-pll-vco-div",
        "debug.ad9371-phy.adi,clocks-clk-pll-hs-div",

        "debug.ad9371-phy.adi,tx-settings-tx-channels-enable",
        "debug.ad9371-phy.adi,tx-settings-tx-pll-use-external-lo",
        "debug.ad9371-phy.adi,tx-settings-tx-pll-lo-frequency_hz",
        "debug.ad9371-phy.adi,tx-settings-tx-atten-step-size",
        "debug.ad9371-phy.adi,tx-settings-tx1-atten_mdb",
        "debug.ad9371-phy.adi,tx-settings-tx2-atten_mdb",

        "debug.ad9371-phy.adi,rx-settings-rx-channels-enable",
        "debug.ad9371-phy.adi,rx-settings-rx-pll-use-external-lo",
        "debug.ad9371-phy.adi,rx-settings-rx-pll-lo-frequency_hz",
        "debug.ad9371-phy.adi,rx-settings-real-if-data",

        "debug.ad9371-phy.adi,obs-settings-obs-rx-channels-enable",
        "debug.ad9371-phy.adi,obs-settings-obs-rx-lo-source",
        "debug.ad9371-phy.adi,obs-settings-sniffer-pll-lo-frequency_hz",
        "debug.ad9371-phy.adi,obs-settings-real-if-data",
        "debug.ad9371-phy.adi,obs-settings-default-obs-rx-channel",

        "debug.ad9371-phy.adi,arm-gpio-use-rx2-enable-pin",
        "debug.ad9371-phy.adi,arm-gpio-use-tx2-enable-pin",
        "debug.ad9371-phy.adi,arm-gpio-tx-rx-pin-mode",
        "debug.ad9371-phy.adi,arm-gpio-orx-pin-mode",
        "debug.ad9371-phy.adi,arm-gpio-orx-trigger-pin",
        "debug.ad9371-phy.adi,arm-gpio-orx-mode2-pin",
        "debug.ad9371-phy.adi,arm-gpio-orx-mode1-pin",
        "debug.ad9371-phy.adi,arm-gpio-orx-mode0-pin",
        "debug.ad9371-phy.adi,arm-gpio-rx1-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-rx2-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-tx1-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-tx2-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-orx1-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-orx2-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-srx-enable-ack",
        "debug.ad9371-phy.adi,arm-gpio-tx-obs-select",
        "debug.ad9371-phy.adi,arm-gpio-enable-mask",

        "debug.ad9371-phy.adi,gpio-3v3-oe-mask",
        "debug.ad9371-phy.adi,gpio-3v3-src-ctrl3_0",
        "debug.ad9371-phy.adi,gpio-3v3-src-ctrl7_4",
        "debug.ad9371-phy.adi,gpio-3v3-src-ctrl11_8",

        "debug.ad9371-phy.adi,gpio-oe-mask",
        "debug.ad9371-phy.adi,gpio-src-ctrl3_0",
        "debug.ad9371-phy.adi,gpio-src-ctrl7_4",
        "debug.ad9371-phy.adi,gpio-src-ctrl11_8",
        "debug.ad9371-phy.adi,gpio-src-ctrl15_12",
        "debug.ad9371-phy.adi,gpio-src-ctrl18_16",

        "debug.ad9371-phy.adi,aux-dac-enable-mask",
        "debug.ad9371-phy.adi,aux-dac-value0",
        "debug.ad9371-phy.adi,aux-dac-slope0",
        "debug.ad9371-phy.adi,aux-dac-vref0",
        "debug.ad9371-phy.adi,aux-dac-value1",
        "debug.ad9371-phy.adi,aux-dac-slope1",
        "debug.ad9371-phy.adi,aux-dac-vref1",
        "debug.ad9371-phy.adi,aux-dac-value2",
        "debug.ad9371-phy.adi,aux-dac-slope2",
        "debug.ad9371-phy.adi,aux-dac-vref2",
        "debug.ad9371-phy.adi,aux-dac-value3",
        "debug.ad9371-phy.adi,aux-dac-slope3",
        "debug.ad9371-phy.adi,aux-dac-vref3",
        "debug.ad9371-phy.adi,aux-dac-value4",
        "debug.ad9371-phy.adi,aux-dac-slope4",
        "debug.ad9371-phy.adi,aux-dac-vref4",
        "debug.ad9371-phy.adi,aux-dac-value5",
        "debug.ad9371-phy.adi,aux-dac-slope5",
        "debug.ad9371-phy.adi,aux-dac-vref5",
        "debug.ad9371-phy.adi,aux-dac-value6",
        "debug.ad9371-phy.adi,aux-dac-slope6",
        "debug.ad9371-phy.adi,aux-dac-vref6",
        "debug.ad9371-phy.adi,aux-dac-value7",
        "debug.ad9371-phy.adi,aux-dac-slope7",
        "debug.ad9371-phy.adi,aux-dac-vref7",
        "debug.ad9371-phy.adi,aux-dac-value8",
        "debug.ad9371-phy.adi,aux-dac-slope8",
        "debug.ad9371-phy.adi,aux-dac-vref8",
        "debug.ad9371-phy.adi,aux-dac-value9",
        "debug.ad9371-phy.adi,aux-dac-slope9",
        "debug.ad9371-phy.adi,aux-dac-vref9",

        "debug.adi,default-initial-calibrations-mask",

        "debug.ad9371-phy.adi,dpd-damping",
        "debug.ad9371-phy.adi,dpd-num-weights",
        "debug.ad9371-phy.adi,dpd-model-version",
        "debug.ad9371-phy.adi,dpd-high-power-model-update",
        "debug.ad9371-phy.adi,dpd-model-prior-weight",
        "debug.ad9371-phy.adi,dpd-robust-modeling",
        "debug.ad9371-phy.adi,dpd-samples",
        "debug.ad9371-phy.adi,dpd-outlier-threshold",
        "debug.ad9371-phy.adi,dpd-additional-delay-offset",
        "debug.ad9371-phy.adi,dpd-path-delay-pn-seq-level",
        "debug.ad9371-phy.adi,dpd-weights0-real",
        "debug.ad9371-phy.adi,dpd-weights0-imag",
        "debug.ad9371-phy.adi,dpd-weights1-real",
        "debug.ad9371-phy.adi,dpd-weights1-imag",
        "debug.ad9371-phy.adi,dpd-weights2-real",
        "debug.ad9371-phy.adi,dpd-weights2-imag",

        "debug.ad9371-phy.adi,clgc-tx1-desired-gain",
        "debug.ad9371-phy.adi,clgc-tx2-desired-gain",
        "debug.ad9371-phy.adi,clgc-tx1-atten-limit",
        "debug.ad9371-phy.adi,clgc-tx2-atten-limit",
        "debug.ad9371-phy.adi,clgc-tx1-control-ratio",
        "debug.ad9371-phy.adi,clgc-tx2-control-ratio",
        "debug.ad9371-phy.adi,clgc-allow-tx1-atten-updates",
        "debug.ad9371-phy.adi,clgc-allow-tx2-atten-updates",
        "debug.ad9371-phy.adi,clgc-additional-delay-offset",
        "debug.ad9371-phy.adi,clgc-path-delay-pn-seq-level",
        "debug.ad9371-phy.adi,clgc-tx1-rel-threshold",
        "debug.ad9371-phy.adi,clgc-tx2-rel-threshold",
        "debug.ad9371-phy.adi,clgc-tx1-rel-threshold-en",
        "debug.ad9371-phy.adi,clgc-tx2-rel-threshold-en",

        "debug.ad9371-phy.adi,vswr-additional-delay-offset",
        "debug.ad9371-phy.adi,vswr-path-delay-pn-seq-level",
        "debug.ad9371-phy.adi,vswr-tx1-vswr-switch-gpio3p3-pin",
        "debug.ad9371-phy.adi,vswr-tx2-vswr-switch-gpio3p3-pin",
        "debug.ad9371-phy.adi,vswr-tx1-vswr-switch-polarity",
        "debug.ad9371-phy.adi,vswr-tx2-vswr-switch-polarity",
        "debug.ad9371-phy.adi,vswr-tx1-vswr-switch-delay_us",
        "debug.ad9371-phy.adi,vswr-tx2-vswr-switch-delay_us",
        };

ad9371_adv::ad9371_adv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ad9371_adv)
{
    ui->setupUi(this);

    ui->mainTabContainer->setTabPosition(QTabWidget::West);
}

ad9371_adv::~ad9371_adv()
{
    delete ui;
}

void ad9371_adv::AddWidgetToList(QWidget *widget, char *name, unsigned char * const lut,  unsigned char lut_len)
{

    w_info *temp=new w_info();

    temp->widget=widget;
    temp->name=name;
    temp->lut=lut;
    temp->lut_len=lut_len;

    globals::attrs->append(temp);
}


void ad9371_adv::InitializeWidgets(){

    int i=199;


    AddWidgetToList(ui->clk_settings_txt_device_clock, "adi,clocks-device-clock_khz", NULL, 0);
    AddWidgetToList(ui->clk_settings_txt_clk_pll_vco_freq, "adi,clocks-clk-pll-vco-freq_khz", NULL, 0);
    AddWidgetToList(ui->clk_settings_cmb_pll_vco_div, "adi,clocks-clk-pll-vco-div", NULL, 0);
    AddWidgetToList(ui->clk_settings_txt_clk_pll_hs_div, "adi,clocks-clk-pll-hs-div", NULL, 0);

    AddWidgetToList(ui->jesd_framer_txt_bank_id_rx, "adi,jesd204-rx-framer-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_device_id_rx, "adi,jesd204-rx-framer-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lane0_rx, "adi,jesd204-rx-framer-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_m_rx, "adi,jesd204-rx-framer-m", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_k_rx, "adi,jesd204-rx-framer-k", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_scramble_rx, "adi,jesd204-rx-framer-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_external_sysref_rx, "adi,jesd204-rx-framer-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane0_rx, "adi,jesd204-rx-framer-serializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane1_rx, "adi,jesd204-rx-framer-serializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane2_rx, "adi,jesd204-rx-framer-serializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane3_rx, "adi,jesd204-rx-framer-serializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_serializer_lane_crossbar_rx, "adi,jesd204-rx-framer-serializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_serializer_amplitude_rx, "adi,jesd204-rx-framer-serializer-amplitude", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_pre_emphasis_rx, "adi,jesd204-rx-framer-pre-emphasis", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane0_polarity_rx, "adi,jesd204-rx-framer-invert-lane-polarity#0", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane1_polarity_rx, "adi,jesd204-rx-framer-invert-lane-polarity#1", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane2_polarity_rx, "adi,jesd204-rx-framer-invert-lane-polarity#2", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane3_polarity_rx, "adi,jesd204-rx-framer-invert-lane-polarity#3", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lmfc_offset_rx, "adi,jesd204-rx-framer-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_new_sysref_on_relink_rx, "adi,jesd204-rx-framer-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_enable_auto_chan_xbar_rx, "adi,jesd204-rx-framer-enable-auto-chan-xbar", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_obs_rx_syncb_select_rx, "adi,jesd204-rx-framer-obs-rx-syncb-select", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_rx_syncb_mode_rx, "adi,jesd204-rx-framer-rx-syncb-mode", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_over_sample_rx, "adi,jesd204-rx-framer-over-sample", NULL, 0);

    AddWidgetToList(ui->jesd_framer_txt_bank_id_observation, "adi,jesd204-obs-framer-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_device_id_observation, "adi,jesd204-obs-framer-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lane0_observation, "adi,jesd204-obs-framer-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_m_observation, "adi,jesd204-obs-framer-m", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_k_observation, "adi,jesd204-obs-framer-k", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_scramble_observation , "adi,jesd204-obs-framer-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_external_sysref_observation, "adi,jesd204-obs-framer-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane0_observation, "adi,jesd204-obs-framer-serializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane1_observation, "adi,jesd204-obs-framer-serializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane2_observation, "adi,jesd204-obs-framer-serializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_serializer_lane3_observation, "adi,jesd204-obs-framer-serializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_serializer_lane_crossbar_observation, "adi,jesd204-obs-framer-serializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_serializer_amplitude_observation, "adi,jesd204-obs-framer-serializer-amplitude", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_pre_emphasis_observation, "adi,jesd204-obs-framer-pre-emphasis", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane0_polarity_observation, "adi,jesd204-obs-framer-invert-lane-polarity#0", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane1_polarity_observation, "adi,jesd204-obs-framer-invert-lane-polarity#1", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane2_polarity_observation, "adi,jesd204-obs-framer-invert-lane-polarity#2", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_invert_lane3_polarity_observation, "adi,jesd204-obs-framer-invert-lane-polarity#3", NULL, 0);
    AddWidgetToList(ui->jesd_framer_txt_lmfc_offset_observation, "adi,jesd204-obs-framer-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_new_sysref_on_relink_observation, "adi,jesd204-obs-framer-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_enable_auto_chan_xbar_observation, "adi,jesd204-obs-framer-enable-auto-chan-xbar", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_obs_rx_syncb_select_observation, "adi,jesd204-obs-framer-obs-rx-syncb-select", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_rx_syncb_mode_observation, "adi,jesd204-obs-framer-rx-syncb-mode", NULL, 0);
    AddWidgetToList(ui->jesd_framer_chk_over_sample_observation, "adi,jesd204-obs-framer-over-sample", NULL, 0);

    AddWidgetToList(ui->jesd_deframer_txt_bank_id_rx, "adi,jesd204-deframer-bank-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_device_id_rx, "adi,jesd204-deframer-device-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_lane0_id_rx, "adi,jesd204-deframer-lane0-id", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_m_rx, "adi,jesd204-deframer-m", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_k_rx, "adi,jesd204-deframer-k", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_scramble, "adi,jesd204-deframer-scramble", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_external_sysref, "adi,jesd204-deframer-external-sysref", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane0, "adi,jesd204-deframer-deserializer-lanes-enabled#0", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane1, "adi,jesd204-deframer-deserializer-lanes-enabled#1", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane2, "adi,jesd204-deframer-deserializer-lanes-enabled#2", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_deserializer_lane3, "adi,jesd204-deframer-deserializer-lanes-enabled#3", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_deserializer_lane_crossbar, "adi,jesd204-deframer-deserializer-lane-crossbar", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_eq_setting, "adi,jesd204-deframer-eq-setting", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_invert_lane0_polarity, "adi,jesd204-deframer-invert-lane-polarity#0", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_invert_lane1_polarity, "adi,jesd204-deframer-invert-lane-polarity#1", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_invert_lane2_polarity, "adi,jesd204-deframer-invert-lane-polarity#2", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_invert_lane3_polarity, "adi,jesd204-deframer-invert-lane-polarity#3", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_txt_lmfc_offset, "adi,jesd204-deframer-lmfc-offset", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_new_sysref_on_relink, "adi,jesd204-deframer-new-sysref-on-relink", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_enable_auto_chan_xbar, "adi,jesd204-deframer-enable-auto-chan-xbar", NULL, 0);
    AddWidgetToList(ui->jesd_deframer_chk_tx_syncb_mode, "adi,jesd204-deframer-tx-syncb-mode", NULL, 0);

    AddWidgetToList(ui->gain_setup_cmb_rx_gain_control_mode, "adi,rx-gain-mode", new (unsigned char[3]){0, 2,3}, 3);
    AddWidgetToList(ui->gain_setup_txt_rx1_gain_index, "adi,rx1-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx2_gain_index, "adi,rx2-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx1_max_gain_index, "adi,rx1-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx1_min_gain_index, "adi,rx1-min-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx2_max_gain_index, "adi,rx2-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_rx2_min_gain_index, "adi,rx2-min-gain-index", NULL, 0);

    AddWidgetToList(ui->gain_setup_cmb_observation_gain_control_mode, "adi,orx-gain-mode",  new (unsigned char[3]){0, 2,3}, 3);
    AddWidgetToList(ui->gain_setup_txt_observation_orx1_gain_index, "adi,orx1-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_orx2_gain_indx, "adi,orx2-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_max_gain_index, "adi,orx-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_observation_min_gain_index, "adi,orx-min-gain-index", NULL, 0);

    AddWidgetToList(ui->gain_setup_cmb_sniffer_gain_control_mode, "adi,sniffer-gain-mode",  new (unsigned char[3]){0, 2, 3}, 3);
    AddWidgetToList(ui->gain_setup_txt_sniffer_gain_index, "adi,sniffer-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_sniffer_max_gain_index, "adi,sniffer-max-gain-index", NULL, 0);
    AddWidgetToList(ui->gain_setup_txt_sniffer_min_gain_index, "adi,sniffer-min-gain-index", NULL, 0);

    AddWidgetToList(ui->agc_setup_txt_apd_high_thresh_rx, "adi,rx-peak-agc-apd-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_thresh_rx, "adi,rx-peak-agc-apd-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_high_thresh_rx, "adi,rx-peak-agc-hb2-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_low_thresh_rx, "adi,rx-peak-agc-hb2-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_thresh_rx, "adi,rx-peak-agc-hb2-very-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_high_thresh_exceeded_cnt_rx_2, "adi,rx-peak-agc-apd-high-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_thresh_exceeded_cnt_rx, "adi,rx-peak-agc-apd-low-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_high_thresh_exceeded_cnt_rx, "adi,rx-peak-agc-hb2-high-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_low_thresh_exceeded_cnt_rx, "adi,rx-peak-agc-hb2-low-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_thresh_exceeded_cnt_rx, "adi,rx-peak-agc-hb2-very-low-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_high_gain_step_attack_rx, "adi,rx-peak-agc-apd-high-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_gain_step_recovery_rx, "adi,rx-peak-agc-apd-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_high_gain_step_attack_rx, "adi,rx-peak-agc-hb2-high-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_gain_step_recovery_rx, "adi,rx-peak-agc-hb2-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_gain_step_recovery_rx, "adi,rx-peak-agc-hb2-very-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_apd_fast_attack_rx, "adi,rx-peak-agc-apd-fast-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_hb2_fast_attack_rx, "adi,rx-peak-agc-hb2-fast-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_hb2_overload_detect_enable_rx, "adi,rx-peak-agc-hb2-overload-detect-enable", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_overload_duration_cnt_rx, "adi,rx-peak-agc-hb2-overload-duration-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_overload_thresh_cnt_rx, "adi,rx-peak-agc-hb2-overload-thresh-cnt", NULL, 0);

    AddWidgetToList(ui->agc_setup_txt_apd_high_thresh_observation, "adi,obs-peak-agc-apd-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_thresh_observation, "adi,obs-peak-agc-apd-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_high_thresh_observation, "adi,obs-peak-agc-hb2-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_low_thresh_observation, "adi,obs-peak-agc-hb2-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_thresh_observation, "adi,obs-peak-agc-hb2-very-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_high_thresh_exceeded_cnt_observation, "adi,obs-peak-agc-apd-high-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_thresh_exceeded_cnt_observation, "adi,obs-peak-agc-apd-low-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_high_thresh_exceeded_cnt_observation, "adi,obs-peak-agc-hb2-high-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_low_thresh_exceeded_cnt_observation, "adi,obs-peak-agc-hb2-low-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_thresh_exceeded_cnt_observation, "adi,obs-peak-agc-hb2-very-low-thresh-exceeded-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_high_gain_step_attack_observation, "adi,obs-peak-agc-apd-high-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_apd_low_gain_step_attack_observation, "adi,obs-peak-agc-apd-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_high_gain_step_attack_observation, "adi,obs-peak-agc-hb2-high-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_gain_step_recovery_observation, "adi,obs-peak-agc-hb2-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_very_low_gain_step_recovery_observation, "adi,obs-peak-agc-hb2-very-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_apd_fast_attack_observation, "adi,obs-peak-agc-apd-fast-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_hb2_fast_attack_observation, "adi,obs-peak-agc-hb2-fast-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_hb2_overload_detect_enable_observation, "adi,obs-peak-agc-hb2-overload-detect-enable", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_overload_duration_cnt_observation, "adi,obs-peak-agc-hb2-overload-duration-cnt", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_hb2_overload_thresh_cnt_observation, "adi,obs-peak-agc-hb2-overload-thresh-cnt", NULL, 0);

    AddWidgetToList(ui->agc_setup_txt_pmd_upper_high_thresh_rx, "adi,rx-pwr-agc-pmd-upper-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_upper_low_thresh_rx, "adi,rx-pwr-agc-pmd-upper-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_high_thresh_rx, "adi,rx-pwr-agc-pmd-lower-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_low_thresh_rx, "adi,rx-pwr-agc-pmd-lower-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_upper_high_gain_step_attack_rx, "adi,rx-pwr-agc-pmd-upper-high-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_upper_low_gain_step_attack_rx, "adi,rx-pwr-agc-pmd-upper-low-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_high_gain_step_recovery_rx, "adi,rx-pwr-agc-pmd-lower-high-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_low_gain_step_recovery_rx, "adi,rx-pwr-agc-pmd-lower-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_meas_duration_rx, "adi,rx-pwr-agc-pmd-meas-duration", NULL, 0);
    AddWidgetToList(ui->agc_setup_cmb_pmd_configuration_rx, "adi,rx-pwr-agc-pmd-meas-config",  new (unsigned char[4]){0, 1, 2, 3}, 4);

    AddWidgetToList(ui->agc_setup_txt_pmd_upper_high_thresh_observation,  "adi,obs-pwr-agc-pmd-upper-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_upper_low_thresh_observation,  "adi,obs-pwr-agc-pmd-upper-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_high_thresh_observation, "adi,obs-pwr-agc-pmd-lower-high-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_low_thresh_observation, "adi,obs-pwr-agc-pmd-lower-low-thresh", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_upper_high_gain_step_attack_observation, "adi,obs-pwr-agc-pmd-upper-high-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_upper_low_gain_step_attack_observation, "adi,obs-pwr-agc-pmd-upper-low-gain-step-attack", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_high_gain_step_recovery_observation, "adi,obs-pwr-agc-pmd-lower-high-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_lower_low_gain_step_recovery_observation, "adi,obs-pwr-agc-pmd-lower-low-gain-step-recovery", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_pmd_meas_duration_observation, "adi,obs-pwr-agc-pmd-meas-duration", NULL, 0);
    AddWidgetToList(ui->agc_setup_cmb_pmd_configuration_observation, "adi,obs-pwr-agc-pmd-meas-config",  new (unsigned char[4]){0, 1, 2, 3}, 4);

    AddWidgetToList(ui->agc_setup_txt_rx_max_gain_index_rx, "adi,rx-agc-conf-agc-rx1-max-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx_min_gain_index_rx, "adi,rx-agc-conf-agc-rx1-min-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx2_max_gain_index, "adi,rx-agc-conf-agc-rx2-max-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx2_min_gain_index, "adi,rx-agc-conf-agc-rx2-min-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_peak_threshold_mode, "adi,rx-agc-conf-agc-peak-threshold-mode", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_low_ths_prevent_gain_increase, "adi,rx-agc-conf-agc-low-ths-prevent-gain-increase", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_gain_update_counter_rx, "adi,rx-agc-conf-agc-gain-update-counter", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_slow_loop_settling_delay_rx, "adi,rx-agc-conf-agc-slow-loop-settling-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_peak_wait_time_rx, "adi,rx-agc-conf-agc-peak-wait-time", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_reset_on_rx_enable, "adi,rx-agc-conf-agc-reset-on-rx-enable", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_enable_sync_pulse_for_gain_counter_rx, "adi,rx-agc-conf-agc-enable-sync-pulse-for-gain-counter", NULL, 0);

    AddWidgetToList(ui->agc_setup_txt_rx_max_gain_index_observation, "adi,obs-agc-conf-agc-obs-rx-max-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_rx_min_gain_index_observation, "adi,obs-agc-conf-agc-obs-rx-min-gain-index", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_obs_rx_select, "adi,obs-agc-conf-agc-obs-rx-select", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_peak_threshold_mode_observation, "adi,obs-agc-conf-agc-peak-threshold-mode", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_low_ths_prevent_gain_increase_observation, "adi,obs-agc-conf-agc-low-ths-prevent-gain-increase", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_gain_update_counter_rx, "adi,obs-agc-conf-agc-gain-update-counter", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_slow_loop_settling_delay_observation, "adi,obs-agc-conf-agc-slow-loop-settling-delay", NULL, 0);
    AddWidgetToList(ui->agc_setup_txt_peak_wait_time_observation, "adi,obs-agc-conf-agc-peak-wait-time", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_reset_on_rx_enable_observation, "adi,obs-agc-conf-agc-reset-on-rx-enable", NULL, 0);
    AddWidgetToList(ui->agc_setup_chk_enable_sync_pulse_for_gain_counter_observation, "adi,obs-agc-conf-agc-enable-sync-pulse-for-gain-counter", NULL, 0);

    AddWidgetToList(ui->rx_settings_txt_adc_div, "adi,rx-profile-adc-div", NULL, 0);
    AddWidgetToList(ui->rx_settings_cmb_rx_fir, "adi,rx-profile-rx-fir-decimation",  new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->rx_settings_txt_rx_dec5_decimation, "adi,rx-profile-rx-dec5-decimation", NULL, 0);
    AddWidgetToList(ui->rx_settings_chk_en_high_rej_dec5, "adi,rx-profile-en-high-rej-dec5", NULL, 0);
    AddWidgetToList(ui->rx_settings_txt_rhb1_decimation, "adi,rx-profile-rhb1-decimation", NULL, 0);
    AddWidgetToList(ui->rx_settings_txt_iq_rate_khz, "adi,rx-profile-iq-rate_khz", NULL, 0);
    AddWidgetToList(ui->rx_settings_txt_rf_bandwith_hz, "adi,rx-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->rx_settings_txt_rx_bbf_3db_corner_khz, "adi,rx-profile-rx-bbf-3db-corner_khz", NULL, 0);

    AddWidgetToList(ui->obs_settings_txt_adc_div, "adi,obs-profile-adc-div", NULL, 0);
    AddWidgetToList(ui->obs_settings_cmb_rx_fir, "adi,obs-profile-rx-fir-decimation",  new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->obs_settings_txt_rx_dec5_decimation, "adi,obs-profile-rx-dec5-decimation", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_en_high_rej_dec5, "adi,obs-profile-en-high-rej-dec5", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_rhb1_decimation, "adi,obs-profile-rhb1-decimation", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_iq_rate_khz, "adi,obs-profile-iq-rate_khz", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_rf_bandwith_hz, "adi,obs-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_rx_bbf_3db_corner_khz, "adi,obs-profile-rx-bbf-3db-corner_khz", NULL, 0);

    AddWidgetToList(ui->obs_settings_txt_sniffer_adc_div_, "adi,sniffer-profile-adc-div", NULL, 0);
    AddWidgetToList(ui->obs_settings_cmb_sniffer_rx_fir, "adi,sniffer-profile-rx-fir-decimation",  new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->obs_settings_txt_sniffer_rx_dec5_decimation, "adi,sniffer-profile-rx-dec5-decimation", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_sniffer_en_high_rej_dec5, "adi,sniffer-profile-en-high-rej-dec5", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_sniffer_rx_dec5_decimation, "adi,sniffer-profile-rhb1-decimation", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_sniffer_iq_rate_khz, "adi,sniffer-profile-iq-rate_khz", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_sniffer_rf_bandwith_hz, "adi,sniffer-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->obs_settings_txt_sniffer_rx_bbf_3db_corner_khz, "adi,sniffer-profile-rx-bbf-3db-corner_khz", NULL, 0);

    AddWidgetToList(ui->tx_settings_cmb_dac_div, "adi,tx-profile-dac-div",  new (unsigned char[3]){0,1,2}, 3);
    AddWidgetToList(ui->tx_settings_cmb_tx_fir, "adi,tx-profile-tx-fir-interpolation",  new (unsigned char[3]){1,2,4}, 3);
    AddWidgetToList(ui->tx_settings_txt_thb1_interpolation, "adi,tx-profile-thb1-interpolation", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_thb2_interpolation, "adi,tx-profile-thb2-interpolation", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx_input_hp_interpolation, "adi,tx-profile-tx-input-hb-interpolation", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_iq_rate, "adi,tx-profile-iq-rate_khz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_primary_sig_bandwidth_hz, "adi,tx-profile-primary-sig-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_rf_bandwidth_hz, "adi,tx-profile-rf-bandwidth_hz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx_dac_3db_corner_khz, "adi,tx-profile-tx-dac-3db-corner_khz", NULL, 0);
    AddWidgetToList(ui->tx_settings_txt_tx_bbf_3db_corner_khz, "adi,tx-profile-tx-bbf-3db-corner_khz", NULL, 0);

    AddWidgetToList(ui->tx_settings_channel_enable, "adi,tx-settings-tx-channels-enable",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->tx_settings_chk_tx_pll_use_external, "adi,tx-settings-tx-pll-use-external-lo", NULL, 0);
    AddWidgetToList(ui->tx_settings_tx_pll_lo_frequency, "adi,tx-settings-tx-pll-lo-frequency_hz", NULL, 0);
    AddWidgetToList(ui->tx_settings_cmb_step_size, "adi,tx-settings-tx-atten-step-size",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->tx_settings_tx1_atten, "adi,tx-settings-tx1-atten_mdb", NULL, 0);
    AddWidgetToList(ui->tx_settings_tx2_atten, "adi,tx-settings-tx2-atten_mdb", NULL, 0);

    AddWidgetToList(ui->rx_settings_cmb_channel_enable, "adi,rx-settings-rx-channels-enable",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->rx_settings_chk_rx_pll_use_external_lo, "adi,rx-settings-rx-pll-use-external-lo", NULL, 0);
    AddWidgetToList(ui->rx_settings_txt_rx_pll_lo_frequency_hz, "adi,rx-settings-rx-pll-lo-frequency_hz", NULL, 0);
    AddWidgetToList(ui->rx_settings_real_if_data, "adi,rx-settings-real-if-data", NULL, 0);

    AddWidgetToList(ui->obs_settings_chk_orx1, "adi,obs-settings-obs-rx-channels-enable#0", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_orx2, "adi,obs-settings-obs-rx-channels-enable#1", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_snrxa, "adi,obs-settings-obs-rx-channels-enable#2", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_snrxb, "adi,obs-settings-obs-rx-channels-enable#3", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_snrxc, "adi,obs-settings-obs-rx-channels-enable#4", NULL, 0);

    AddWidgetToList(ui->obs_settings_cmb_lo_source, "adi,obs-settings-obs-rx-lo-source",  new (unsigned char[2]){0, 1}, 2);
    AddWidgetToList(ui->obs_settings_txt_sniffer_pll_lo_frequency_hz, "adi,obs-settings-sniffer-pll-lo-frequency_hz", NULL, 0);
    AddWidgetToList(ui->obs_settings_chk_rx_fir, "adi,obs-settings-real-if-data", NULL, 0);
    AddWidgetToList(ui->obs_settings_cmb_default_channel, "adi,obs-settings-default-obs-rx-channel",  new (unsigned char[10]){0,1,2,3,4,5,6,0x14,0x24,0x34}, 10);

    AddWidgetToList(ui->arm_gpio_chk_use_rx2_enable_pin, "adi,arm-gpio-use-rx2-enable-pin", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_use_tx2_enable_pin, "adi,arm-gpio-use-tx2-enable-pin", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_tx_rx_pin_mode, "adi,arm-gpio-tx-rx-pin-mode", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_orx_pin_mode, "adi,arm-gpio-orx-pin-mode", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx_trigger_pin, "adi,arm-gpio-orx-trigger-pin", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx_mode2_pin, "adi,arm-gpio-orx-mode2-pin", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx_mode1_pin, "adi,arm-gpio-orx-mode1-pin", NULL, 0);
    AddWidgetToList(ui->arm_gpio_txt_orx_mode0_pin, "adi,arm-gpio-orx-mode0-pin", NULL, 0);
    AddWidgetToList(ui->arm_gpio_chk_rx1_enable_ack, "adi,arm-gpio-rx1-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_rx2_enable_ack, "adi,arm-gpio-rx2-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_tx1_enable_ack, "adi,arm-gpio-tx1-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_tx2_enable_ack, "adi,arm-gpio-tx2-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_orx1_enable_ack, "adi,arm-gpio-orx1-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_orx2_enable_ack, "adi,arm-gpio-orx2-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_srx_enable_ack, "adi,arm-gpio-srx-enable-ack", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_chk_tx_obs_select, "adi,arm-gpio-tx-obs-select", NULL, 4); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_rx1_enable_ack, "adi,arm-gpio-rx1-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_rx2_enable_ack, "adi,arm-gpio-rx2-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_tx1_enable_ack, "adi,arm-gpio-tx1-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_tx2_enable_ack, "adi,arm-gpio-tx2-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_orx1_enable_ack, "adi,arm-gpio-orx1-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_orx2_enable_ack, "adi,arm-gpio-orx2-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_srx_enable_ack, "adi,arm-gpio-srx-enable-ack#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->arm_gpio_txt_tx_obs_select, "adi,arm-gpio-tx-obs-select#4", NULL, 0); /* Special handling */
    AddWidgetToList(ui->gpio_chk_3v_0_3_enable1, "adi,gpio-3v3-oe-mask#0", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_0_3_enable2, "adi,gpio-3v3-oe-mask#1", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_0_3_enable3, "adi,gpio-3v3-oe-mask#2", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_0_3_enable4, "adi,gpio-3v3-oe-mask#3", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_4_7_enable1, "adi,gpio-3v3-oe-mask#4", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_4_7_enable2, "adi,gpio-3v3-oe-mask#5", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_4_7_enable3, "adi,gpio-3v3-oe-mask#6", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_4_7_enable4, "adi,gpio-3v3-oe-mask#7", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_8_11_enable1, "adi,gpio-3v3-oe-mask#8", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_8_11_enable2, "adi,gpio-3v3-oe-mask#9", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_8_11_enable3, "adi,gpio-3v3-oe-mask#10", NULL, 0);
    AddWidgetToList(ui->gpio_chk_3v_8_11_enable4, "adi,gpio-3v3-oe-mask#11", NULL, 0);
    AddWidgetToList(ui->gpio_cmb_3v_0_3, "adi,gpio-3v3-src-ctrl3_0",  new (unsigned char[4]){1,2,3,4}, 4);
    AddWidgetToList(ui->gpio_cmb_3v_4_7, "adi,gpio-3v3-src-ctrl7_4",  new (unsigned char[4]){1,2,3,4}, 4);
    AddWidgetToList(ui->gpio_cmb_3v_8_11, "adi,gpio-3v3-src-ctrl11_8",  new (unsigned char[4]){1,2,3,4}, 4);

    AddWidgetToList(ui->gpio_chk_low_voltage_0_3_enable1, "adi,gpio-oe-mask#0", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_0_3_enable2, "adi,gpio-oe-mask#1", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_0_3_enable3, "adi,gpio-oe-mask#2", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_0_3_enable4, "adi,gpio-oe-mask#3", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_4_7_enable1, "adi,gpio-oe-mask#4", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_4_7_enable2, "adi,gpio-oe-mask#5", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_4_7_enable3, "adi,gpio-oe-mask#6", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_4_7_enable4, "adi,gpio-oe-mask#7", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_8_11_enable1, "adi,gpio-oe-mask#8", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_8_11_enable2, "adi,gpio-oe-mask#9", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_8_11_enable3, "adi,gpio-oe-mask#10", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_8_11_enable4, "adi,gpio-oe-mask#11", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_12_15_enable1, "adi,gpio-oe-mask#12", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_12_15_enable2, "adi,gpio-oe-mask#13", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_12_15_enable3, "adi,gpio-oe-mask#14", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_12_15_enable4, "adi,gpio-oe-mask#15", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_16_18_enable1, "adi,gpio-oe-mask#16", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_16_18_enable2, "adi,gpio-oe-mask#17", NULL, 0);
    AddWidgetToList(ui->gpio_chk_low_voltage_16_18_enable3, "adi,gpio-oe-mask#18", NULL, 0);
    AddWidgetToList(ui->gpio_cmb_low_voltage_0_3, "adi,gpio-src-ctrl3_0",  new (unsigned char[4]){0,3,9,10}, 4);
    AddWidgetToList(ui->gpio_cmb_low_voltage_4_7, "adi,gpio-src-ctrl7_4",  new (unsigned char[4]){0,3,9,10}, 4);
    AddWidgetToList(ui->gpio_cmb_low_voltage_8_11, "adi,gpio-src-ctrl11_8",  new (unsigned char[4]){0,3,9,10}, 4);
    AddWidgetToList(ui->gpio_cmb_low_voltage_12_15, "adi,gpio-src-ctrl15_12",  new (unsigned char[4]){0,3,9,10}, 4);
    AddWidgetToList(ui->gpio_cmb_low_voltage_16_18, "adi,gpio-src-ctrl18_16",  new (unsigned char[4]){0,3,9,10}, 4);

    AddWidgetToList(ui->aux_dac_txt_dac0, "adi,aux-dac-enable-mask#0", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac1, "adi,aux-dac-enable-mask#1", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac2, "adi,aux-dac-enable-mask#2", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac3, "adi,aux-dac-enable-mask#3", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac4, "adi,aux-dac-enable-mask#4", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac5, "adi,aux-dac-enable-mask#5", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac6, "adi,aux-dac-enable-mask#6", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac7, "adi,aux-dac-enable-mask#7", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac8, "adi,aux-dac-enable-mask#8", NULL, 0);
    AddWidgetToList(ui->aux_dac_txt_dac9, "adi,aux-dac-enable-mask#9", NULL, 0);

    AddWidgetToList(ui->aux_dac_txt_dac0, "adi,aux-dac-value0", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac0_slop, "adi,aux-dac-slope0",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac0_vref, "adi,aux-dac-vref0",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac1, "adi,aux-dac-value1", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac1_slop, "adi,aux-dac-slope1",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac1_vref, "adi,aux-dac-vref1",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac2, "adi,aux-dac-value2", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac2_slop, "adi,aux-dac-slope2",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac2_vref, "adi,aux-dac-vref2",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac3, "adi,aux-dac-value3", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac3_slop, "adi,aux-dac-slope3",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac3_vref, "adi,aux-dac-vref3",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac4, "adi,aux-dac-value4", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac4_slop, "adi,aux-dac-slope4",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac4_vref, "adi,aux-dac-vref4",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac5, "adi,aux-dac-value5", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac5_slop, "adi,aux-dac-slope5",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac5_vref, "adi,aux-dac-vref5",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac6, "adi,aux-dac-value6", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac6_slop, "adi,aux-dac-slope6",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac6_vref, "adi,aux-dac-vref6",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac7, "adi,aux-dac-value7", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac7_slop, "adi,aux-dac-slope7",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac7_vref, "adi,aux-dac-vref7",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac8, "adi,aux-dac-value8", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac8_slop, "adi,aux-dac-slope8",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac8_vref, "adi,aux-dac-vref8",  new (unsigned char[4]){0,1,2,3}, 4);
    AddWidgetToList(ui->aux_dac_txt_dac9, "adi,aux-dac-value9", NULL, 0);
    AddWidgetToList(ui->aux_dac_cmb_dac9_slop, "adi,aux-dac-slope9",  new (unsigned char[2]){0,1}, 2);
    AddWidgetToList(ui->aux_dac_cmb_dac9_vref, "adi,aux-dac-vref9",  new (unsigned char[4]){0,1,2,3}, 4);

    AddWidgetToList(ui->calibration_chk_rx_qec, "adi,default-initial-calibrations-mask#14", NULL, 0);
    AddWidgetToList(ui->calibration_chk_tx_qec, "adi,default-initial-calibrations-mask#10", NULL, 0);
    AddWidgetToList(ui->calibration_chk_external_tx_lol, "adi,default-initial-calibrations-mask#9", NULL, 0);
    AddWidgetToList(ui->calibration_chk_tx_lol, "adi,default-initial-calibrations-mask#8", NULL, 0);
    AddWidgetToList(ui->calibration_chk_dpd, "adi,default-initial-calibrations-mask#15", NULL, 0);
    AddWidgetToList(ui->calibration_chk_clgc, "adi,default-initial-calibrations-mask#16", NULL, 0);
    AddWidgetToList(ui->calibration_chk_vswr, "adi,default-initial-calibrations-mask#17", NULL, 0);


    AddWidgetToList(ui->dpd_settings_txt_damping, "adi,dpd-damping", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_num_weights, "adi,dpd-num-weights", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_model_version, "adi,dpd-model-version", NULL, 0);
    AddWidgetToList(ui->dpd_settings_chk_high_power_model_update, "adi,dpd-high-power-model-update", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_model_prior_weight, "adi,dpd-model-prior-weight", NULL, 0);
    AddWidgetToList(ui->dpd_settings_chk_robust_modeling, "adi,dpd-robust-modeling", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_samples, "adi,dpd-samples", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_outlier_threshold, "adi,dpd-outlier-threshold", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_additional_delay_offset, "adi,dpd-additional-delay-offset", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_path_delay_pn_seq_level, "adi,dpd-path-delay-pn-seq-level", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_weight0_real, "adi,dpd-weights0-real", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_weight0_imag, "adi,dpd-weights0-imag", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_weight1_real, "adi,dpd-weights1-real", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_weight1_imag, "adi,dpd-weights1-imag", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_weight2_real, "adi,dpd-weights2-real", NULL, 0);
    AddWidgetToList(ui->dpd_settings_txt_weight2_imag, "adi,dpd-weights2-imag", NULL, 0);

    AddWidgetToList(ui->clgc_settings_txt_desired_gain_tx1, "adi,clgc-tx1-desired-gain", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_desired_gain_tx2, "adi,clgc-tx2-desired-gain", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_atten_limit_tx1, "adi,clgc-tx1-atten-limit", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_atten_limit_tx2, "adi,clgc-tx2-atten-limit", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_control_ratio_tx1, "adi,clgc-tx1-control-ratio", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_control_ratio_tx2, "adi,clgc-tx2-control-ratio", NULL, 0);
    AddWidgetToList(ui->clgc_settings_chk_allow_atten_updates_tx1, "adi,clgc-allow-tx1-atten-updates", NULL, 0);
    AddWidgetToList(ui->clgc_settings_chk_allow_atten_updates_tx2, "adi,clgc-allow-tx2-atten-updates", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_additional_delay_offset, "adi,clgc-additional-delay-offset", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_path_delay_pn_seq_level, "adi,clgc-path-delay-pn-seq-level", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_rel_threshold_tx1, "adi,clgc-tx1-rel-threshold", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_rel_threshold_tx2, "adi,clgc-tx2-rel-threshold", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_rel_threshold_tx1, "adi,clgc-tx1-rel-threshold-en", NULL, 0);
    AddWidgetToList(ui->clgc_settings_txt_rel_threshold_tx2, "adi,clgc-tx2-rel-threshold-en", NULL, 0);

    AddWidgetToList(ui->vswr_settings_txt_additional_delay_offset, "adi,vswr-additional-delay-offset", NULL, 0);
    AddWidgetToList(ui->vswr_settings_txt_path_delay_pn_seq_level, "adi,vswr-path-delay-pn-seq-level", NULL, 0);
    AddWidgetToList(ui->vswr_settings_txt_vswr_switch_gpio_3p3_pint_tx1, "adi,vswr-tx1-vswr-switch-gpio3p3-pin", NULL, 0);
    AddWidgetToList(ui->vswr_settings_txt_vswr_switch_gpio_3p3_pint_tx2_38, "adi,vswr-tx2-vswr-switch-gpio3p3-pin", NULL, 0);
    AddWidgetToList(ui->vswr_settings_chk_vswr_switch_ploarity_tx1, "adi,vswr-tx1-vswr-switch-polarity", NULL, 0);
    AddWidgetToList(ui->vswr_settings_chk_vswr_switch_ploarity_tx2, "adi,vswr-tx2-vswr-switch-polarity", NULL, 0);
    AddWidgetToList(ui->vswr_settings_txt_vswr_switch_delay_tx1, "adi,vswr-tx1-vswr-switch-delay_us", NULL, 0);
    AddWidgetToList(ui->vswr_settings_txt_vswr_switch_delay_tx2, "adi,vswr-tx2-vswr-switch-delay_us", NULL, 0);

    AddWidgetToList(ui->bist_cmb_prbs_rx, "bist_prbs_rx",  new (unsigned char[4]){0, 1, 2, 3}, 4);
    AddWidgetToList(ui->bist_cmb_prbs_obs, "bist_prbs_obs",  new (unsigned char[4]){0, 1, 2, 3}, 4);
    AddWidgetToList(ui->bist_chk_rx_loopback, "loopback_tx_rx", NULL, 0);
    AddWidgetToList(ui->bist_chk_obs_loopback, "loopback_tx_obs", NULL, 0);

    AddWidgetToList(ui->btn_save_settings, "initialize", NULL, 0);

}

//,struct osc_plugin *plugin, const char *ini_fn
QWidget * ad9371_adv::init()
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

    ui->mainTabContainer->removeTab(ui->mainTabContainer->count()-1);
    ui->mainTabContainer->removeTab(ui->mainTabContainer->count()-1);
    ui->mainTabContainer->removeTab(ui->mainTabContainer->count()-1);

    globals::attrs=new QList<w_info*>();

    InitializeWidgets();

    globals::dev = iio_context_find_device(globals::ctx, PHY_DEVICE);

    /* DPD, CLGC and VSWR is AD9375 only */
//    if (iio_device_find_debug_attr(dev, "adi,dpd-model-version") == NULL) {
//        qInfo()<<"AD9375";
//    }

    globals::connect_widgets(new QWidget());

    // Missing return: falling off the end of a QWidget*-returning function is
    // undefined behavior and can abort with SIGILL on optimized builds.
    return NULL;
}

void ad9371_adv::on_btn_save_settings_clicked()
{
    iio_device_debug_attr_write_longlong(globals::dev, "initialize", 1);
}

