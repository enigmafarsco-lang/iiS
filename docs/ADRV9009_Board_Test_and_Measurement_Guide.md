# ADRV9009 RF Transceiver Board Test, Verification & Measurement Manual
## دستورالعمل جامع آزمون، ارزیابی و اندازه‌گیری آزمایشگاهی برد ترنسور رادیویی ADRV9009

---

### Document Overview & Metadata / مشخصات و شناسنامه سند
- **Target Transceiver IC:** Analog Devices ADRV9009 (Dual RF Transceiver + Integrated Fractional-N Synthesizers)
- **Host Platform / Baseband:** Xilinx Zynq UltraScale+ MPSoC / Zynq-7000 (AXI4-Lite & JESD204B Subclass 1)
- **Operating Software:** Linux Industrial I/O (IIO), libiio v0.24+, eLynxSDR Workstation v0.0.2
- **Document Version:** v1.2.0 — October 2026

---

# Part 1: English Technical Reference & Measurement Procedures

## 1. Executive Summary & Hardware Verification Strategy
The **ADRV9009** is a wideband, direct-conversion (Zero-IF) RF transceiver IC providing dual transmitters (TX1, TX2), dual receivers (RX1, RX2), and dual observation receivers (ORX1, ORX2) with integrated local oscillators. Supporting continuous frequency tuning from **75 MHz to 6000 MHz**, up to **200 MHz receiver bandwidth**, and **450 MHz transmitter synthesis bandwidth**, it forms the core RF frontend of the eLynxSDR software-defined radio system.

This manual defines standard laboratory verification procedures to certify board manufacturing, RF performance, digital JESD204B data integrity, power distribution, and dynamic tracking calibration.

---

## 2. ADRV9009 Datasheet Parameter Matrix & Verification Criteria

| Subsystem / Parameter | Datasheet Min | Nominal / Typ | Datasheet Max | Unit / Test Conditions |
| :--- | :--- | :--- | :--- | :--- |
| **RF Carrier Tuning Range** | 75 | — | 6000 | MHz (Continuous tuning) |
| **TX Synthesis Bandwidth** | — | 200 / 400 | 450 | MHz (Profile dependent) |
| **RX Instantaneous Bandwidth** | — | 100 / 200 | 200 | MHz |
| **ORX Observation Bandwidth** | — | 200 / 400 | 450 | MHz (DPD / Sniffer mode) |
| **TX Max Output Power (P1dB)** | +6.0 | +8.5 | +10.0 | dBm (CW tone @ 2.4 GHz) |
| **TX Digital Attenuation Range** | 0.0 | — | 41.95 | dB (0.05 dB resolution) |
| **TX Carrier Leakage (LOL)** | — | -85.0 | -75.0 | dBc (With tracking cal) |
| **TX Image Rejection (QEC)** | 70.0 | 80.0 | — | dBc (With tracking cal) |
| **TX Phase Noise @ 100 kHz offset** | — | -102.0 | -98.0 | dBc/Hz (f0 = 2.0 GHz) |
| **TX Phase Noise @ 10 MHz offset** | — | -138.0 | -135.0 | dBc/Hz (f0 = 2.0 GHz) |
| **TX Error Vector Magnitude (EVM)** | — | 0.65 | 1.0 | % rms (LTE 20 MHz 64-QAM) |
| **RX Noise Figure (NF) @ 2 GHz** | — | 12.0 | 13.5 | dB (Max gain state) |
| **RX Noise Figure (NF) @ 5 GHz** | — | 15.0 | 16.5 | dB (Max gain state) |
| **RX Input IP3 (IIP3)** | +15.0 | +18.0 | — | dBm (Min attenuation) |
| **RX Input P1dB** | -14.0 | -12.0 | — | dBm (Max gain state) |
| **RX Image Rejection (QEC)** | 70.0 | 82.0 | — | dBc (Tracking QEC active) |
| **RX Gain Control Range** | 0.0 | — | 30.0 | dB (0.5 dB step MGC/AGC) |
| **JESD204B SerDes Data Rate** | 1.2288 | 9.8304 | 12.288 | Gbps per lane (Subclass 1) |
| **Supply 1.0V Digital Core** | 0.95 | 1.00 | 1.05 | V (Ripple < 5 mV rms) |
| **Supply 1.3V Analog RF/PLL** | 1.24 | 1.30 | 1.36 | V (Ripple < 3 mV rms) |
| **Supply 1.8V Aux / I/O** | 1.71 | 1.80 | 1.89 | V (Ripple < 10 mV rms) |
| **Junction Temp (Tj)** | -40 | +45 | +110 | °C (Internal diode sensor) |

---

## 3. Laboratory Test Bench & Equipment Configuration
- **Vector Signal Analyzer (VSA) / Spectrum Analyzer:** Keysight N9020B (MXA) / R&S FSV3000 (10 Hz – 13.6 GHz, >= 160 MHz BW).
- **Vector Signal Generator (VSG):** Keysight N5182B (MXG) / R&S SMBV100B (9 kHz – 6 GHz, ARB baseband).
- **Digital Storage Oscilloscope:** Keysight Infiniium / R&S RTO (>= 4 GHz BW, 20 GSa/s).
- **Precision RF Power Meter & Sensors:** Keysight N1914A + E9304A Average Power Sensor (-70 dBm to +20 dBm).
- **Noise Figure Analyzer / Noise Source:** Keysight N4000A SNS Noise Source (10 MHz – 18 GHz, ENR 15 dB).
- **Precision DC Power Supply:** Keysight E36313A / Keithley 2230 (< 1 mV rms ripple).
- **Host PC:** Ubuntu 22.04 LTS with USB-UART (`/dev/ttyUSB0`) and Gigabit LAN.

---

## 4. RF Transmitter (TX1 / TX2) Measurement Procedures

### 4.1 Transmit Power & Dynamic Range
1. Connect TX1 (or TX2) output through a calibrated 20 dB RF attenuator to the RF Power Meter.
2. Program the RF carrier to 2400 MHz via IIO:
   ```bash
   iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 2400000000
   ```
3. Enable single-tone DDS generation at 5 MHz offset with 0 dBFS amplitude:
   ```bash
   iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 1.0
   ```
4. Step digital attenuation from 0 dB to 40 dB in 5 dB increments via `hardwaregain`:
   ```bash
   iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain <gain_value>
   ```
5. Record measured power and verify step linearity (< ±0.3 dB error).

### 4.2 Local Oscillator Carrier Leakage (LOL) & Image Rejection (QEC)
1. Connect TX1 to the Spectrum Analyzer (Center Freq = 2400 MHz, Span = 50 MHz, RBW = 10 kHz).
2. Synthesize a CW tone at +10 MHz baseband offset (RF tone at 2410 MHz).
3. Measure the three spectral components:
   - **Desired RF Tone:** $f_0 + 10\text{ MHz}$ ($P_{\text{Tone}}$)
   - **LO Carrier Leakage:** $f_0 = 2400\text{ MHz}$ ($P_{\text{LO}}$)
   - **Quadrature Image:** $f_0 - 10\text{ MHz} = 2390\text{ MHz}$ ($P_{\text{Image}}$)
4. Compute Carrier Suppression: $\text{LOL (dBc)} = P_{\text{LO}} - P_{\text{Tone}}$. *(Must be $\le -75\text{ dBc}$)*
5. Compute Image Rejection: $\text{QEC (dBc)} = P_{\text{Image}} - P_{\text{Tone}}$. *(Must be $\le -70\text{ dBc}$)*

---

## 5. RF Receiver (RX1 / RX2) Measurement Procedures

### 5.1 Noise Figure (NF) & Sensitivity
1. Connect the calibrated Noise Source (Keysight N4000A) to the RX1 SMA port.
2. Digitize baseband output and compute the Y-Factor:
   $$Y = \frac{P_{\text{hot}}}{P_{\text{cold}}}$$
   $$\text{NF (dB)} = \text{ENR} - 10 \log_{10}(Y - 1)$$
3. Compute Minimum Detectable Signal (MDS):
   $$\text{MDS (dBm)} = -174\text{ dBm/Hz} + \text{NF} + 10\log_{10}(\text{BW}_{\text{Hz}}) + \text{SNR}_{\text{min}}$$
4. **Pass Criteria:** $\text{NF} \le 13.5\text{ dB}$ at 2.4 GHz; $\text{NF} \le 16.5\text{ dB}$ at 5.8 GHz.

### 5.2 Input Third-Order Intercept Point (IIP3)
1. Combine two CW tones at $f_1 = 2400\text{ MHz}$ and $f_2 = 2401\text{ MHz}$ with $P_{\text{in}} = -25\text{ dBm}$ per tone.
2. Apply two-tone signal to RX1 and capture baseband FFT spectrum.
3. Measure fundamental tone power $P_{\text{fund}}$ and 3rd-order intermodulation products $P_{\text{IM3}}$ at $2f_1 - f_2$ and $2f_2 - f_1$.
4. Calculate IIP3:
   $$\text{IIP3} = P_{\text{in}} + \frac{P_{\text{fund}} - P_{\text{IM3}}}{2}$$
5. **Pass Criteria:** $\text{IIP3} \ge +15\text{ dBm}$ at minimum attenuation.

---

# Part 2: Persian Translation & Guidelines (بخش فارسی)

## ۱. معماری تراشه ADRV9009 و راهبرد آزمون سخت‌افزار
تراشه **ADRV9009** یک ترنسور رادیویی فوق‌پهن‌باند با معماری تبدیل مستقیم (Zero-IF) است که شامل دو فرستنده، دو گیرنده اصلی و دو گیرنده ناظر (ORX) با سینتی‌سایزرهای فرکانسی مجتمع می‌باشد. این تراشه فرکانس‌های رادیویی از **۷۵ مگاهرتز تا ۶۰۰۰ مگاهرتز** را با پهنای باند سنتز فرستنده تا **۴۵۰ مگاهرتز** و گیرندگی تا **۲۰۰ مگاهرتز** پوشش می‌دهد.

این دستورالعمل روش‌های استاندارد آزمون جهت تایید صحت تولید، ارزیابی مشخصات RF، خطوط دیجیتال JESD204B، ریپل تغذیه و کالیبراسیون‌های خودکار را شرح می‌دهد.

---

## ۲. جدول معیارهای پذیرش و رد آزمون آزمایشگاهی

| ردیف | نام پارامتر / آزمون | حداقل استاندارد | مقدار نامی (Typ) | حداکثر مجاز | واحد / شرایط آزمون |
| :---: | :--- | :---: | :---: | :---: | :--- |
| ۱ | محدوده تنظیم فرکانس کریر | 75 | — | 6000 | مگاهرتز (پیوسته) |
| ۲ | توان خروجی اشباع فرستنده | +6.0 | +8.5 | +10.0 | dBm (در فرکانس 2.4 GHz) |
| ۳ | سرکوب نشتی نوسان‌ساز محلی (LOL) | — | -85.0 | -75.0 | dBc (کالیبراسیون ردیابی فعال) |
| ۴ | سرکوب تصویر فرستنده (TX QEC) | 70.0 | 80.0 | — | dBc (کالیبراسیون ردیابی فعال) |
| ۵ | نویز فاز فرستنده در آفست 100 kHz | — | -102.0 | -98.0 | dBc/Hz (در فرکانس 2 GHz) |
| ۶ | بردار قدر مطلق خطای فرستنده (EVM) | — | 0.65 | 1.0 | % rms (مدولاسیون 64-QAM) |
| ۷ | ضریب نویز گیرنده (NF) در 2 GHz | — | 12.0 | 13.5 | dB (حالت حداکثر گین) |
| ۸ | ضریب نویز گیرنده (NF) در 5 GHz | — | 15.0 | 16.5 | dB (حالت حداکثر گین) |
| ۹ | نقطه برخورد مرتبه سوم ورودی (IIP3) | +15.0 | +18.0 | — | dBm (حالت حداقل گین) |
| ۱۰ | نرخ تبادل داده سریال JESD204B | 1.2288 | 9.8304 | 12.288 | Gbps بر روی هر خط |
| ۱۱ | ولتاژ هسته دیجیتال (1.0V Core) | 0.95 | 1.00 | 1.05 | ولت (ریپل کمتر از 5 mV) |
| ۱۲ | دمای جانکشن تراشه (Tj) | -40 | +45 | +110 | درجه سانتی‌گراد |

---

## ۳. دستورالعمل آزمون‌های آزمایشگاهی فرستنده و گیرنده

### ۳.۱ اندازه‌گیری توان خروجی و گستره دینامیکی فرستنده
۱. خروجی پورت TX1 را از طریق تضعیف‌کننده ۲۰ دسی‌بل به توان‌سنج RF کالیبره‌شده متصل کنید.
۲. فرکانس نوسان‌ساز محلی را بر روی ۲۴۰۰ مگاهرتز تنظیم نمایید.
۳. تن تک‌فرکانس DDS با دامنه 0 dBFS فعال کرده و توان خروجی را در تضعیف‌های ۰ تا ۴۰ دسی‌بل ثبت نمایید.
۴. **معیار قبولی:** توان خروجی در ۲.۴ گیگاهرتز بیشتر از ۶+ dBm و خطای گام تضعیف کمتر از ۰.۳± دسی‌بل باشد.

### ۳.۲ اندازه‌گیری سرکوب نشتی کریر (LOL) و سیگنال تصویر (QEC)
۱. خروجی فرستنده را به اسپکتروم آنالایزر با رزولوشن ۱۰ کیلوهرتز متصل نمایید.
۲. تن تک‌فرکانس با آفست ۱۰+ مگاهرتز نسبت به کریر تولید کنید.
۳. توان نشتی کریر ($f_0$) و توان تصویر ($f_0 - 10\text{ MHz}$) را نسبت به سیگنال اصلی ($f_0 + 10\text{ MHz}$) ثبت کنید.
۴. **معیار قبولی:** مقدار LOL کمتر از ۷۵- dBc و مقدار QEC بهتر از ۷۰- dBc باشد.

### ۳.۳ اندازه‌گیری ضریب نویز و حساسیت گیرنده
۱. منبع نویز کالیبره (Noise Source) را به ورودی پورت RX1 متصل کنید.
۲. با متد Y-Factor، نسبت توان گرم به سرد را اندازه‌گیری کرده و ضریب نویز را استخراج نمایید.
۳. **معیار قبولی:** ضریب نویز در ۲.۴ گیگاهرتز کمتر از ۱۳.۵ دسی‌بل و در ۵.۸ گیگاهرتز کمتر از ۱۶.۵ دسی‌بل باشد.
