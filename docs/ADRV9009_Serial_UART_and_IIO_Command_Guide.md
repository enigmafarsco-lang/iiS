# ADRV9009 Serial /dev/ttyUSB0, Linux Terminal & IIO Command Guide
## راهنمای جامع کار با پورت سریال ttyUSB0، ترمینال لینوکس و دستورات خط فرمان IIO در تراشه ADRV9009

---

### Document Overview & Metadata / مشخصات و شناسنامه سند
- **UART Hardware Device:** FTDI / Silicon Labs USB-to-UART Bridge (`/dev/ttyUSB0`, 115200 8-N-1)
- **Embedded Linux OS:** Analog Devices Kuiper Linux / PetaLinux on ARM Cortex-A53 / Cortex-A9
- **IIO Framework:** Linux Industrial I/O (IIO), `libiio` v0.24+, `iio-tools`, `devmem2`
- **Document Version:** v1.2.0 — October 2026

---

# Part 1: English Serial & IIO Command Reference

## 1. Serial Port Connection & Terminal Setup (/dev/ttyUSB0)
The ADRV9009 evaluation board and eLynxSDR hardware platform connect to the host computer through a high-speed USB-to-UART bridge.

### 1.1 Verifying USB Enumeration & Permissions
```bash
# Check USB device detection in Linux kernel logs
$ dmesg | grep -i ttyUSB
[ 1245.890] ftdi_sio 1-1.2:1.0: FTDI USB Serial Device converter detected
[ 1245.891] usb 1-1.2: FTDI USB Serial Device converter now attached to ttyUSB0

# Grant read/write access
$ sudo chmod 666 /dev/ttyUSB0
```

### 1.2 Opening the Terminal Session
```bash
# Recommended: Connect using picocom (115200 baud, 8 data bits, no parity, 1 stop bit)
$ picocom -b 115200 /dev/ttyUSB0

# Alternative: Connect using minicom
$ minicom -D /dev/ttyUSB0 -b 115200 -8 -N

# Alternative: Connect using GNU screen
$ screen /dev/ttyUSB0 115200
```

---

## 2. IIO Device Topology Discovery (iio_info)

```bash
# Local scan inside the board's serial terminal
root@analog:~# iio_info

# Remote scan from host PC across Gigabit Ethernet
$ iio_info -u "ip:192.168.1.10"

# Query specific device (adrv9009-phy)
$ iio_info -u "ip:192.168.1.10" -d adrv9009-phy
```

### Primary IIO Devices Table:
1. `adrv9009-phy`: RF LO synthesizers, analog filters, gain amplifiers, ENSM, and tracking calibrations.
2. `axi-adrv9009-tx-hpc`: FPGA DAC DMA buffer streamer and Direct Digital Synthesis (DDS) CW tone generator.
3. `axi-adrv9009-rx-hpc`: FPGA ADC DMA high-speed sample capture engine.
4. `hmc7044`: Low-jitter clock generator distributing DEV_CLK (122.88 MHz) and SYSREF pulses.
5. `led-count-iio` / `mwipcore0`: Custom DRFM baseband registers at physical address `0x43C30000`.

---

## 3. iio_attr Command Reference — Frequency, Gain & ENSM

### 3.1 RF Carrier Frequency Read & Write
```bash
# 1. Read Current RF Carrier Frequency (Hz)
$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency
2400000000

# 2. Set Carrier Frequency to 5.8 GHz
$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 5800000000

# 3. Read Baseband Sampling Rate (Hz)
$ iio_attr -d adrv9009-phy -c voltage0 -i sampling_frequency
122880000

# 4. Set Analog RF Filter Bandwidth (100 MHz)
$ iio_attr -d adrv9009-phy -c voltage0 -i rf_bandwidth 100000000
```

### 3.2 Hardware Gain & Attenuation Control
```bash
# 1. Read TX1 Digital Attenuation (dB)
$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain
-10.000000 dB

# 2. Set TX1 to Maximum Power (0 dB Attenuation)
$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain 0.0

# 3. Set TX1 to 20 dB Attenuation
$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain -20.0

# 4. Set RX1 Gain to +25 dB in Manual Gain Mode (MGC)
$ iio_attr -d adrv9009-phy -c voltage0 -i gain_control_mode manual
$ iio_attr -d adrv9009-phy -c voltage0 -i hardwaregain 25.0
```

### 3.3 ENSM State Machine & Calibration Controls
```bash
# 1. Query Current ENSM State
$ iio_attr -d adrv9009-phy ensm_mode
radio_on

# 2. Put Transceiver to Sleep or Awake
$ iio_attr -d adrv9009-phy ensm_mode sleep
$ iio_attr -d adrv9009-phy ensm_mode radio_on

# 3. Enable Internal Tracking Calibrations
$ iio_attr -d adrv9009-phy calibrate_rx_qec_en 1
$ iio_attr -d adrv9009-phy calibrate_tx_qec_en 1
$ iio_attr -d adrv9009-phy calibrate_tx_lol_en 1

# 4. Read On-Chip Junction Temperature (in millidegrees C)
$ iio_attr -d adrv9009-phy -c temp0 in_temp0_input
42350
# (42350 m°C = 42.35 °C)
```

---

## 4. Direct Digital Synthesis (DDS) CW Tone Generation

```bash
# Generate 10 MHz CW Tone on TX1 (altvoltage0) at Full Scale (1.0)
$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o frequency 10000000
$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 1.0
$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o phase 0
$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 1

# Mute DDS CW Tone
$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 0
```

---

## 5. Direct Memory Mapped (AXI) Register Control (`0x43C30000` & `devmem2`)

| Register Symbol | Physical Address | Bit Width | Functional Description |
| :--- | :--- | :--- | :--- |
| **REG_AMPLY (TH0)** | `0x43C30100` | 16-bit | Linear DRFM attenuation factor (`out_count1_th0_raw`) |
| **REG_VGPO_ENABLE (PDW)** | `0x43C30110` | 32-bit | Doppler enable gate (`1` = Active, `0` = Bypass) |
| **REG_VGPO_OFFSET (INchann)** | `0x43C30118` | 32-bit | Initial phase offset word for Doppler NCO |
| **REG_VGPO_STEP (THCW)** | `0x43C3011C` | 32-bit signed | Phase step: `thcw = round(fd * 2^32 / Fs)` |
| **REG_DRFM_SELECT (DACseles)**| `0x43C30138` | 32-bit | `1` = DRFM target echo; `0` = DMA / Noise buffer |

```bash
# 1. Switch DAC output to DRFM Target Echo
root@analog:~# devmem2 0x43C30138 w 0x00000001

# 2. Set Doppler Step for +50 kHz Shift at 122.88 MSPS (THCW = 1747627)
root@analog:~# devmem2 0x43C3011C w 0x001AAAEA

# 3. Enable Doppler Modulation
root@analog:~# devmem2 0x43C30110 w 0x00000001

# 4. Fast Sysfs Direct Write (Pure IIO)
root@analog:~# echo 1 > /sys/bus/iio/devices/iio:device*/out_count15_dacseles_raw
```

---

# Part 2: Persian Translation & Operator Guide (بخش فارسی)

## ۱. ساختار ارتباط سریال و لاگین به ترمینال برد (ttyUSB0)
برد ترنسور ADRV9009 از طریق پورت سریال مجازی `/dev/ttyUSB0` امکان دسترسی مستقیم به ترمینال خط فرمان لینوکس (PetaLinux/Kuiper Linux) را فراهم می‌سازد.

### تنظیمات ارتباط سریال:
- **نرخ تبادل داده (Baud Rate):** ۱۱۵۲۰۰ بیت بر ثانیه
- **فرمت داده:** 8 Data bits, No Parity, 1 Stop bit (8-N-1)
- **دستور اتصال سریع در لینوکس:**
  ```bash
  picocom -b 115200 /dev/ttyUSB0
  ```

---

## ۲. دستورات کلیدی IIO جهت تنظیم فرکانس، گین و کالیبراسیون

### ۲.۱ خواندن و نوشتن فرکانس نوسان‌ساز محلی (RF LO):
```bash
# خواندن فرکانس فعلی فرستنده و گیرنده:
iio_attr -d adrv9009-phy -c altvoltage0 -o frequency

# تغییر فرکانس کریر به ۲۴۰۰ مگاهرتز (۲.۴ گیگاهرتز):
iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 2400000000

# تغییر فرکانس کریر به ۵۸۰۰ مگاهرتز (۵.۸ گیگاهرتز):
iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 5800000000
```

### ۲.۲ کنترل توان و تضعیف دیجیتال فرستنده:
```bash
# خواندن تضعیف فعلی فرستنده:
iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain

# تنظیم تضعیف روی ۰ دسی‌بل (حداکثر توان خروجی):
iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain 0.0

# اعمال ۱۵ دسی‌بل تضعیف بر روی سیگنال فرستنده:
iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain -15.0
```

### ۲.۳ تولید تن تک‌فرکانس سینوسی با DDS سخت‌افزاری:
```bash
# فعال‌سازی تن سینوسی ۱۰ مگاهرتز روی کانال TX1:
iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o frequency 10000000
iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 1.0
iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 1

# قطع سیگنال تن سینوسی:
iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 0
```

### ۲.۴ کنترل ثبات‌های سخت‌افزاری DRFM در آدرس `0x43C30000`:
```bash
# اتصال خروجی DAC به مسیر شبیه‌ساز اکوی رادار DRFM:
devmem2 0x43C30138 w 0x00000001

# تنظیم شیفت فرکانسی داپلر (+50 kHz):
devmem2 0x43C3011C w 0x001AAAEA

# فعال‌سازی مدولاسیون داپلر:
devmem2 0x43C30110 w 0x00000001
```
