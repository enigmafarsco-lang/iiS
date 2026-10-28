#!/usr/bin/env python3
"""
eLynxSDR Software User Guide Generator
Generates a professional .docx manual documenting every subsystem, GUI tab,
DSP algorithm, hardware register interface, and operational procedure
based on the 19 UI screenshots in pictures/.
"""

import os
import sys
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

def set_cell_background(cell, fill_hex):
    """Set shading/background color of a table cell."""
    tcPr = cell._tc.get_or_add_tcPr()
    shd = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
    tcPr.append(shd)

def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
    """Set internal cell margins in dxa (1 pt = 20 dxa)."""
    tcPr = cell._tc.get_or_add_tcPr()
    tcMar = parse_xml(
        f'<w:tcMar {nsdecls("w")}>'
        f'<w:top w:w="{top}" w:type="dxa"/>'
        f'<w:bottom w:w="{bottom}" w:type="dxa"/>'
        f'<w:left w:w="{left}" w:type="dxa"/>'
        f'<w:right w:w="{right}" w:type="dxa"/>'
        f'</w:tcMar>'
    )
    tcPr.append(tcMar)

def set_table_borders(table, color="D3D3D3", sz="4", val="single"):
    """Set clean light borders for the entire table."""
    tblPr = table._tbl.tblPr
    borders = parse_xml(
        f'<w:tblBorders {nsdecls("w")}>'
        f'<w:top w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:bottom w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:left w:val="none"/>'
        f'<w:right w:val="none"/>'
        f'<w:insideH w:val="{val}" w:sz="{sz}" w:space="0" w:color="{color}"/>'
        f'<w:insideV w:val="none"/>'
        f'</w:tblBorders>'
    )
    tblPr.append(borders)

def add_callout(doc, text, title="NOTE", callout_type="info"):
    """Add a professional styled callout box."""
    colors = {
        "info": ("003366", "EBF2FA", "INFORMATION"),
        "warning": ("B22222", "FDEDEC", "IMPORTANT WARNING"),
        "tip": ("006644", "EAF7F0", "OPERATIONAL TIP"),
        "math": ("2B547E", "F0F4F8", "MATHEMATICAL FORMULATION")
    }
    border_color, bg_color, default_title = colors.get(callout_type, colors["info"])
    if title == "NOTE" and default_title != "INFORMATION":
        title = default_title

    tbl = doc.add_table(rows=1, cols=1)
    tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tbl.autofit = False
    
    cell = tbl.rows[0].cells[0]
    cell.width = Inches(6.5)
    set_cell_background(cell, bg_color)
    set_cell_margins(cell, top=140, bottom=140, left=200, right=200)

    # Set left border thick, others none
    tcPr = cell._tc.get_or_add_tcPr()
    borders = parse_xml(
        f'<w:tcBorders {nsdecls("w")}>'
        f'<w:top w:val="none"/>'
        f'<w:left w:val="single" w:sz="24" w:space="0" w:color="{border_color}"/>'
        f'<w:bottom w:val="none"/>'
        f'<w:right w:val="none"/>'
        f'</w:tcBorders>'
    )
    tcPr.append(borders)

    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(2)
    p.paragraph_format.line_spacing = 1.15
    run_title = p.add_run(f"[{title}] ")
    run_title.bold = True
    run_title.font.name = "Arial"
    run_title.font.size = Pt(10)
    run_title.font.color.rgb = RGBColor.from_string(border_color)

    run_text = p.add_run(text)
    run_text.font.name = "Arial"
    run_text.font.size = Pt(10)
    run_text.font.color.rgb = RGBColor(0x33, 0x33, 0x33)

    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(6)

def add_figure(doc, img_path, caption_text, width_inches=6.2):
    """Add an image with a centered caption."""
    if os.path.exists(img_path):
        p_img = doc.add_paragraph()
        p_img.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p_img.paragraph_format.space_before = Pt(8)
        p_img.paragraph_format.space_after = Pt(4)
        run_img = p_img.add_run()
        run_img.add_picture(img_path, width=Inches(width_inches))

        p_cap = doc.add_paragraph()
        p_cap.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p_cap.paragraph_format.space_before = Pt(2)
        p_cap.paragraph_format.space_after = Pt(12)
        run_cap = p_cap.add_run(caption_text)
        run_cap.italic = True
        run_cap.font.name = "Arial"
        run_cap.font.size = Pt(9.5)
        run_cap.font.color.rgb = RGBColor(0x55, 0x55, 0x55)
    else:
        p_warn = doc.add_paragraph(f"[Image file not found: {img_path}]")
        p_warn.paragraph_format.space_after = Pt(6)

def format_table(table, col_widths, headers, data):
    """Format a professional data table with shaded header and alternating rows."""
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.autofit = False
    set_table_borders(table)

    # Header Row
    hdr_cells = table.rows[0].cells
    for i, title in enumerate(headers):
        hdr_cells[i].text = title
        hdr_cells[i].width = col_widths[i]
        set_cell_background(hdr_cells[i], "003366")
        set_cell_margins(hdr_cells[i], top=120, bottom=120, left=140, right=140)
        p = hdr_cells[i].paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.LEFT
        for run in p.runs:
            run.font.name = "Arial"
            run.font.bold = True
            run.font.size = Pt(9.5)
            run.font.color.rgb = RGBColor(0xFF, 0xFF, 0xFF)

    # Data Rows
    for r_idx, row_data in enumerate(data):
        row_cells = table.add_row().cells
        bg_col = "F8F9FA" if (r_idx % 2 == 1) else "FFFFFF"
        for c_idx, val in enumerate(row_data):
            row_cells[c_idx].text = str(val)
            row_cells[c_idx].width = col_widths[c_idx]
            set_cell_background(row_cells[c_idx], bg_col)
            set_cell_margins(row_cells[c_idx], top=80, bottom=80, left=140, right=140)
            p = row_cells[c_idx].paragraphs[0]
            p.alignment = WD_ALIGN_PARAGRAPH.LEFT
            for run in p.runs:
                run.font.name = "Arial"
                run.font.size = Pt(9)
                run.font.color.rgb = RGBColor(0x22, 0x22, 0x22)

    # p_spacer


def build_user_guide():
    doc = Document()

    # Document Geometry: Standard 1 inch margins
    sections = doc.sections
    for section in sections:
        section.top_margin = Inches(1.0)
        section.bottom_margin = Inches(1.0)
        section.left_margin = Inches(1.0)
        section.right_margin = Inches(1.0)

    # Style definitions
    style_normal = doc.styles['Normal']
    style_normal.font.name = 'Arial'
    style_normal.font.size = Pt(10.5)
    style_normal.font.color.rgb = RGBColor(0x2A, 0x2A, 0x2A)

    # Title Cover Page
    p_title_space = doc.add_paragraph()
    p_title_space.paragraph_format.space_before = Pt(72)

    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run_title = p_title.add_run("eLynxSDR Software Platform\nComprehensive User & Technical Reference Guide")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(24)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0x00, 0x33, 0x66)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_before = Pt(12)
    p_sub.paragraph_format.space_after = Pt(36)
    run_sub = p_sub.add_run("Advanced Radar Signal Processing, Multi-Waveform Exciter Engine,\nDRFM Target Simulator & Wideband SDR Transceiver Control")
    run_sub.font.name = "Arial"
    run_sub.font.size = Pt(13)
    run_sub.font.color.rgb = RGBColor(0x55, 0x6B, 0x82)

    p_meta = doc.add_paragraph()
    p_meta.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_meta.paragraph_format.space_before = Pt(120)
    run_meta = p_meta.add_run(
        "Software Version: v0.0.2-Release (eLynxSDR IIO / Linux Edition)\n"
        "Transceiver Architecture: Analog Devices ADRV9009 / Zynq UltraScale+ RFSoC\n"
        "Document Reference: eLynx-UG-2026-REV3\n"
        "Classification: Technical & Operational Manual"
    )
    run_meta.font.name = "Arial"
    run_meta.font.size = Pt(10)
    run_meta.font.color.rgb = RGBColor(0x77, 0x77, 0x77)

    doc.add_page_break()

    # Table of Contents Outline
    p_toc_hdr = doc.add_heading("Table of Contents", level=1)
    p_toc_hdr.paragraph_format.space_before = Pt(12)
    p_toc_hdr.paragraph_format.space_after = Pt(12)

    toc_items = [
        ("1. System Overview & Architecture", "1.1 Hardware Architecture & Transceiver IC\n1.2 Software Stack & Qt/Qwt Framework\n1.3 Subsystem Interconnection Matrix"),
        ("2. System Startup & Connection Setup", "2.1 Network Discovery & IIO Context Initialisation\n2.2 Transceiver Profile Selection & Multi-Profile Switching"),
        ("3. Receiver & Spectrum Analysis Subsystem", "3.1 Real-Time FFT Spectrum Analyzer & Spectrogram Waterfall\n3.2 Global Controls, ENSM States & Calibration Triggers\n3.3 Profile Parameters & Digital Filter Engine\n3.4 Spectrum Display DSP Settings & Windowing Functions\n3.5 Transmitter (TX) Chain Configuration & Attenuation\n3.6 Observation Receiver (OBSRX) Chain Configuration\n3.7 RF Switch Matrix & Signal Routing Architecture\n3.8 Frequency Calibration & Phase/Gain Correction Curves"),
        ("4. Digital Radio Frequency Memory (DRFM) Subsystem", "4.1 DRFM Mathematical Principles & Doppler Formulations\n4.2 Hardware AXI Register Mapping & Bit Allocations\n4.3 Range Delay Simulation & Baseline Latency Calibration\n4.4 Independent DRFM Modulation & Seamless Exciter Playback"),
        ("5. Multi-Waveform Exciter Subsystem", "5.1 Continuous Wave (CW) Single-Tone Generator\n5.2 Spot Noise / Electronic Jamming Generator\n5.3 Linear Frequency Sweep Generator\n5.4 Pulse & Impulse Waveform Generator\n5.5 Wideband Noise & Comb Bridge Noise Generators\n5.6 Multi-Target Radar Echo Generator\n5.7 Linear Frequency Modulated (LFM Chirp) Generator"),
        ("6. Low-Level Transceiver Configuration & Control Unit", "6.1 ADRV9009 Global Register & Sensor Controls\n6.2 Multi-Chip Phase Synchronization (MCS) Workflow"),
        ("7. Operational Procedures & Best Practices", "7.1 Complete System Startup & Calibration Sequence\n7.2 Radar Target Echo Generation Workflow\n7.3 Exciter Waveform Transmission & Attenuation Control"),
        ("8. Troubleshooting, Diagnostics & Technical Specifications", "8.1 Status Indicators, Error Codes & Common Diagnostics\n8.2 Comprehensive System Technical Specifications")
    ]

    for title, subs in toc_items:
        p_t = doc.add_paragraph()
        p_t.paragraph_format.space_before = Pt(6)
        p_t.paragraph_format.space_after = Pt(1)
        r_t = p_t.add_run(title)
        r_t.bold = True
        r_t.font.name = "Arial"
        r_t.font.size = Pt(10.5)
        r_t.font.color.rgb = RGBColor(0x00, 0x33, 0x66)

        p_s = doc.add_paragraph()
        p_s.paragraph_format.space_before = Pt(0)
        p_s.paragraph_format.space_after = Pt(6)
        p_s.paragraph_format.left_indent = Inches(0.25)
        r_s = p_s.add_run(subs)
        r_s.font.name = "Arial"
        r_s.font.size = Pt(9.5)
        r_s.font.color.rgb = RGBColor(0x55, 0x55, 0x55)

    doc.add_page_break()

    # SECTION 1: System Overview & Architecture
    doc.add_heading("1. System Overview & Architecture", level=1)
    
    p = doc.add_paragraph(
        "The eLynxSDR platform is an advanced, high-performance Software Defined Radio (SDR) and Digital Radio Frequency Memory (DRFM) "
        "instrumentation suite designed for modern radar testing, electronic warfare simulation, spectrum monitoring, and wideband RF synthesis. "
        "The software provides an integrated graphical workstation interface for multi-channel RF transceivers (specifically Analog Devices ADRV9009 / AD9371 families), "
        "high-speed FPGA baseband processing engines, multi-waveform exciter signal generation, and real-time spectrum/spectrogram visualization."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_heading("1.1 Hardware Architecture & Transceiver IC", level=2)
    p = doc.add_paragraph(
        "The underlying hardware platform couples a high-density Xilinx Zynq UltraScale+ MPSoC / Kintex FPGA with the Analog Devices ADRV9009 dual-channel RF transceiver. "
        "The ADRV9009 operates over a frequency range of 75 MHz to 6 GHz, offering instantaneous receiver bandwidths up to 200 MHz and transmitter / observation receiver bandwidths up to 450 MHz. "
        "Digital sample exchange is conducted over multi-lane JESD204B high-speed serial links connected to high-performance DMA cores inside the FPGA."
    )
    p.paragraph_format.space_after = Pt(6)

    # Hardware Specs Table
    headers = ["Subsystem Component", "Hardware / Implementation", "Operational Specifications", "Interface / Bus"]
    col_w = [Inches(1.8), Inches(1.6), Inches(2.0), Inches(1.1)]
    data = [
        ["RF Transceiver", "ADRV9009 2T2R2ORX", "75 MHz – 6.0 GHz, BW up to 450 MHz", "SPI / JESD204B"],
        ["FPGA Processing Engine", "Zynq UltraScale+ / Kintex-7", "Real-time FFT, DRFM pipeline, NCO", "AXI4-Lite / AXI-Stream"],
        ["DAC DMA Engine", "High-Speed Cyclic DMA", "Continuous multi-waveform excitation", "AXI DMA / JESD204B TX"],
        ["Observation Receiver", "Dual ORX (ORX1 / ORX2)", "High dynamic range sniffer / DPD loop", "JESD204B RX / IIO"],
        ["Host Communication", "Gigabit Ethernet (TCP/IP)", "IIO Network Daemon, AXI Register bridge", "libiio TCP / Socket LAN"],
        ["RF Front-End Switches", "Solid-state PIN / SPDT", "Sub-microsecond path commutation", "FPGA GPIO / AXI IO"]
    ]
    format_table(doc.add_table(rows=1, cols=4), col_w, headers, data)

    doc.add_heading("1.2 Software Architecture & Multi-Threaded Engine", level=2)
    p = doc.add_paragraph(
        "The eLynxSDR application is built upon modern C++ and Qt/Qwt graphics subsystems, structured as a modular multi-threaded pipeline: \n"
        "• Communications Thread: Manages high-speed asynchronous TCP/IP socket connections, AXI register read/write commands, and hardware state telemetry.\n"
        "• Data Ingestion & IIO Thread: Directly streams multi-channel IQ sample buffers from the Linux Industrial I/O (IIO) kernel driver via libiio.\n"
        "• DSP Calculation Thread: Executes multi-point Fast Fourier Transforms (FFT), windowing (Hann, Hamming, Blackman-Harris, Flat-Top), peak detection, averaging, and persistence calculations.\n"
        "• OpenGL / Qwt Visualization Engine: Renders real-time spectrum traces, 2D spectrogram waterfalls, 3D waterfall surfaces, and multi-target radar parameter panels at up to 60 FPS.\n"
        "• Exciter Waveform Generator: Computes complex double-precision analytical IQ waveforms (LFM chirp, Spot noise, CW, Multi-Target, Impulse) and uploads them seamlessly into cyclic DAC DMA memory buffers."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_page_break()

    # SECTION 2: System Startup & Connection Setup
    doc.add_heading("2. System Startup & Connection Setup", level=1)
    
    doc.add_heading("2.1 Network Discovery & Connection Dialog", level=2)
    p = doc.add_paragraph(
        "Upon launching eLynxSDR, the Connection Dialog prompts the operator to specify the target SDR hardware IP address and communication port. "
        "The software connects to the embedded IIO daemon (`iiod`) running on the SDR hardware over a dedicated Gigabit Ethernet network."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-45-30.png", "Figure 2.1: eLynxSDR Hardware Connection Configuration Dialog")

    headers_conn = ["Control Element", "Parameter / Type", "Default Value", "Description & Operational Usage"]
    col_w_conn = [Inches(1.8), Inches(1.3), Inches(1.2), Inches(2.2)]
    data_conn = [
        ["IP Address Input", "IPv4 String", "192.168.1.10", "Static or DHCP IP address assigned to the SDR hardware platform."],
        ["Port / Protocol", "Integer / TCP", "30718 / 50000", "TCP control channel for register read/write and command telemetry."],
        ["Scan Network Button", "Action Trigger", "Auto-Discovery", "Scans local subnet for active eLynxSDR hardware nodes."],
        ["Connect / Cancel", "Button Controls", "N/A", "Initialises libiio context, establishes socket link, and launches main workspace."]
    ]
    format_table(doc.add_table(rows=1, cols=4), col_w_conn, headers_conn, data_conn)

    add_callout(doc, "Ensure the host PC network interface is configured with a static IPv4 address on the same subnet (e.g., 192.168.1.100, Subnet Mask 255.255.255.0) before initiating connection.", "ETHERNET CONFIGURATION", "info")

    doc.add_heading("2.2 Transceiver Profile Selection & Multi-Profile Switching", level=2)
    p = doc.add_paragraph(
        "The ADRV9009 transceiver operates according to calibrated digital clock and filter profiles loaded directly into the transceiver's internal ARM core. "
        "The Profile Selection Dialog enables the user to select from pre-calibrated profiles matching specific operational bandwidth and sample rate requirements."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-00.png", "Figure 2.2: ADRV9009 Transceiver Profile Selection and Configuration Dialog")

    headers_prof = ["Profile Identifier", "Sample Rate (MSPS)", "RX Bandwidth", "TX / ORX Bandwidth", "Application Domain"]
    col_w_prof = [Inches(1.8), Inches(1.3), Inches(1.1), Inches(1.1), Inches(1.2)]
    data_prof = [
        ["Profile 100", "122.88 MSPS", "100.00 MHz", "200.00 MHz", "Narrowband / standard surveillance, high dynamic range."],
        ["Profile 200", "245.76 MSPS", "200.00 MHz", "450.00 MHz", "Wideband radar monitoring and medium-rate DRFM emulation."],
        ["Profile 400", "491.52 MSPS", "200.00 MHz", "450.00 MHz", "Ultra-wideband pulse processing, high-fidelity chirp DRFM."]
    ]
    format_table(doc.add_table(rows=1, cols=5), col_w_prof, headers_prof, data_prof)

    add_callout(doc, "In Profile 100 (122.88 MSPS) and Profile 200 (245.76 MSPS), the software automatically maintains an uncalibrated raw noise floor baseline on ORx spectrum displays to preserve real-world noise characterization. In Profile 400 (491.52 MSPS), dynamic multi-point calibration curves are applied across the entire observation passband.", "PROFILE CALIBRATION BEHAVIOR", "tip")

    doc.add_page_break()

    # SECTION 3: Receiver & Spectrum Analysis Subsystem
    doc.add_heading("3. Receiver & Spectrum Analysis Subsystem", level=1)
    
    doc.add_heading("3.1 Main Interface Layout & High-Speed Spectrum / Waterfall", level=2)
    p = doc.add_paragraph(
        "The primary operational interface comprises an upper interactive spectrum analyzer and a lower scrolling waterfall (spectrogram) display. "
        "The display engine supports full resizability and maximization, adapting smoothly to full HD, 2K, and 4K workstation monitors."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-10.png", "Figure 3.1: eLynxSDR Primary Spectrum Display and Global Configuration Tab")

    p = doc.add_paragraph(
        "The top toolbar provides immediate operational shortcuts: \n"
        "• Time Domain View: Switches display to oscilloscope-style raw I(t) and Q(t) time-series voltage waveforms.\n"
        "• CW Table: Displays detected continuous-wave carrier frequencies, signal-to-noise ratios (SNR), and peak amplitudes.\n"
        "• Markers: Places interactive frequency/amplitude markers with delta readouts.\n"
        "• Waterfall / Waterfall 3D: Toggles between 2D continuous spectrogram roll and 3D surface mesh waterfall rendering.\n"
        "• Zoom In / Zoom Out / Autoscale: Dynamically rescales frequency span and power axis limits (-140 dBm to +10 dBm).\n"
        "• Calibration Triggers: Initiates real-time baseline, gain, and quadrature calibration routines."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_heading("3.2 Global Controls, ENSM States & Calibration Triggers", level=2)
    p = doc.add_paragraph(
        "The Global tab configures primary RF transceiver operating parameters: \n"
        "• Active ENSM: Indicates and sets Enable State Machine mode (`radio_on`, `radio_off`, `sleep`, `alert`).\n"
        "• TRX Local Oscillator (LO): Sets the RF center frequency from 75.0 MHz to 6000.0 MHz with 1 Hz tuning resolution.\n"
        "• Calibration Triggers: Initiates Internal RF LO calibration, ADC/DAC DC offset tracking, and internal tracking loops.\n"
        "• Sensor Telemetry: Monitors real-time on-chip junction temperature sensors (`ADRV9009 PHY Temp`)."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_heading("3.3 Profile Parameters & Digital Filter Configuration", level=2)
    p = doc.add_paragraph(
        "The Profile tab presents detailed decimation, interpolation, and digital FIR filter characteristics configured for the active ADRV9009 profile."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-16.png", "Figure 3.2: Transceiver Profile and Digital FIR Filter Configuration Tab")

    doc.add_heading("3.4 Spectrum Analysis DSP Settings & Windowing", level=2)
    p = doc.add_paragraph(
        "The Spectrum tab configures the mathematical Fast Fourier Transform (FFT) engine powering both the 2D spectrum plot and waterfall spectrogram."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-22.png", "Figure 3.3: Spectrum Analyzer FFT DSP Settings, Windowing Functions, and Averaging")

    headers_spec = ["Parameter", "Available Options", "Recommended Setting", "Mathematical / Operational Impact"]
    col_w_spec = [Inches(1.8), Inches(1.5), Inches(1.2), Inches(2.0)]
    data_spec = [
        ["FFT Size", "1024, 2048, 4096, 8192, 16384, 32768, 65536", "4096 / 8192", "Determines bin frequency resolution: $\Delta f = F_s / N_{FFT}$. Higher sizes increase resolution at slight compute cost."],
        ["Window Function", "Rectangular, Hann, Hamming, Blackman-Harris, Flat-Top", "Blackman-Harris", "Suppresses spectral leakage sidelobes (>92 dB attenuation for Blackman-Harris). Flat-Top provides exact amplitude calibration."],
        ["Average Count", "1 (Off) to 128", "8 – 16", "Performs exponential or linear power averaging across FFT frames to reduce thermal noise variance."],
        ["Peak Hold", "Disabled / Enabled", "Enabled (Signals)", "Retains maximum envelope power per frequency bin for transient pulse detection."],
        ["Persistence", "0% to 100%", "30% – 50%", "Decays prior FFT traces smoothly for phosphor-like visual signal persistence."]
    ]
    format_table(doc.add_table(rows=1, cols=4), col_w_spec, headers_spec, data_spec)

    doc.add_page_break()

    doc.add_heading("3.5 Transmitter (TX) Chain Configuration & Digital Attenuation", level=2)
    p = doc.add_paragraph(
        "The TX tab configures transmitter channels (TX1 and TX2), digital attenuation, baseband gain scaling, and RF Power Amplifier (PA) protection circuits."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-26.png", "Figure 3.4: Transmitter (TX) Channel Controls, Digital Attenuation, and Power Settings")

    doc.add_heading("3.6 Observation Receiver (OBSRX) Subsystem", level=2)
    p = doc.add_paragraph(
        "The Observation Receiver (OBSRX) subsystem supports dual observation channels (ORX1 and ORX2) dedicated to Digital Pre-Distortion (DPD) feedback, "
        "wideband RF sniffer monitoring, and internal RF loopback characterization."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-31.png", "Figure 3.5: Observation Receiver (OBSRX) Gain Modes, Tracking Loops, and Calibration")

    doc.add_heading("3.7 RF Switch Matrix & Signal Commutation Routing", level=2)
    p = doc.add_paragraph(
        "The Switch tab controls the hardware RF switch matrix, directing signals between external antenna ports, internal calibration loops, and receiver channels."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-36.png", "Figure 3.6: RF Switch Matrix and Signal Commutation Control Tab")

    doc.add_heading("3.8 Frequency Calibration & Gain/Phase Imbalance Correction", level=2)
    p = doc.add_paragraph(
        "The Calibration tab provides comprehensive system-level calibration routines, storing frequency-dependent gain correction offsets, "
        "phase quadrature balance values, and noise floor equalization tables across the entire 75 MHz to 6 GHz RF range."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-43.png", "Figure 3.7: Frequency Calibration Management and Gain/Phase Correction Tables")

    doc.add_page_break()

    # SECTION 4: Digital Radio Frequency Memory (DRFM) Subsystem
    doc.add_heading("4. Digital Radio Frequency Memory (DRFM) Subsystem", level=1)
    
    p = doc.add_paragraph(
        "The DRFM subsystem implements real-time radar target simulation, range delay generation, Doppler frequency shifts, and echo amplitude modulation. "
        "The DRFM engine operates entirely inside high-speed FPGA fabric with sub-nanosecond pipelining, enabling hardware-in-the-loop (HIL) radar testing."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-48.png", "Figure 4.1: Digital Radio Frequency Memory (DRFM) Target Parameter Configuration Interface")

    doc.add_heading("4.1 Mathematical Formulations & Radar Physics", level=2)
    p = doc.add_paragraph(
        "The DRFM engine precisely simulates physical radar target parameters according to fundamental electromagnetic wave propagation principles:"
    )
    p.paragraph_format.space_after = Pt(4)

    math_desc = (
        "1. Range Delay Formula:\n"
        "   The round-trip propagation time delay $\\tau$ for a simulated target at range $R$ is given by:\n"
        "      \\tau = \\frac{2 \\cdot R}{c}\n"
        "   where $c \\approx 299,792,458\\text{ m/s}$ is the speed of light in vacuum.\n"
        "   • Minimum Range: $R_{\\min} = 150\\text{ m} \\implies \\tau_{\\min} = 1.00069\\ \\mu\\text{s}$\n"
        "   • Maximum Range: $R_{\\max} = 25,000\\text{ m} (25\\text{ km}) \\implies \\tau_{\\max} = 166.782\\ \\mu\\text{s}$\n"
        "   • Range Resolution Step: $\\Delta R = 10\\text{ m} \\implies \\Delta \\tau = 66.713\\text{ ns}$\n"
        "   • Hardware Baseline Delay: An internal hardware transit latency of approximately 80 ns is compensated:\n"
        "      \\tau_{\\text{total}} = \\tau + \\tau_{\\text{baseline}} = \\frac{2 \\cdot R}{c} + 80\\text{ ns}\n\n"
        "2. Doppler Frequency Shift Formula:\n"
        "   A target moving with radial velocity $v$ illuminated by radar carrier frequency $f_0$ experiences Doppler frequency shift $f_d$:\n"
        "      f_d = \\frac{2 \\cdot v \\cdot f_0}{c} = \\frac{2 \\cdot v}{\\lambda}\n"
        "   where $\\lambda = c / f_0$ is the RF wavelength.\n"
        "   • Velocity Range: $-1000\\text{ m/s}$ to $+1000\\text{ m/s}$\n"
        "   • Carrier Range: Up to $6.0\\text{ GHz}$ (e.g., at $f_0 = 3\\text{ GHz}$ and $v = 150\\text{ m/s}$, $f_d = 3000\\text{ Hz}$)\n"
        "   • Doppler Frequency Resolution: $10\\text{ Hz}$\n\n"
        "3. FPGA Numerically Controlled Oscillator (NCO) Phase Step (thcw):\n"
        "   The Doppler frequency shift is synthesized via a 32-bit FPGA NCO phase accumulator running at sampling clock $F_s = 122.88\\text{ MHz}$:\n"
        "      \\text{thcw} = \\text{round}\\left( f_d \\cdot \\frac{2^{32}}{F_s} \\right)\n"
        "   For example, for $f_d = 1000\\text{ Hz}$ at $F_s = 122.88\\text{ MHz}$, $\\text{thcw} = 34953$."
    )
    add_callout(doc, math_desc, "RADAR KINEMATICS & NCO EQUATIONS", "math")

    doc.add_heading("4.2 Hardware AXI Register Mapping & Bit Allocations", level=2)
    p = doc.add_paragraph(
        "DRFM parameters are written directly to memory-mapped AXI4-Lite registers inside the FPGA. "
        "Crucially, modifying DRFM parameters (Range, Doppler, TH0 attenuation, or DRFM enable) does NOT re-initialize or interrupt the DAC DMA cyclic buffer, "
        "allowing active exciter waveforms to transmit continuously without phase discontinuities."
    )
    p.paragraph_format.space_after = Pt(6)

    headers_reg = ["Register Name", "AXI Base / Offset", "Bit Fields", "Data Type", "Function & Engineering Description"]
    col_w_reg = [Inches(1.5), Inches(1.2), Inches(1.0), Inches(1.0), Inches(1.8)]
    data_reg = [
        ["REG_DRFM_CTRL", "0x43C00000 / 0x00", "[0] DRFM_EN\n[1] DAC_SEL", "uint32_t", "Bit 0: Master DRFM enable. Bit 1: DAC source multiplexer (0=DMA, 1=DRFM)."],
        ["REG_DRFM_RANGE", "0x43C00000 / 0x04", "[31:0] DELAY_CYCLES", "uint32_t", "Target range delay in clock cycles: $N = \\text{round}(\\tau \\cdot F_s)$."],
        ["REG_DRFM_THCW", "0x43C00000 / 0x08", "[31:0] NCO_PHASE_INC", "uint32_t", "32-bit phase increment word $\\text{thcw}$ for Doppler frequency generation."],
        ["REG_DRFM_AMPL", "0x43C00000 / 0x0C", "[15:0] TH0_GAIN", "uint16_t", "Target echo digital scale factor / attenuation word (TH0)."],
        ["REG_DRFM_STATUS", "0x43C00000 / 0x10", "[0] FIFO_RDY\n[1] SYNC_OK", "uint32_t", "Real-time DRFM memory buffer status and phase synchronization locks."]
    ]
    format_table(doc.add_table(rows=1, cols=5), col_w_reg, headers_reg, data_reg)

    add_callout(doc, "On startup with DRFM enabled, the software checks DAC DMA buffer state; if uninitialized, it uploads a minimal 4-sample dummy zero sequence ('0 0 0 0') once to initialize the DMA clock without overwriting active exciter files.", "NON-BLOCKING DRFM OPERATION", "tip")

    doc.add_page_break()

    # SECTION 5: Multi-Waveform Exciter Subsystem
    doc.add_heading("5. Multi-Waveform Exciter Subsystem", level=1)
    
    p = doc.add_paragraph(
        "The Exciter Subsystem provides a versatile software-synthesized signal generation engine for radar target simulation, "
        "electronic countermeasure (ECM) testing, and wideband RF channel sounding. "
        "Waveforms are synthesized with double-precision floating-point accuracy, scaled to 16-bit signed integer DAC words, and uploaded directly to cyclic FPGA DMA memory."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_heading("5.1 Continuous Wave (CW) Tone Generator", level=2)
    p = doc.add_paragraph(
        "The CW generator produces single-frequency continuous wave tones with selectable center frequency offset and output attenuation. "
        "Signal routing allows independent assignment to DAC Voltage 0/1 (TX1) and Voltage 2/3 (TX2)."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-58.png", "Figure 5.1: Continuous Wave (CW) Single-Tone Exciter Generator Tab")

    doc.add_heading("5.2 Spot Noise / Electronic Attack Generator", level=2)
    p = doc.add_paragraph(
        "The Spot Noise generator synthesizes narrowband gaussian noise centered at a tunable frequency offset with user-defined bandwidth (1 MHz to 50 MHz), "
        "ideal for spot jamming and interference resilience verification."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-02.png", "Figure 5.2: Spot Noise / Targeted Interference Exciter Tab")

    doc.add_heading("5.3 Linear Frequency Sweep Generator", level=2)
    p = doc.add_paragraph(
        "The Sweep generator creates frequency sweeps over a user-defined frequency span (Start MHz to Stop MHz) with adjustable sweep duration (ms) "
        "and sweep modes (Linear Continuous or Discrete Stepped)."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-06.png", "Figure 5.3: Frequency Sweep Generator Tab (Linear / Stepped)")

    doc.add_page_break()

    doc.add_heading("5.4 Pulse & Impulse Waveform Generator", level=2)
    p = doc.add_paragraph(
        "The Impulse generator synthesizes pulsed RF signals with high-precision pulse width ($T_p$) in microseconds, "
        "Pulse Repetition Interval (PRI), and selectable amplitude envelope shaping (Rectangular, Gaussian, Hamming)."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-10.png", "Figure 5.4: Impulse / Pulsed Waveform Exciter Generator Tab")

    doc.add_heading("5.5 Wideband Noise & Comb Bridge Noise Generators", level=2)
    p = doc.add_paragraph(
        "• Wideband Noise: Generates uniform pseudo-random Gaussian noise across bandwidths up to 200 MHz with precise output power attenuation control (dBm).\n"
        "• Bridge Noise: Generates multi-carrier comb noise with selectable inter-carrier spacing and bandwidth for multi-channel receiver testing."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-14.png", "Figure 5.5: WideBand Noise Generator Tab with Attenuation Control")
    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-18.png", "Figure 5.6: Bridge (Comb) Noise Generator Tab")

    doc.add_page_break()

    doc.add_heading("5.6 Multi-Target Radar Echo Generator", level=2)
    p = doc.add_paragraph(
        "The Multi-Target Exciter enables simultaneous synthesis of up to five independent radar targets. "
        "Each target channel features independent waveform selection (Spot, CW, LFM), individual bandwidth (MHz), and Doppler frequency shift (MHz)."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-24.png", "Figure 5.7: Multi-Target Radar Echo Synthesis Generator Tab (5 Concurrent Targets)")

    headers_mt = ["Target Channel", "Waveform Type", "Bandwidth Range", "Doppler Shift Range", "Operational Application"]
    col_w_mt = [Inches(1.4), Inches(1.4), Inches(1.2), Inches(1.2), Inches(1.3)]
    data_mt = [
        ["Target 1", "Spot / CW / LFM", "1.0 – 50.0 MHz", "-50.0 to +50.0 MHz", "Primary skin return / leading target"],
        ["Target 2", "Spot / CW / LFM", "1.0 – 50.0 MHz", "-50.0 to +50.0 MHz", "Formation wingman / secondary target"],
        ["Target 3", "Spot / CW / LFM", "1.0 – 50.0 MHz", "-50.0 to +50.0 MHz", "Chaff cloud / clutter centroid"],
        ["Target 4", "Spot / CW / LFM", "1.0 – 50.0 MHz", "-50.0 to +50.0 MHz", "Coordinated deceptive decoy"],
        ["Target 5", "Spot / CW / LFM", "1.0 – 50.0 MHz", "-50.0 to +50.0 MHz", "High-speed projectile / missile threat"]
    ]
    format_table(doc.add_table(rows=1, cols=5), col_w_mt, headers_mt, data_mt)

    doc.add_heading("5.7 Linear Frequency Modulated (LFM Chirp) Generator", level=2)
    p = doc.add_paragraph(
        "The LFM generator creates Linear Frequency Modulated (Chirp) radar pulses according to user parameters: "
        "Start Frequency ($f_{\\text{start}}$ in MHz), Chirp Bandwidth ($B$ in MHz), and Pulse Duration ($T$ in microseconds). "
        "The complex baseband chirp waveform is synthesized as: \n"
        "   s(t) = \\exp\\left( j 2\\pi \\left( f_{\\text{start}} t + \\frac{B}{2 T} t^2 \\right) \\right), \\quad 0 \\le t \\le T"
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-29.png", "Figure 5.8: Linear Frequency Modulated (LFM Chirp) Generator Tab")

    doc.add_page_break()

    # SECTION 6: Low-Level Transceiver Configuration & Control Unit
    doc.add_heading("6. Low-Level Transceiver Configuration & Control Unit", level=1)
    
    p = doc.add_paragraph(
        "The Configs menu hosts low-level hardware diagnostics, internal ADRV9009 register maps, temperature monitoring, and Multi-Chip Synchronization (MCS) controls."
    )
    p.paragraph_format.space_after = Pt(6)

    add_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-35.png", "Figure 6.1: ADRV9009 Hardware Controls, Sensor Readings, and Chain Synchronization")

    headers_ctrl = ["Functional Block", "Diagnostic / Control Registers", "Monitoring Value", "Operational Function"]
    col_w_ctrl = [Inches(1.8), Inches(1.6), Inches(1.2), Inches(1.9)]
    data_ctrl = [
        ["Global Settings", "ENSM Modes, Profile Path", "radio_on", "Transceiver master operational state."],
        ["TRX Local Oscillator", "PLL Synthesizer registers", "1800.000 MHz", "Sets RF LO mixing frequency."],
        ["Thermal Sensors", "ADRV9009 PHY Temp, AD7291 Temp", "22.00 °C / Active", "Monitors junction temperatures to prevent thermal shutdown."],
        ["Frequency Hopping", "Hopping Mode, Min/Max/Step/Delay", "Disabled / Fast Hop", "Configures fast RF hopping tables across the operating band."],
        ["RX Gain & Tracking", "Hardware Gain, RSSI, Quadrature/HD2", "30.00 dB / Active", "Manual/AGC gain control and digital tracking loop enable."],
        ["TX Attenuation & PA", "PA Protection, Digital Attenuation", "0 dB Atten / Safe", "Amplifier protection interlocking and output power leveling."],
        ["MCS Sync Trigger", "SYSREF / Multi-Chip Sync", "Sync Locked", "Synchronizes internal clock dividers across multiple transceiver ICs."]
    ]
    format_table(doc.add_table(rows=1, cols=4), col_w_ctrl, headers_ctrl, data_ctrl)

    doc.add_page_break()

    # SECTION 7: Operational Procedures & Best Practices
    doc.add_heading("7. Operational Procedures & Best Practices", level=1)

    doc.add_heading("7.1 Complete System Startup & Initialisation Sequence", level=2)
    p = doc.add_paragraph(
        "Follow this standardized procedure to bring the eLynxSDR system from cold boot to fully operational status: \n"
        "1. Power on the SDR platform hardware and wait for network initialization (~30 seconds).\n"
        "2. Launch the `eLynxSDR` executable on the host workstation.\n"
        "3. In the Connection Dialog (Fig 2.1), enter the target IP (`192.168.1.10`) and click 'Connect'.\n"
        "4. In the Profile Selection Dialog (Fig 2.2), choose the appropriate operational profile (Profile 100 for 122.88 MSPS, Profile 200 for 245.76 MSPS, or Profile 400 for 491.52 MSPS) and click 'OK'.\n"
        "5. Observe the Global tab (Fig 3.1): verify that `Active ENSM` indicates `radio_on` and PHY Temperature is within normal operating limits (<65 °C).\n"
        "6. Set the desired RF Center Frequency (e.g., 1800.0 MHz) in the TRX Local Oscillator field and press Enter.\n"
        "7. Configure the Spectrum Analyzer parameters (Fig 3.3): select FFT Size (4096), Window (Blackman-Harris), and Average Count (8).\n"
        "8. Verify that clean spectrum and waterfall traces are actively streaming in the main window."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_heading("7.2 Radar Target Echo Generation & DRFM Emulation Workflow", level=2)
    p = doc.add_paragraph(
        "To simulate realistic radar targets with precise range and Doppler kinematics: \n"
        "1. Open the DRFM Tab (Fig 4.1) from the sidebar.\n"
        "2. Set the desired Target Distance (Range in meters, e.g. 5,000 m). Note: Valid range is 150 m to 25,000 m with 10 m increments.\n"
        "3. Set the Target Velocity (m/s) or directly input the Doppler Frequency Shift ($f_d$ in Hz, up to 6 GHz RF carrier with 10 Hz resolution).\n"
        "4. Adjust Target Echo Attenuation (TH0) to match the desired Radar Cross Section (RCS) return amplitude.\n"
        "5. Toggle the 'DRFM Enable' switch to ON. Observe that active exciter transmissions remain uninterrupted while the hardware DRFM registers update instantly."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_heading("7.3 Exciter Waveform Transmission & Attenuation Control", level=2)
    p = doc.add_paragraph(
        "To synthesize and transmit RF signals: \n"
        "1. Switch to the Exciter tab in the top navigation bar.\n"
        "2. Select the desired waveform generator tab (CW, Spot, Sweep, Impulse, WideBand, Bridge Noise, Multi Target, or LFM).\n"
        "3. Configure waveform parameters (e.g., for LFM: Start=0.0 MHz, BW=10.0 MHz, T=10.0 us).\n"
        "4. Select target DAC TX Channels (Voltage 0/1 for TX1, Voltage 2/3 for TX2).\n"
        "5. Click 'Set <Waveform>' or 'Generate_Send'. The status bar will indicate successful DMA buffer transfer and continuous transmission."
    )
    p.paragraph_format.space_after = Pt(6)

    doc.add_page_break()

    # SECTION 8: Troubleshooting, Diagnostics & Specifications
    doc.add_heading("8. Troubleshooting, Diagnostics & Technical Specifications", level=1)

    doc.add_heading("8.1 Common Diagnostics & Troubleshooting Matrix", level=2)
    headers_tb = ["Symptom / Error", "Probable Root Cause", "Recommended Diagnostic Action"]
    col_w_tb = [Inches(2.0), Inches(2.0), Inches(2.5)]
    data_tb = [
        ["Connection Failed / Timeout", "Network IP mismatch, firewall blocking port 30718/50000, or iiod service down.", "Verify host IP is on 192.168.1.x subnet; ping 192.168.1.10; restart iiod daemon on target hardware."],
        ["Spectrum Display Flat / No Noise Floor", "ENSM state in 'radio_off' or 'alert', or RF LO unlocked.", "Navigate to Configs -> ADRV9009, click 'Reload Settings', verify LO frequency within 75–6000 MHz range."],
        ["Spectrum Frequency Span Distorted", "Transform sampling rate ($F_s$) unassigned in FFT structure.", "Resolved in v0.0.2: verify active profile matches hardware clocking (122.88 / 245.76 / 491.52 MSPS)."],
        ["Exciter Signal Drops When Modifying DRFM", "DAC DMA cyclic buffer re-triggered on register write.", "Resolved in v0.0.2: DRFM writes directly to AXI registers without triggering DAC file reloads."],
        ["High PHY Temperature Alarm (>75 °C)", "Insufficient cooling fan airflow or heavy continuous TX at high power.", "Check cooling fan status in devices menu; reduce TX digital attenuation (increase attenuation dB)."]
    ]
    format_table(doc.add_table(rows=1, cols=3), col_w_tb, headers_tb, data_tb)

    doc.add_heading("8.2 Comprehensive System Technical Specifications", level=2)
    headers_spec_full = ["System Parameter", "Specification / Rating", "Engineering Remarks"]
    col_w_spec_full = [Inches(2.2), Inches(2.0), Inches(2.3)]
    data_spec_full = [
        ["RF Tuning Range", "75.0 MHz to 6000.0 MHz (6.0 GHz)", "Continuous tuning with 1 Hz LO synthesizer resolution"],
        ["Instantaneous RX Bandwidth", "Up to 200.0 MHz", "Supports Profile 100 (100 MHz) and Profile 200/400 (200 MHz)"],
        ["Instantaneous TX/ORX Bandwidth", "Up to 450.0 MHz", "Wideband synthesised chirp and observation feedback"],
        ["ADC / DAC Resolution", "16-bit Complex IQ (I and Q channels)", "High dynamic range (>80 dB SFDR)"],
        ["DRFM Range Delay Coverage", "150 meters to 25,000 meters (25 km)", "10-meter range step; $\\tau = 1.0\\ \\mu\\text{s}$ to $166.7\\ \\mu\\text{s}$"],
        ["DRFM Baseline Hardware Latency", "~80 nanoseconds", "Automatically compensated in hardware range delay registers"],
        ["DRFM Doppler Frequency Tuning", "Carrier up to 6.0 GHz, 10 Hz resolution", "32-bit FPGA NCO phase accumulator ($\text{thcw}$ word)"],
        ["Exciter Waveform Types", "CW, Spot Noise, Sweep, Impulse, WideBand, Bridge Noise, Multi Target, LFM, NLFM", "Arbitrary IQ synthesis with cyclic DMA burst playback"],
        ["Simultaneous Target Simulation", "Up to 5 concurrent independent targets", "Individual waveform, bandwidth, and Doppler shift controls"],
        ["FFT Size Range", "1024 to 65536 points", "Configurable windowing (Blackman-Harris, Flat-Top, Hann, Hamming)"],
        ["Workstation Display Compatibility", "Scalable / Resizable (Full HD, 2K, 4K)", "Hardware-accelerated OpenGL / Qwt rendering pipeline"]
    ]
    format_table(doc.add_table(rows=1, cols=3), col_w_spec_full, headers_spec_full, data_spec_full)

    # Save document
    out_path = os.path.join(os.path.dirname(__file__), "eLynxSDR_Software_User_Guide.docx")
    doc.save(out_path)
    print(f"Successfully generated User Guide DOCX at: {out_path}")

if __name__ == "__main__":
    build_user_guide()
