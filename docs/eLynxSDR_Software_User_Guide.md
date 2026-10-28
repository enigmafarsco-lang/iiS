# eLynxSDR Software Platform
## Comprehensive User & Technical Reference Guide

**Software Version:** v0.0.2-Release (eLynxSDR IIO / Linux Edition)  
**Transceiver Architecture:** Analog Devices ADRV9009 / Zynq UltraScale+ MPSoC RFSoC  
**Document Reference:** eLynx-UG-2026-REV3  
**Classification:** Technical & Operational Manual  

---

## Table of Contents

1. [System Overview & Architecture](#1-system-overview--architecture)
   - 1.1 Hardware Architecture & Transceiver IC
   - 1.2 Software Stack & Multi-Threaded Engine
   - 1.3 Subsystem Interconnection Matrix
2. [System Startup & Connection Setup](#2-system-startup--connection-setup)
   - 2.1 Network Discovery & IIO Context Initialisation
   - 2.2 Transceiver Profile Selection & Multi-Profile Switching
3. [Receiver & Spectrum Analysis Subsystem](#3-receiver--spectrum-analysis-subsystem)
   - 3.1 Main Layout & Real-Time FFT Spectrum Analyzer / Waterfall
   - 3.2 Global Controls, ENSM States & Calibration Triggers
   - 3.3 Profile Parameters & Digital Filter Engine
   - 3.4 Spectrum Display DSP Settings & Windowing Functions
   - 3.5 Transmitter (TX) Chain Configuration & Digital Attenuation
   - 3.6 Observation Receiver (OBSRX) Chain Configuration
   - 3.7 RF Switch Matrix & Signal Routing Architecture
   - 3.8 Frequency Calibration & Phase/Gain Correction Curves
4. [Digital Radio Frequency Memory (DRFM) Subsystem](#4-digital-radio-frequency-memory-drfm-subsystem)
   - 4.1 DRFM Mathematical Principles & Doppler Formulations
   - 4.2 Hardware AXI Register Mapping & Bit Allocations
   - 4.3 Range Delay Simulation & Baseline Latency Calibration
   - 4.4 Independent DRFM Modulation & Seamless Exciter Playback
5. [Multi-Waveform Exciter Subsystem](#5-multi-waveform-exciter-subsystem)
   - 5.1 Continuous Wave (CW) Single-Tone Generator
   - 5.2 Spot Noise / Electronic Jamming Generator
   - 5.3 Linear Frequency Sweep Generator
   - 5.4 Pulse & Impulse Waveform Generator
   - 5.5 Wideband Noise & Comb Bridge Noise Generators
   - 5.6 Multi-Target Radar Echo Generator (5 Concurrent Targets)
   - 5.7 Linear Frequency Modulated (LFM Chirp) Generator
6. [Low-Level Transceiver Configuration & Control Unit](#6-low-level-transceiver-configuration--control-unit)
   - 6.1 ADRV9009 Global Register & Sensor Controls
   - 6.2 Multi-Chip Phase Synchronization (MCS) Workflow
7. [Operational Procedures & Best Practices](#7-operational-procedures--best-practices)
   - 7.1 Complete System Startup & Initialisation Sequence
   - 7.2 Radar Target Echo Generation Workflow
   - 7.3 Exciter Waveform Transmission & Attenuation Control
8. [Troubleshooting, Diagnostics & Technical Specifications](#8-troubleshooting-diagnostics--technical-specifications)
   - 8.1 Common Diagnostics & Troubleshooting Matrix
   - 8.2 Comprehensive System Technical Specifications

---

## 1. System Overview & Architecture

The **eLynxSDR** platform is an advanced, high-performance Software Defined Radio (SDR) and Digital Radio Frequency Memory (DRFM) instrumentation workstation designed for modern radar testing, electronic warfare simulation, spectrum monitoring, and wideband RF synthesis. The software provides an integrated graphical workstation interface for multi-channel RF transceivers (specifically Analog Devices ADRV9009 / AD9371 families), high-speed FPGA baseband processing engines, multi-waveform exciter signal generation, and real-time spectrum/spectrogram visualization.

### 1.1 Hardware Architecture & Transceiver IC

The underlying hardware platform couples a high-density Xilinx Zynq UltraScale+ MPSoC / Kintex FPGA with the Analog Devices ADRV9009 dual-channel RF transceiver. The ADRV9009 operates over a frequency range of 75 MHz to 6 GHz, offering instantaneous receiver bandwidths up to 200 MHz and transmitter / observation receiver bandwidths up to 450 MHz. Digital sample exchange is conducted over multi-lane JESD204B high-speed serial links connected to high-performance DMA cores inside the FPGA.

| Subsystem Component | Hardware / Implementation | Operational Specifications | Interface / Bus |
| :--- | :--- | :--- | :--- |
| **RF Transceiver** | ADRV9009 2T2R2ORX | 75 MHz – 6.0 GHz, BW up to 450 MHz | SPI / JESD204B |
| **FPGA Processing Engine** | Zynq UltraScale+ / Kintex-7 | Real-time FFT, DRFM pipeline, NCO | AXI4-Lite / AXI-Stream |
| **DAC DMA Engine** | High-Speed Cyclic DMA | Continuous multi-waveform excitation | AXI DMA / JESD204B TX |
| **Observation Receiver** | Dual ORX (ORX1 / ORX2) | High dynamic range sniffer / DPD loop | JESD204B RX / IIO |
| **Host Communication** | Gigabit Ethernet (TCP/IP) | IIO Network Daemon, AXI Register bridge | libiio TCP / Socket LAN |
| **RF Front-End Switches** | Solid-state PIN / SPDT | Sub-microsecond path commutation | FPGA GPIO / AXI IO |

### 1.2 Software Stack & Multi-Threaded Engine

The eLynxSDR application is built upon modern C++ and Qt/Qwt graphics subsystems, structured as a modular multi-threaded pipeline:
- **Communications Thread (`Communications/`):** Manages high-speed asynchronous TCP/IP socket connections, AXI register read/write commands, and hardware state telemetry.
- **Data Ingestion & IIO Thread (`receiver/`):** Directly streams multi-channel IQ sample buffers from the Linux Industrial I/O (IIO) kernel driver via `libiio`.
- **DSP Calculation Thread (`receiver/calculateplotdata.cpp`, `plotfunction.cpp`):** Executes multi-point Fast Fourier Transforms (FFT), windowing (Hann, Hamming, Blackman-Harris, Flat-Top), peak detection, averaging, and persistence calculations.
- **OpenGL / Qwt Visualization Engine (`components/GLChart/`, `receiver/plot.cpp`):** Renders real-time spectrum traces, 2D spectrogram waterfalls, 3D waterfall surfaces, and multi-target radar parameter panels at up to 60 FPS.
- **Exciter Waveform Generator (`devices/exciter/`):** Computes complex double-precision analytical IQ waveforms (LFM chirp, Spot noise, CW, Multi-Target, Impulse) and uploads them seamlessly into cyclic DAC DMA memory buffers.

---

## 2. System Startup & Connection Setup

### 2.1 Network Discovery & Connection Dialog

Upon launching eLynxSDR, the Connection Dialog prompts the operator to specify the target SDR hardware IP address and communication port. The software connects to the embedded IIO daemon (`iiod`) running on the SDR hardware over a dedicated Gigabit Ethernet network.

![Figure 2.1: eLynxSDR Hardware Connection Configuration Dialog](../pictures/Screenshot%20from%202026-10-28%2016-45-30.png)
*Figure 2.1: eLynxSDR Hardware Connection Configuration Dialog*

| Control Element | Parameter / Type | Default Value | Description & Operational Usage |
| :--- | :--- | :--- | :--- |
| **IP Address Input** | IPv4 String | `192.168.1.10` | Static or DHCP IP address assigned to the SDR hardware platform. |
| **Port / Protocol** | Integer / TCP | `30718` / `50000` | TCP control channel for register read/write and command telemetry. |
| **Scan Network Button** | Action Trigger | Auto-Discovery | Scans local subnet for active eLynxSDR hardware nodes. |
| **Connect / Cancel** | Button Controls | N/A | Initialises libiio context, establishes socket link, and launches main workspace. |

> **[ETHERNET CONFIGURATION NOTE]**  
> Ensure the host PC network interface is configured with a static IPv4 address on the same subnet (e.g., `192.168.1.100`, Subnet Mask `255.255.255.0`) before initiating connection.

### 2.2 Transceiver Profile Selection & Multi-Profile Switching

The ADRV9009 transceiver operates according to calibrated digital clock and filter profiles loaded directly into the transceiver's internal ARM core. The Profile Selection Dialog enables the user to select from pre-calibrated profiles matching specific operational bandwidth and sample rate requirements.

![Figure 2.2: ADRV9009 Transceiver Profile Selection and Configuration Dialog](../pictures/Screenshot%20from%202026-10-28%2016-46-00.png)
*Figure 2.2: ADRV9009 Transceiver Profile Selection and Configuration Dialog*

| Profile Identifier | Sample Rate (MSPS) | RX Bandwidth | TX / ORX Bandwidth | Application Domain |
| :--- | :--- | :--- | :--- | :--- |
| **Profile 100** | `122.88 MSPS` | `100.00 MHz` | `200.00 MHz` | Narrowband / standard surveillance, high dynamic range. |
| **Profile 200** | `245.76 MSPS` | `200.00 MHz` | `450.00 MHz` | Wideband radar monitoring and medium-rate DRFM emulation. |
| **Profile 400** | `491.52 MSPS` | `200.00 MHz` | `450.00 MHz` | Ultra-wideband pulse processing, high-fidelity chirp DRFM. |

> **[PROFILE CALIBRATION BEHAVIOR TIP]**  
> In Profile 100 (122.88 MSPS) and Profile 200 (245.76 MSPS), the software automatically maintains an uncalibrated raw noise floor baseline on ORx spectrum displays to preserve real-world noise characterization. In Profile 400 (491.52 MSPS), dynamic multi-point calibration curves are applied across the entire observation passband.

---

## 3. Receiver & Spectrum Analysis Subsystem

### 3.1 Main Layout & Real-Time FFT Spectrum Analyzer / Waterfall

The primary operational interface comprises an upper interactive spectrum analyzer and a lower scrolling waterfall (spectrogram) display. The display engine supports full resizability and maximization, adapting smoothly to full HD, 2K, and 4K workstation monitors.

![Figure 3.1: eLynxSDR Primary Spectrum Display and Global Configuration Tab](../pictures/Screenshot%20from%202026-10-28%2016-46-10.png)
*Figure 3.1: eLynxSDR Primary Spectrum Display and Global Configuration Tab*

**Top Toolbar Features:**
- **Time Domain View:** Switches display to oscilloscope-style raw I(t) and Q(t) time-series voltage waveforms.
- **CW Table:** Displays detected continuous-wave carrier frequencies, signal-to-noise ratios (SNR), and peak amplitudes.
- **Markers:** Places interactive frequency/amplitude markers with delta readouts.
- **Waterfall / Waterfall 3D:** Toggles between 2D continuous spectrogram roll and 3D surface mesh waterfall rendering.
- **Zoom In / Zoom Out / Autoscale:** Dynamically rescales frequency span and power axis limits (-140 dBm to +10 dBm).
- **Calibration Triggers:** Initiates real-time baseline, gain, and quadrature calibration routines.

### 3.2 Global Controls, ENSM States & Calibration Triggers

The Global tab configures primary RF transceiver operating parameters:
- **Active ENSM:** Indicates and sets Enable State Machine mode (`radio_on`, `radio_off`, `sleep`, `alert`).
- **TRX Local Oscillator (LO):** Sets the RF center frequency from 75.0 MHz to 6000.0 MHz with 1 Hz tuning resolution.
- **Calibration Triggers:** Initiates Internal RF LO calibration, ADC/DAC DC offset tracking, and internal tracking loops.
- **Sensor Telemetry:** Monitors real-time on-chip junction temperature sensors (`ADRV9009 PHY Temp`).

### 3.3 Profile Parameters & Digital Filter Engine

The Profile tab presents detailed decimation, interpolation, and digital FIR filter characteristics configured for the active ADRV9009 profile.

![Figure 3.2: Transceiver Profile and Digital FIR Filter Configuration Tab](../pictures/Screenshot%20from%202026-10-28%2016-46-16.png)
*Figure 3.2: Transceiver Profile and Digital FIR Filter Configuration Tab*

### 3.4 Spectrum Analysis DSP Settings & Windowing

The Spectrum tab configures the mathematical Fast Fourier Transform (FFT) engine powering both the 2D spectrum plot and waterfall spectrogram.

![Figure 3.3: Spectrum Analyzer FFT DSP Settings, Windowing Functions, and Averaging](../pictures/Screenshot%20from%202026-10-28%2016-46-22.png)
*Figure 3.3: Spectrum Analyzer FFT DSP Settings, Windowing Functions, and Averaging*

| Parameter | Available Options | Recommended Setting | Mathematical / Operational Impact |
| :--- | :--- | :--- | :--- |
| **FFT Size** | 1024, 2048, 4096, 8192, 16384, 32768, 65536 | `4096` / `8192` | Determines bin frequency resolution: $\Delta f = F_s / N_{FFT}$. Higher sizes increase resolution at slight compute cost. |
| **Window Function** | Rectangular, Hann, Hamming, Blackman-Harris, Flat-Top | `Blackman-Harris` | Suppresses spectral leakage sidelobes (>92 dB attenuation for Blackman-Harris). Flat-Top provides exact amplitude calibration. |
| **Average Count** | 1 (Off) to 128 | `8` – `16` | Performs exponential or linear power averaging across FFT frames to reduce thermal noise variance. |
| **Peak Hold** | Disabled / Enabled | `Enabled` (Signals) | Retains maximum envelope power per frequency bin for transient pulse detection. |
| **Persistence** | 0% to 100% | `30%` – `50%` | Decays prior FFT traces smoothly for phosphor-like visual signal persistence. |

### 3.5 Transmitter (TX) Chain Configuration & Digital Attenuation

The TX tab configures transmitter channels (TX1 and TX2), digital attenuation, baseband gain scaling, and RF Power Amplifier (PA) protection circuits.

![Figure 3.4: Transmitter (TX) Channel Controls, Digital Attenuation, and Power Settings](../pictures/Screenshot%20from%202026-10-28%2016-46-26.png)
*Figure 3.4: Transmitter (TX) Channel Controls, Digital Attenuation, and Power Settings*

### 3.6 Observation Receiver (OBSRX) Chain Configuration

The Observation Receiver (OBSRX) subsystem supports dual observation channels (ORX1 and ORX2) dedicated to Digital Pre-Distortion (DPD) feedback, wideband RF sniffer monitoring, and internal RF loopback characterization.

![Figure 3.5: Observation Receiver (OBSRX) Gain Modes, Tracking Loops, and Calibration](../pictures/Screenshot%20from%202026-10-28%2016-46-31.png)
*Figure 3.5: Observation Receiver (OBSRX) Gain Modes, Tracking Loops, and Calibration*

### 3.7 RF Switch Matrix & Signal Routing Architecture

The Switch tab controls the hardware RF switch matrix, directing signals between external antenna ports, internal calibration loops, and receiver channels.

![Figure 3.6: RF Switch Matrix and Signal Commutation Control Tab](../pictures/Screenshot%20from%202026-10-28%2016-46-36.png)
*Figure 3.6: RF Switch Matrix and Signal Commutation Control Tab*

### 3.8 Frequency Calibration & Phase/Gain Correction Curves

The Calibration tab provides comprehensive system-level calibration routines, storing frequency-dependent gain correction offsets, phase quadrature balance values, and noise floor equalization tables across the entire 75 MHz to 6 GHz RF range.

![Figure 3.7: Frequency Calibration Management and Gain/Phase Correction Tables](../pictures/Screenshot%20from%202026-10-28%2016-46-43.png)
*Figure 3.7: Frequency Calibration Management and Gain/Phase Correction Tables*

---

## 4. Digital Radio Frequency Memory (DRFM) Subsystem

The DRFM subsystem implements real-time radar target simulation, range delay generation, Doppler frequency shifts, and echo amplitude modulation. The DRFM engine operates entirely inside high-speed FPGA fabric with sub-nanosecond pipelining, enabling hardware-in-the-loop (HIL) radar testing.

![Figure 4.1: Digital Radio Frequency Memory (DRFM) Target Parameter Configuration Interface](../pictures/Screenshot%20from%202026-10-28%2016-46-48.png)
*Figure 4.1: Digital Radio Frequency Memory (DRFM) Target Parameter Configuration Interface*

### 4.1 DRFM Mathematical Principles & Doppler Formulations

The DRFM engine precisely simulates physical radar target parameters according to fundamental electromagnetic wave propagation principles:

#### 1. Range Delay Formula
The round-trip propagation time delay $\tau$ for a simulated target at range $R$ is given by:
$$\tau = \frac{2 \cdot R}{c}$$
where $c \approx 299,792,458\text{ m/s}$ is the speed of light in vacuum.
- **Minimum Range:** $R_{\min} = 150\text{ m} \implies \tau_{\min} = 1.00069\ \mu\text{s}$
- **Maximum Range:** $R_{\max} = 25,000\text{ m} (25\text{ km}) \implies \tau_{\max} = 166.782\ \mu\text{s}$
- **Range Resolution Step:** $\Delta R = 10\text{ m} \implies \Delta \tau = 66.713\text{ ns}$
- **Hardware Baseline Delay:** An internal hardware transit latency of approximately 80 ns is compensated:
$$\tau_{\text{total}} = \tau + \tau_{\text{baseline}} = \frac{2 \cdot R}{c} + 80\text{ ns}$$

#### 2. Doppler Frequency Shift Formula
A target moving with radial velocity $v$ illuminated by radar carrier frequency $f_0$ experiences Doppler frequency shift $f_d$:
$$f_d = \frac{2 \cdot v \cdot f_0}{c} = \frac{2 \cdot v}{\lambda}$$
where $\lambda = c / f_0$ is the RF wavelength.
- **Velocity Range:** $-1000\text{ m/s}$ to $+1000\text{ m/s}$
- **Carrier Range:** Up to $6.0\text{ GHz}$ (e.g., at $f_0 = 3\text{ GHz}$ and $v = 150\text{ m/s}$, $f_d = 3000\text{ Hz}$)
- **Doppler Frequency Resolution:** $10\text{ Hz}$

#### 3. FPGA Numerically Controlled Oscillator (NCO) Phase Step ($\text{thcw}$)
The Doppler frequency shift is synthesized via a 32-bit FPGA NCO phase accumulator running at sampling clock $F_s = 122.88\text{ MHz}$:
$$\text{thcw} = \text{round}\left( f_d \cdot \frac{2^{32}}{F_s} \right)$$
For example, for $f_d = 1000\text{ Hz}$ at $F_s = 122.88\text{ MHz}$, $\text{thcw} = 34953$.

### 4.2 Hardware AXI Register Mapping & Bit Allocations

| Register Name | AXI Base / Offset | Bit Fields | Data Type | Function & Engineering Description |
| :--- | :--- | :--- | :--- | :--- |
| `REG_DRFM_CTRL` | `0x43C00000 / 0x00` | `[0] DRFM_EN`<br>`[1] DAC_SEL` | `uint32_t` | Bit 0: Master DRFM enable. Bit 1: DAC source multiplexer (0=DMA, 1=DRFM). |
| `REG_DRFM_RANGE` | `0x43C00000 / 0x04` | `[31:0] DELAY_CYCLES` | `uint32_t` | Target range delay in clock cycles: $N = \text{round}(\tau \cdot F_s)$. |
| `REG_DRFM_THCW` | `0x43C00000 / 0x08` | `[31:0] NCO_PHASE_INC` | `uint32_t` | 32-bit phase increment word $\text{thcw}$ for Doppler frequency generation. |
| `REG_DRFM_AMPL` | `0x43C00000 / 0x0C` | `[15:0] TH0_GAIN` | `uint16_t` | Target echo digital scale factor / attenuation word (TH0). |
| `REG_DRFM_STATUS` | `0x43C00000 / 0x10` | `[0] FIFO_RDY`<br>`[1] SYNC_OK` | `uint32_t` | Real-time DRFM memory buffer status and phase synchronization locks. |

> **[NON-BLOCKING DRFM OPERATION NOTE]**  
> On startup with DRFM enabled, the software checks DAC DMA buffer state; if uninitialized, it uploads a minimal 4-sample dummy zero sequence ('0 0 0 0') once to initialize the DMA clock without overwriting active exciter files. Changing DRFM parameters updates AXI registers directly without restarting or interrupting the active exciter signal.

---

## 5. Multi-Waveform Exciter Subsystem

The Exciter Subsystem provides a versatile software-synthesized signal generation engine for radar target simulation, electronic countermeasure (ECM) testing, and wideband RF channel sounding. Waveforms are synthesized with double-precision floating-point accuracy, scaled to 16-bit signed integer DAC words, and uploaded directly to cyclic FPGA DMA memory.

### 5.1 Continuous Wave (CW) Single-Tone Generator

The CW generator produces single-frequency continuous wave tones with selectable center frequency offset and output attenuation. Signal routing allows independent assignment to DAC Voltage 0/1 (TX1) and Voltage 2/3 (TX2).

![Figure 5.1: Continuous Wave (CW) Single-Tone Exciter Generator Tab](../pictures/Screenshot%20from%202026-10-28%2016-46-58.png)
*Figure 5.1: Continuous Wave (CW) Single-Tone Exciter Generator Tab*

### 5.2 Spot Noise / Electronic Jamming Generator

The Spot Noise generator synthesizes narrowband gaussian noise centered at a tunable frequency offset with user-defined bandwidth (1 MHz to 50 MHz), ideal for spot jamming and interference resilience verification.

![Figure 5.2: Spot Noise / Targeted Interference Exciter Tab](../pictures/Screenshot%20from%202026-10-28%2016-47-02.png)
*Figure 5.2: Spot Noise / Targeted Interference Exciter Tab*

### 5.3 Linear Frequency Sweep Generator

The Sweep generator creates frequency sweeps over a user-defined frequency span (Start MHz to Stop MHz) with adjustable sweep duration (ms) and sweep modes (Linear Continuous or Discrete Stepped).

![Figure 5.3: Frequency Sweep Generator Tab (Linear / Stepped)](../pictures/Screenshot%20from%202026-10-28%2016-47-06.png)
*Figure 5.3: Frequency Sweep Generator Tab (Linear / Stepped)*

### 5.4 Pulse & Impulse Waveform Generator

The Impulse generator synthesizes pulsed RF signals with high-precision pulse width ($T_p$) in microseconds, Pulse Repetition Interval (PRI), and selectable amplitude envelope shaping (Rectangular, Gaussian, Hamming).

![Figure 5.4: Impulse / Pulsed Waveform Exciter Generator Tab](../pictures/Screenshot%20from%202026-10-28%2016-47-10.png)
*Figure 5.4: Impulse / Pulsed Waveform Exciter Generator Tab*

### 5.5 Wideband Noise & Comb Bridge Noise Generators

- **Wideband Noise:** Generates uniform pseudo-random Gaussian noise across bandwidths up to 200 MHz with precise output power attenuation control (dBm).
- **Bridge Noise:** Generates multi-carrier comb noise with selectable inter-carrier spacing and bandwidth for multi-channel receiver testing.

![Figure 5.5: WideBand Noise Generator Tab with Attenuation Control](../pictures/Screenshot%20from%202026-10-28%2016-47-14.png)
*Figure 5.5: WideBand Noise Generator Tab with Attenuation Control*

![Figure 5.6: Bridge (Comb) Noise Generator Tab](../pictures/Screenshot%20from%202026-10-28%2016-47-18.png)
*Figure 5.6: Bridge (Comb) Noise Generator Tab*

### 5.6 Multi-Target Radar Echo Generator

The Multi-Target Exciter enables simultaneous synthesis of up to five independent radar targets. Each target channel features independent waveform selection (Spot, CW, LFM), individual bandwidth (MHz), and Doppler frequency shift (MHz).

![Figure 5.7: Multi-Target Radar Echo Synthesis Generator Tab (5 Concurrent Targets)](../pictures/Screenshot%20from%202026-10-28%2016-47-24.png)
*Figure 5.7: Multi-Target Radar Echo Synthesis Generator Tab (5 Concurrent Targets)*

| Target Channel | Waveform Type | Bandwidth Range | Doppler Shift Range | Operational Application |
| :--- | :--- | :--- | :--- | :--- |
| **Target 1** | Spot / CW / LFM | 1.0 – 50.0 MHz | -50.0 to +50.0 MHz | Primary skin return / leading target |
| **Target 2** | Spot / CW / LFM | 1.0 – 50.0 MHz | -50.0 to +50.0 MHz | Formation wingman / secondary target |
| **Target 3** | Spot / CW / LFM | 1.0 – 50.0 MHz | -50.0 to +50.0 MHz | Chaff cloud / clutter centroid |
| **Target 4** | Spot / CW / LFM | 1.0 – 50.0 MHz | -50.0 to +50.0 MHz | Coordinated deceptive decoy |
| **Target 5** | Spot / CW / LFM | 1.0 – 50.0 MHz | -50.0 to +50.0 MHz | High-speed projectile / missile threat |

### 5.7 Linear Frequency Modulated (LFM Chirp) Generator

The LFM generator creates Linear Frequency Modulated (Chirp) radar pulses according to user parameters: Start Frequency ($f_{\text{start}}$ in MHz), Chirp Bandwidth ($B$ in MHz), and Pulse Duration ($T$ in microseconds).

The complex baseband chirp waveform is synthesized as:
$$s(t) = \exp\left( j 2\pi \left( f_{\text{start}} t + \frac{B}{2 T} t^2 \right) \right), \quad 0 \le t \le T$$

![Figure 5.8: Linear Frequency Modulated (LFM Chirp) Generator Tab](../pictures/Screenshot%20from%202026-10-28%2016-47-29.png)
*Figure 5.8: Linear Frequency Modulated (LFM Chirp) Generator Tab*

---

## 6. Low-Level Transceiver Configuration & Control Unit

The Configs menu hosts low-level hardware diagnostics, internal ADRV9009 register maps, temperature monitoring, and Multi-Chip Synchronization (MCS) controls.

![Figure 6.1: ADRV9009 Hardware Controls, Sensor Readings, and Chain Synchronization](../pictures/Screenshot%20from%202026-10-28%2016-47-35.png)
*Figure 6.1: ADRV9009 Hardware Controls, Sensor Readings, and Chain Synchronization*

| Functional Block | Diagnostic / Control Registers | Monitoring Value | Operational Function |
| :--- | :--- | :--- | :--- |
| **Global Settings** | ENSM Modes, Profile Path | `radio_on` | Transceiver master operational state. |
| **TRX Local Oscillator** | PLL Synthesizer registers | `1800.000 MHz` | Sets RF LO mixing frequency. |
| **Thermal Sensors** | ADRV9009 PHY Temp, AD7291 Temp | `22.00 °C` / Active | Monitors junction temperatures to prevent thermal shutdown. |
| **Frequency Hopping** | Hopping Mode, Min/Max/Step/Delay | Disabled / Fast Hop | Configures fast RF hopping tables across the operating band. |
| **RX Gain & Tracking** | Hardware Gain, RSSI, Quadrature/HD2 | `30.00 dB` / Active | Manual/AGC gain control and digital tracking loop enable. |
| **TX Attenuation & PA** | PA Protection, Digital Attenuation | `0 dB Atten` / Safe | Amplifier protection interlocking and output power leveling. |
| **MCS Sync Trigger** | SYSREF / Multi-Chip Sync | Sync Locked | Synchronizes internal clock dividers across multiple transceiver ICs. |

---

## 7. Operational Procedures & Best Practices

### 7.1 Complete System Startup & Initialisation Sequence

1. Power on the SDR platform hardware and wait for network initialization (~30 seconds).
2. Launch the `eLynxSDR` executable on the host workstation.
3. In the Connection Dialog (Fig 2.1), enter the target IP (`192.168.1.10`) and click **Connect**.
4. In the Profile Selection Dialog (Fig 2.2), choose the appropriate operational profile (**Profile 100** for 122.88 MSPS, **Profile 200** for 245.76 MSPS, or **Profile 400** for 491.52 MSPS) and click **OK**.
5. Observe the Global tab (Fig 3.1): verify that `Active ENSM` indicates `radio_on` and PHY Temperature is within normal operating limits (<65 °C).
6. Set the desired RF Center Frequency (e.g., `1800.0 MHz`) in the TRX Local Oscillator field and press Enter.
7. Configure the Spectrum Analyzer parameters (Fig 3.3): select FFT Size (`4096`), Window (`Blackman-Harris`), and Average Count (`8`).
8. Verify that clean spectrum and waterfall traces are actively streaming in the main window.

### 7.2 Radar Target Echo Generation & DRFM Emulation Workflow

1. Open the **DRFM Tab** (Fig 4.1) from the sidebar.
2. Set the desired **Target Distance** (Range in meters, e.g. `5,000 m`). *Note: Valid range is 150 m to 25,000 m with 10 m increments.*
3. Set the **Target Velocity** (m/s) or directly input the **Doppler Frequency Shift** ($f_d$ in Hz, up to 6 GHz RF carrier with 10 Hz resolution).
4. Adjust **Target Echo Attenuation (TH0)** to match the desired Radar Cross Section (RCS) return amplitude.
5. Toggle the **DRFM Enable** switch to ON. Observe that active exciter transmissions remain uninterrupted while the hardware DRFM registers update instantly.

### 7.3 Exciter Waveform Transmission & Attenuation Control

1. Switch to the **Exciter** tab in the top navigation bar.
2. Select the desired waveform generator tab (CW, Spot, Sweep, Impulse, WideBand, Bridge Noise, Multi Target, or LFM).
3. Configure waveform parameters (e.g., for LFM: Start=`0.0 MHz`, BW=`10.0 MHz`, T=`10.0 us`).
4. Select target DAC TX Channels (**Voltage 0/1** for TX1, **Voltage 2/3** for TX2).
5. Click **Set <Waveform>** or **Generate_Send**. The status bar will indicate successful DMA buffer transfer and continuous transmission.

---

## 8. Troubleshooting, Diagnostics & Technical Specifications

### 8.1 Common Diagnostics & Troubleshooting Matrix

| Symptom / Error | Probable Root Cause | Recommended Diagnostic Action |
| :--- | :--- | :--- |
| **Connection Failed / Timeout** | Network IP mismatch, firewall blocking port 30718/50000, or iiod service down. | Verify host IP is on 192.168.1.x subnet; ping `192.168.1.10`; restart iiod daemon on target hardware. |
| **Spectrum Display Flat / No Noise Floor** | ENSM state in `radio_off` or `alert`, or RF LO unlocked. | Navigate to Configs -> ADRV9009, click `Reload Settings`, verify LO frequency within 75–6000 MHz range. |
| **Spectrum Frequency Span Distorted** | Transform sampling rate ($F_s$) unassigned in FFT structure. | Resolved in v0.0.2: verify active profile matches hardware clocking (122.88 / 245.76 / 491.52 MSPS). |
| **Exciter Signal Drops When Modifying DRFM** | DAC DMA cyclic buffer re-triggered on register write. | Resolved in v0.0.2: DRFM writes directly to AXI registers without triggering DAC file reloads. |
| **High PHY Temperature Alarm (>75 °C)** | Insufficient cooling fan airflow or heavy continuous TX at high power. | Check cooling fan status in devices menu; reduce TX digital attenuation (increase attenuation dB). |

### 8.2 Comprehensive System Technical Specifications

| System Parameter | Specification / Rating | Engineering Remarks |
| :--- | :--- | :--- |
| **RF Tuning Range** | 75.0 MHz to 6000.0 MHz (6.0 GHz) | Continuous tuning with 1 Hz LO synthesizer resolution |
| **Instantaneous RX Bandwidth** | Up to 200.0 MHz | Supports Profile 100 (100 MHz) and Profile 200/400 (200 MHz) |
| **Instantaneous TX/ORX Bandwidth** | Up to 450.0 MHz | Wideband synthesised chirp and observation feedback |
| **ADC / DAC Resolution** | 16-bit Complex IQ (I and Q channels) | High dynamic range (>80 dB SFDR) |
| **DRFM Range Delay Coverage** | 150 meters to 25,000 meters (25 km) | 10-meter range step; $\tau = 1.0\ \mu\text{s}$ to $166.7\ \mu\text{s}$ |
| **DRFM Baseline Hardware Latency** | ~80 nanoseconds | Automatically compensated in hardware range delay registers |
| **DRFM Doppler Frequency Tuning** | Carrier up to 6.0 GHz, 10 Hz resolution | 32-bit FPGA NCO phase accumulator ($\text{thcw}$ word) |
| **Exciter Waveform Types** | CW, Spot Noise, Sweep, Impulse, WideBand, Bridge Noise, Multi Target, LFM, NLFM | Arbitrary IQ synthesis with cyclic DMA burst playback |
| **Simultaneous Target Simulation** | Up to 5 concurrent independent targets | Individual waveform, bandwidth, and Doppler shift controls |
| **FFT Size Range** | 1024 to 65536 points | Configurable windowing (Blackman-Harris, Flat-Top, Hann, Hamming) |
| **Workstation Display Compatibility** | Scalable / Resizable (Full HD, 2K, 4K) | Hardware-accelerated OpenGL / Qwt rendering pipeline |

---
*Document generated by eLynxSDR Technical Documentation Suite.*
