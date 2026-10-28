# ADRV9009 RF Transceiver Board Hardware Maintenance, Troubleshooting & Repair Manual with Full Test & Measurement Instructions
## دستورالعمل جامع نگهداری پیشگیرانه، عیب‌یابی و تعمیرات سخت‌افزاری برد فرستنده/گیرنده ADRV9009 به همراه دستورالعمل‌های تست و اندازه‌گیری

---

### Document Overview & Metadata / مشخصات و شناسنامه سند
- **Hardware Under Maintenance:** ADRV9009 Dual-Channel Wideband RF Transceiver Evaluation & Baseband Carrier Board
- **Maintenance Standards:** IPC-7711/7721 (Rework & Repair of Electronic Assemblies), IPC-A-610 Class 3, IEEE 1785
- **Document Version:** v1.1.0 — October 2026 | Technical Maintenance & Test Manual for eLynxSDR System Engineers

---

# Part 1: English Hardware Maintenance & Repair Procedures

## 1. Preventative Maintenance Schedules & Visual Inspection Standards

| Inspection Interval | Target Subsystem / Component | Action Required & Standard | Tools / Materials |
| :--- | :--- | :--- | :--- |
| **Daily / Pre-Operation** | RF SMA Connectors (TX1, TX2, RX1, RX2, ORX) | Inspect center pin integrity, verify no debris or stripped threads. Torque to 8 in-lbs (0.9 N·m). | 5/16-inch SMA Torque Wrench, Lint-free swabs |
| **Monthly** | Cooling Fan & Heatsink Fins | Inspect dust buildup, verify tachometer RPM (> 4500 RPM), clear airflow channels. | Dry compressed air (< 30 psi), Soft brush |
| **Quarterly (3 Months)**| Power Supply Rails & Ripple | Probe test points TP1–TP6; verify AC ripple < 5 mV rms on 1.0V and 1.3V rails. | 6.5-digit Multimeter, 1 GHz Oscilloscope |
| **Semi-Annually (6 Mo)**| Thermal Interface Material (TIM) | Inspect thermal pad elasticity. Replace dried thermal paste (PTM7950 phase change material). | 99.9% Isopropyl Alcohol (IPA), TIM pad kit |
| **Annually** | Solder Joints & BGA Underfill | 20x stereo microscope inspection for solder oxidation, tin whiskers, and conformal coating voids. | Stereo Microscope, UV Inspection Lamp |

> **CRITICAL ESD PROTECTION WARNING:**  
> The ADRV9009 IC contains ultra-sensitive RF GaAs/CMOS structures sensitive to ESD pulses < 250V (HBM Class 1A). Always wear a grounded wrist strap (1 MΩ series resistor) and work on an ESD-dissipative mat.

---

## 2. Altium Designer Job 1 & Job 2 Documentation & PCB Check Points

Technicians performing hardware troubleshooting must cross-reference the Altium OutJob schematic sheets (Job 1) and PCB assembly layer drawings (Job 2).

### Figure 1.1: Altium Output Job 1 — Schematic Sheet & Diagnostic Check Points
![Altium Job 1 Schematic](images/altium_job1_schematic_checkpoints.png)
*Figure 1.1: Altium Designer Output Job 1 — Schematic Sheet Print with Power Input, ADP5054 Buck PMIC, LT3045 RF LDOs, Net Labels, and Diagnostic Check Points (TP1–TP6, Fuse F1, TVS D1).*

### Figure 1.2: Altium Output Job 2 — PCB Top & Bottom Assembly Layers
![Altium Job 2 PCB Assembly](images/altium_job2_pcb_assembly_top_bottom.png)
*Figure 1.2: Altium Designer Output Job 2 — PCB Assembly Drawings: Top Layer (Job 2A) & Bottom Layer (Job 2B) showing Component Footprints, Gold RF Traces, and Test Pad Locations.*

---

## 3. DC Power Supply Rail Diagnostics & Test Point Mapping

| Test Point | Power Rail Name | Nominal Voltage | Tolerance | Max AC Ripple | Failure Symptoms & Root Cause |
| :---: | :--- | :---: | :---: | :---: | :--- |
| **TP1** | `VDD_1P0_DIG` | 1.00 V | $\pm 5\%$ (0.95 – 1.05 V) | $< 5\text{ mV rms}$ | JESD204B link down, digital core unbootable. Check ADP5054 Buck 1 / LDO. |
| **TP2** | `VDDA_1P3_RF` | 1.30 V | $\pm 5\%$ (1.24 – 1.36 V) | $< 3\text{ mV rms}$ | Elevated phase noise, LO unlock, mixer distortion. Check LT3045 LDO regulator. |
| **TP3** | `VDDA_1P3_SYNTH` | 1.30 V | $\pm 5\%$ (1.24 – 1.36 V) | $< 2\text{ mV rms}$ | RFPLL fails to lock, carrier frequency drift. Check ferrite bead FB3 filtering. |
| **TP4** | `VDDA_1P8_CLK` | 1.80 V | $\pm 5\%$ (1.71 – 1.89 V) | $< 8\text{ mV rms}$ | HMC7044 clock jitter excessive, missing SYSREF. Check Buck 3 inductor L4. |
| **TP5** | `VCC_3P3_SYS` | 3.30 V | $\pm 5\%$ (3.14 – 3.46 V) | $< 15\text{ mV rms}$| I2C/SPI bus failure, EEPROM unreadable, UART disconnected. Check main 3.3V DC-DC. |
| **TP6** | `VIN_12V0_MAIN` | 12.00 V | $\pm 10\%$ (10.8 – 13.2 V) | $< 50\text{ mV rms}$| Complete board blackout. Check input fuse F1 (5A Fast-blow) & TVS diode D1. |

### Figure 1.3: Power Tree & Voltage Test Points Architecture Schematic
![ADRV9009 Power Tree Schematic](images/adrv9009_power_tree_schematic.png)
*Figure 1.3: ADRV9009 Power Tree Architecture & Voltage Check Points Schematic (ADP5054 PMIC, LT3045 LDOs, Fuse F1, and TP1–TP6).*

---

## 4. Step-by-Step RF Test & Measurement Instructions and Empirical Results

### Figure 1.4: Laboratory RF Test Bench Cabling & Synchronization Setup
![RF Test Bench Setup](images/rf_test_bench_setup_diagram.png)
*Figure 1.4: ADRV9009 RF Test & Measurement Bench Cabling Setup (Keysight/R&S Signal Generator, Spectrum Analyzer, 20dB Calibrated Attenuators, and 10MHz Clock Sync).*

### 4.1 Transmitter (TX) Power, Linearity & Harmonics Measurement Procedure
1. **Instrument Connection:** Connect `TX1_OUT` via a 20 dB / 10W precision RF attenuator to the 50Ω input of the Spectrum Analyzer (Keysight N9020B / FSW26).
2. **Reference Synchronization:** Connect the 10.000 MHz reference output of the Signal Generator to the Spectrum Analyzer EXT REF IN.
3. **Configure ADRV9009 LO:** Set TX LO frequency to 2.6 GHz via Linux IIO command:
   ```bash
   echo 2600000000 > /sys/bus/iio/devices/iio:device1/out_altvoltage0_TX_LO_frequency
   ```
4. **Tone Transmission:** Transmit a continuous wave (CW) digital tone at 0 dB attenuation (`out_voltage0_hardwaregain` = 0 dB).
5. **Spectrum Measurement:** Set Center Frequency = 2.6 GHz, Span = 50 MHz, RBW = 10 kHz, VBW = 1 kHz. Read fundamental power $P_{out}$ (add +20 dB for pad). Span to 12 GHz to record 2nd (5.2 GHz) and 3rd (7.8 GHz) harmonic distortion levels.

| Carrier Frequency | Max TX Output Power | LOL Suppression | Image Rejection | 2nd Harmonic (HD2) | 3rd Harmonic (HD3) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **800 MHz** | $+8.5\text{ dBm} \pm 0.4\text{ dB}$ | $-86.2\text{ dBc}$ | $-82.4\text{ dBc}$ | $-58.5\text{ dBc}$ | $-66.2\text{ dBc}$ |
| **1800 MHz** | $+8.0\text{ dBm} \pm 0.5\text{ dB}$ | $-84.8\text{ dBc}$ | $-80.1\text{ dBc}$ | $-56.2\text{ dBc}$ | $-64.5\text{ dBc}$ |
| **2600 MHz** | $+7.5\text{ dBm} \pm 0.5\text{ dB}$ | $-83.5\text{ dBc}$ | $-78.9\text{ dBc}$ | $-55.0\text{ dBc}$ | $-62.8\text{ dBc}$ |
| **3500 MHz** | $+7.2\text{ dBm} \pm 0.6\text{ dB}$ | $-81.2\text{ dBc}$ | $-76.5\text{ dBc}$ | $-53.4\text{ dBc}$ | $-60.2\text{ dBc}$ |
| **5800 MHz** | $+6.8\text{ dBm} \pm 0.8\text{ dB}$ | $-79.5\text{ dBc}$ | $-74.2\text{ dBc}$ | $-50.8\text{ dBc}$ | $-57.5\text{ dBc}$ |

---

### 4.2 Transmitter Phase Noise Measurement Procedure
1. Set Spectrum Analyzer into Phase Noise Mode with Cross-Correlation (10 averages).
2. Configure carrier at 2600 MHz CW (+7.5 dBm).
3. Set offset measurement range from 1 kHz to 40 MHz.
4. Record Single-Sideband (SSB) Phase Noise in dBc/Hz across standard frequency offsets.

| Carrier Frequency | 10 kHz Offset | 100 kHz Offset | 1 MHz Offset | 10 MHz Offset | Integrated Jitter (1k-40M) |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **800 MHz** | $-118.5\text{ dBc/Hz}$ | $-126.2\text{ dBc/Hz}$ | $-144.1\text{ dBc/Hz}$ | $-155.8\text{ dBc/Hz}$ | $115\text{ fs rms}$ |
| **1800 MHz** | $-112.0\text{ dBc/Hz}$ | $-120.5\text{ dBc/Hz}$ | $-138.4\text{ dBc/Hz}$ | $-151.2\text{ dBc/Hz}$ | $145\text{ fs rms}$ |
| **2600 MHz** | $-108.2\text{ dBc/Hz}$ | $-116.4\text{ dBc/Hz}$ | $-134.2\text{ dBc/Hz}$ | $-147.5\text{ dBc/Hz}$ | $180\text{ fs rms}$ |
| **3500 MHz** | $-104.5\text{ dBc/Hz}$ | $-112.8\text{ dBc/Hz}$ | $-130.6\text{ dBc/Hz}$ | $-144.0\text{ dBc/Hz}$ | $220\text{ fs rms}$ |
| **5800 MHz** | $-98.8\text{ dBc/Hz}$ | $-107.1\text{ dBc/Hz}$ | $-125.0\text{ dBc/Hz}$ | $-138.5\text{ dBc/Hz}$ | $310\text{ fs rms}$ |

---

### 4.3 Receiver (RX) Noise Figure, Gain & Linearity Measurement Procedure
1. Connect calibrated RF Signal Generator to `RX1_IN` with 50Ω cable.
2. Set RX LO to 2.6 GHz and analog gain to Maximum (Gain Index 255).
3. Inject $-50\text{ dBm}$ tone at 2605 MHz (+5 MHz IF offset).
4. Read digital FFT power in eLynxSDR to verify RX conversion gain.
5. Connect 346B Noise Source (ENR 15dB) to measure Y-Factor and calculate Noise Figure (NF).
6. For IIP3, inject two tones at 2600 MHz and 2601 MHz ($-25\text{ dBm}$ each) and measure 3rd-order intermodulation spurs.

| Carrier Frequency | Max Gain (Index 255) | Noise Figure (NF) | Input P1dB | Input IP3 (IIP3) | Image Rejection |
| :---: | :---: | :---: | :---: | :---: | :---: |
| **800 MHz** | $38.5\text{ dB} \pm 0.5\text{ dB}$ | $11.5\text{ dB}$ | $-13.5\text{ dBm}$ | $+24.5\text{ dBm}$ | $> 78\text{ dBc}$ |
| **1800 MHz** | $37.8\text{ dB} \pm 0.5\text{ dB}$ | $12.0\text{ dB}$ | $-13.0\text{ dBm}$ | $+23.8\text{ dBm}$ | $> 76\text{ dBc}$ |
| **2600 MHz** | $37.0\text{ dB} \pm 0.5\text{ dB}$ | $12.5\text{ dB}$ | $-12.5\text{ dBm}$ | $+22.5\text{ dBm}$ | $> 75\text{ dBc}$ |
| **3500 MHz** | $35.5\text{ dB} \pm 0.6\text{ dB}$ | $13.8\text{ dB}$ | $-11.8\text{ dBm}$ | $+21.2\text{ dBm}$ | $> 72\text{ dBc}$ |
| **5800 MHz** | $32.0\text{ dB} \pm 0.8\text{ dB}$ | $16.5\text{ dB}$ | $-9.5\text{ dBm}$ | $+18.5\text{ dBm}$ | $> 68\text{ dBc}$ |

---

### 4.4 DC Power Consumption & Thermal Profile Across Operating Modes

| Operational Profile | 1.0V Digital (TP1) | 1.3V RF (TP2) | 1.3V Synth (TP3) | 1.8V Clock (TP4) | Total Power Dissipation |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Standby / Idle Mode** | 0.35 A (0.35 W) | 0.08 A (0.10 W) | 0.05 A (0.07 W) | 0.15 A (0.27 W) | **0.85 W** (Heatsink: 32°C) |
| **Profile 100 (122.88 MSPS)** | 1.45 A (1.45 W) | 1.20 A (1.56 W) | 0.22 A (0.29 W) | 0.45 A (0.81 W) | **4.25 W** (Heatsink: 46°C) |
| **Profile 200 (245.76 MSPS)** | 1.85 A (1.85 W) | 1.35 A (1.76 W) | 0.25 A (0.33 W) | 0.52 A (0.94 W) | **5.40 W** (Heatsink: 54°C) |
| **Profile 400 (491.52 MSPS / DPD)** | 2.20 A (2.20 W) | 1.50 A (1.95 W) | 0.28 A (0.36 W) | 0.58 A (1.04 W) | **6.65 W** (Heatsink: 62°C) |

---

## 5. Clock Distribution (HMC7044) & PLL Lock Diagnostics
1. **Query HMC7044 Lock Status via Sysfs:**
   ```bash
   cat /sys/bus/iio/devices/iio:device*/name | grep hmc7044
   iio_attr -d hmc7044 pll1_locked
   iio_attr -d hmc7044 pll2_locked
   ```
2. **DEV_CLK Probing:** Probe differential test points `TP_CLK_P` / `TP_CLK_N` with a differential probe. Nominal: 122.88 MHz LVDS, 700 mVpp.
3. **TCXO Reference Inspection:** Verify the 40.000 MHz Stratum 3 oscillator (SiT5356) output on pin 3. Frequency drift must be within $\pm 0.1\text{ ppm}$.

---

## 6. ADRV9009 BGA IC Rework & Reflow Soldering Profile
The ADRV9009 is housed in a **196-ball CSP_BGA ($12\text{ mm} \times 12\text{ mm}$, 0.8 mm ball pitch)**.
- **PCB Baking:** 8 hours at $125^\circ\text{C}$ prior to rework to prevent moisture-induced delamination (popcorning).
- **Preheat Zone:** Ramp at $+1.5^\circ\text{C/s}$ from $150^\circ\text{C}$ to $170^\circ\text{C}$ (80–100s).
- **Soak / Flux Activation:** $170^\circ\text{C}$ to $200^\circ\text{C}$ for 60–90s.
- **Liquidus Transition:** $217^\circ\text{C}$ (SAC305 lead-free alloy).
- **Peak Reflow:** $235^\circ\text{C}$ to $245^\circ\text{C}$ (Time Above Liquidus: 40–60s).
- **Cooling Rate:** $-2.5^\circ\text{C/s}$ to $-3.0^\circ\text{C/s}$ down to room temperature.

### Figure 1.5: BGA Rework Thermal Profile
![ADRV9009 BGA Rework Thermal Profile](images/adrv9009_bga_rework_thermal_profile.png)
*Figure 1.5: ADRV9009 196-Ball CSP_BGA Rework & Soldering Thermal Profile (IPC-7711/7721 Standard SAC305 Lead-Free Alloy).*

---

## 7. Bill of Materials (BOM) for Critical Spare Parts

| Designator | Part Number | Manufacturer | Package / Footprint | Function |
| :---: | :--- | :--- | :--- | :--- |
| **U1** | `ADRV9009BBCZ` | Analog Devices | 196-Ball CSP_BGA ($12 \times 12\text{ mm}$) | Dual RF Transceiver Core IC |
| **U2** | `HMC7044LP10BE` | Analog Devices | 64-Lead LFCSP ($9 \times 9\text{ mm}$) | Clock Jitter Cleaner & Generator |
| **U3** | `ADP5054ACPZ-R7` | Analog Devices | 48-Lead LFCSP ($7 \times 7\text{ mm}$) | Quad High-Current Buck PMIC |
| **U4, U5**| `LT3045EDD#PBF` | Analog Devices | 10-Lead DFN ($3 \times 3\text{ mm}$) | Ultra-Low Noise ($0.8\,\mu\text{V}_{\text{rms}}$) RF LDO |
| **T1–T4** | `TCM1-63AX+` | Mini-Circuits | 6-SMD ($3.8 \times 3.8\text{ mm}$) | $50\,\Omega : 100\,\Omega$ RF Balun (10–6000 MHz) |
| **D1–D4** | `PESD5V0U1BL` | Nexperia | SOD-882 ($1.0 \times 0.6\text{ mm}$) | 0.5 pF Low-Capacitance RF ESD Protection |
| **Y1** | `SiT5356AI-FP-33E`| SiTime | SMD 5032 ($5.0 \times 3.2\text{ mm}$) | 40.000 MHz Stratum 3 Precision TCXO |

---

## 8. End-to-End Hardware System Troubleshooting Flowchart

### Figure 1.6: Hardware Troubleshooting & Fault Isolation Flowchart
![ADRV9009 Troubleshooting Flowchart](images/adrv9009_hardware_troubleshooting_flowchart.png)
*Figure 1.6: End-to-End ADRV9009 Hardware System Troubleshooting & Fault Isolation Flowchart.*

---

# Part 2: Persian Hardware Maintenance Guide (بخش فارسی)

## ۱. زمان‌بندی نگهداری پیشگیرانه و استانداردهای بازرسی چشمی

| دوره بازرسی | زیرسیستم هدف | اقدام و استاندارد بازرسی | ابزارها و مواد مصرفی |
| :--- | :--- | :--- | :--- |
| **روزانه** | کانکتورهای SMA | بررسی سلامت پین مرکزی و رزوه‌ها؛ سفت‌کردن با گشتاور استاندارد 0.9 N·m. | آچار ترکمتر SMA (5/16 اینچ) |
| **ماهانه** | هیت‌سینک و فن | غبارزدایی پره‌ها؛ کنترل دور فن خنک‌کننده (> 4500 RPM). | دمنده هوای خشک (< 30 psi) |
| **سه ماهه** | خطوط تغذیه DC | اندازه‌گیری ولتاژ نقاط تست TP1-TP6؛ کنترل ریپل به کمتر از 5 mV. | مولتی‌متر دیجیتال، اسیلوسکوپ |
| **شش ماهه** | پد سیلیکونی | بررسی انعطاف‌پذیری پد حرارتی؛ تعویض خمیر سیلیکون فرسوده (PTM7950). | الکل ایزوپروپیل ۹۹.۹٪ (IPA) |
| **سالانه** | اتصالات BGA | بازرسی میکروسکوپی ترک‌های قلع، ویسکرها و تست رطوبت‌زدایی. | میکروسکوپ استریو ۲۰ برابر |

---

## ۲. خروجی‌های آلتیوم Job 1 و Job 2 و جانمایی نقاط تست روی لایه‌های PCB

### شکل ۲.۱: نقشه شماتیک نقاط تست آلتیوم (Job 1)
![Altium Job 1 Schematic](images/altium_job1_schematic_checkpoints.png)
*شکل ۲.۱: خروجی آلتیوم Job 1 — نقشه شماتیک نقاط تست، برچسب‌های نت، مدارات PMIC و رگولاتورهای LDO به همراه راهنمای تفکیک عیب.*

### شکل ۲.۲: نقشه‌های مونتاژ لایه‌های PCB آلتیوم (Job 2)
![Altium Job 2 PCB Assembly](images/altium_job2_pcb_assembly_top_bottom.png)
*شکل ۲.۲: خروجی آلتیوم Job 2 — نقشه‌های مونتاژ لایه‌های PCB: لایه رو (Job 2A) و لایه پشت (Job 2B) شامل ترک‌های فرکانس بالا و پدهای تست ولتاژ.*

---

## ۳. عیب‌یابی خطوط تغذیه DC و جدول نقاط تست ولتاژ

| نقطه تست | نام خط تغذیه | ولتاژ نامی | تلرانس مجاز | حداکثر ریپل | علائم خرابی و ریشه عیب |
| :---: | :--- | :---: | :---: | :---: | :--- |
| **TP1** | هسته دیجیتال (1.0V) | 1.00 V | $\pm 5\%$ | $< 5\text{ mV}$ | قطع لینک JESD204B؛ بررسی مبدل Buck آی‌سی ADP5054. |
| **TP2** | آنالوگ RF (1.3V) | 1.30 V | $\pm 5\%$ | $< 3\text{ mV}$ | نویز فاز شدید و افت گین میکسر؛ بررسی رگولاتور LT3045. |
| **TP3** | سینتی‌سایزر (1.3V) | 1.30 V | $\pm 5\%$ | $< 2\text{ mV}$ | قفل نشدن RFPLL نوسان‌ساز؛ بررسی فیلتر فریت FB3. |
| **TP4** | کلاک (1.8V) | 1.80 V | $\pm 5\%$ | $< 8\text{ mV}$ | جیتر شدید کلاک HMC7044؛ بررسی سلف سوئیچینگ L4. |
| **TP5** | ولتاژ سیستم (3.3V) | 3.30 V | $\pm 5\%$ | $< 15\text{ mV}$| عدم شناسایی پورت سریال و EEPROM؛ بررسی مبدل DCDC. |
| **TP6** | ورودی برد (12.0V) | 12.00 V | $\pm 10\%$ | $< 50\text{ mV}$| خاموشی کامل برد؛ بررسی فیوز ورودی F1 (5A) و دیود TVS. |

---

## ۴. دستورالعمل‌های گام‌به‌گام تست آزمایشگاهی و جدول نتایج اندازه‌گیری

### شکل ۲.۳: دیاگرام اتصالات میز تست و اندازه‌گیری آزمایشگاهی
![RF Test Bench Setup](images/rf_test_bench_setup_diagram.png)
*شکل ۲.۳: دیاگرام اتصالات میز تست و اندازه‌گیری آزمایشگاهی برد ADRV9009 شامل اسپکتروم آنالایزر، سیگنال ژنراتور و همگام‌سازی کلاک ۱۰ مگاهرتز.*

### دستورالعمل تست توان، خلوص طیفی و هارمونیک‌های فرستنده (TX):
1. **اتصال فیزیکی:** اتصال پورت `TX1_OUT` از طریق پد تضعیف‌کننده ۲۰ دسی‌بل به ورودی ۵۰ اهم اسپکتروم آنالایزر.
2. **تنظیم فرکانس:** تنظیم فرکانس LO روی ۲.۶ گیگاهرتز از طریق دستورات لینوکس IIO یا eLynxSDR.
3. **ارسال تن دیجیتال:** فعال‌سازی ارسال تن پیوسته (CW) در تضعیف ۰ دسی‌بل و قرائت توان حامل اصلی ($+7.5\text{ dBm}$).
4. **تایید شاخص‌ها:** تضعیف فرکانس تصویر ($>78\text{ dBc}$)، تضعیف نشت کریر ($<-82\text{ dBc}$) و سطح هارمونیک دوم ($-55\text{ dBc}$).

### دستورالعمل تست رقم نویز (NF)، حساسیت و خطینگی گیرنده (RX):
1. اتصال سیگنال ژنراتور کالیبره‌شده به پورت `RX1_IN` و تزریق سیگنال تک‌تن با توان $-50\text{ dBm}$.
2. قرائت دامنه دیجیتال در پنجره FFT نرم‌افزار جهت محاسبه گین تبدیل آنالوگ به دیجیتال ($37.0\text{ dB}$ در ۲.۶ گیگاهرتز).
3. اتصال منبع نویز کالیبره‌شده (346B) جهت اندازه‌گیری Y-Factor و استخراج رقم نویز (کمتر از $12.5\text{ dB}$).

---

## ۵. دستورالعمل ریورک و تعویض تراشه BGA فرستنده/گیرنده ADRV9009

### شکل ۲.۴: پروفایل حرارتی استاندارد ۴ مرحله‌ای ریورک BGA
![ADRV9009 BGA Rework Thermal Profile](images/adrv9009_bga_rework_thermal_profile.png)
*شکل ۲.۴: پروفایل حرارتی استاندارد ۴ مرحله‌ای ریورک و لحیم‌کاری هوای گرم BGA بر اساس استاندارد IPC-7711/7721 با آلیاژ SAC305.*

---

## ۶. فلوچارت جامع عیب‌یابی و تفکیک خطای سخت‌افزاری برد

### شکل ۲.۵: فلوچارت گام‌به‌گام عیب‌یابی و تعمیرات سخت‌افزاری
![ADRV9009 Troubleshooting Flowchart](images/adrv9009_hardware_troubleshooting_flowchart.png)
*شکل ۲.۵: فلوچارت گام‌به‌گام عیب‌یابی، تست خطوط تغذیه، پایش سلامت کلاک و تفکیک خطاهای سخت‌افزاری برد فرستنده/گیرنده ADRV9009.*
