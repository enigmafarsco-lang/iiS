#!/usr/bin/env python3
"""
ADRV9009 Transceiver Board Test and Measurement Guide Generator
Generates a comprehensive bilingual (English and Persian) DOCX and Markdown manual
documenting test setups, RF measurement procedures, datasheet parameters,
and pass/fail criteria for the ADRV9009 RF transceiver board.
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
    """Set clean borders for table."""
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

def make_bidi_p(p):
    """Make paragraph right-to-left for Persian text."""
    pPr = p._p.get_or_add_pPr()
    pPr.append(parse_xml(f'<w:bidi {nsdecls("w")}/>'))

def make_bidi_run(run, font_name="Vazirmatn", size_pt=10.5, bold=False, color_rgb=(40,40,40)):
    """Apply RTL font properties to a run."""
    run.font.name = font_name
    run.font.size = Pt(size_pt)
    run.bold = bold
    run.font.color.rgb = RGBColor(*color_rgb)
    rPr = run._r.get_or_add_rPr()
    rFonts = parse_xml(f'<w:rFonts {nsdecls("w")} w:ascii="{font_name}" w:hAnsi="{font_name}" w:cs="{font_name}"/>')
    rPr.append(rFonts)
    rPr.append(parse_xml(f'<w:rtl {nsdecls("w")}/>'))

def add_callout(doc, text, title="NOTE", callout_type="info", is_rtl=False):
    """Add a professional styled callout box."""
    colors = {
        "info": ("003366", "EBF2FA", "INFORMATION" if not is_rtl else "اطلاعیه فنی"),
        "warning": ("B22222", "FDEDEC", "IMPORTANT WARNING" if not is_rtl else "هشدار مهم"),
        "tip": ("006644", "EAF7F0", "MEASUREMENT TIP" if not is_rtl else "نکته اندازه‌گیری"),
        "math": ("2B547E", "F0F4F8", "TECHNICAL SPECIFICATION" if not is_rtl else "مشخصات فنی و روابط")
    }
    border_color, bg_color, default_title = colors.get(callout_type, colors["info"])
    if not title:
        title = default_title

    tbl = doc.add_table(rows=1, cols=1)
    tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tbl.autofit = False

    cell = tbl.rows[0].cells[0]
    cell.width = Inches(6.5)
    set_cell_background(cell, bg_color)
    set_cell_margins(cell, top=140, bottom=140, left=200, right=200)

    side_border = f'<w:right w:val="single" w:sz="36" w:space="0" w:color="{border_color}"/>' if is_rtl else f'<w:left w:val="single" w:sz="36" w:space="0" w:color="{border_color}"/>'
    other_border = '<w:left w:val="none"/>' if is_rtl else '<w:right w:val="none"/>'
    borders = parse_xml(
        f'<w:tcBorders {nsdecls("w")}>'
        f'<w:top w:val="none"/>'
        f'<w:bottom w:val="none"/>'
        f'{side_border}'
        f'{other_border}'
        f'</w:tcBorders>'
    )
    cell._tc.get_or_add_tcPr().append(borders)

    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(0)
    p.paragraph_format.space_after = Pt(2)
    if is_rtl:
        make_bidi_p(p)
        p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        r_t = p.add_run(f"■ {title}\n")
        make_bidi_run(r_t, font_name="Vazirmatn", size_pt=10.5, bold=True, color_rgb=(0, 51, 102))
        r_b = p.add_run(text)
        make_bidi_run(r_b, font_name="Vazirmatn", size_pt=9.5, bold=False, color_rgb=(40, 40, 40))
    else:
        p.alignment = WD_ALIGN_PARAGRAPH.LEFT
        r_t = p.add_run(f"■ {title}\n")
        r_t.font.name = "Arial"
        r_t.font.size = Pt(10)
        r_t.bold = True
        r_t.font.color.rgb = RGBColor(0, 51, 102)
        r_b = p.add_run(text)
        r_b.font.name = "Arial"
        r_b.font.size = Pt(9.5)
        r_b.font.color.rgb = RGBColor(40, 40, 40)

    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(4)

def add_code_block(doc, code_text):
    """Add a shaded code block."""
    tbl = doc.add_table(rows=1, cols=1)
    tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tbl.autofit = False

    cell = tbl.rows[0].cells[0]
    cell.width = Inches(6.5)
    set_cell_background(cell, "1E1E1E")
    set_cell_margins(cell, top=120, bottom=120, left=180, right=180)

    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(0)
    p.paragraph_format.space_after = Pt(0)
    p.paragraph_format.line_spacing = 1.05

    for line in code_text.strip().split("\n"):
        run = p.add_run(line + "\n")
        run.font.name = "Consolas"
        run.font.size = Pt(9.0)
        if line.startswith("#") or line.startswith("//"):
            run.font.color.rgb = RGBColor(106, 153, 85)
        elif line.startswith("$") or line.startswith(">"):
            run.font.color.rgb = RGBColor(220, 220, 170)
            run.bold = True
        else:
            run.font.color.rgb = RGBColor(220, 220, 220)

    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(4)

def build_test_guide_document():
    doc = Document()

    # Page setup
    sections = doc.sections
    for section in sections:
        section.top_margin = Inches(0.8)
        section.bottom_margin = Inches(0.8)
        section.left_margin = Inches(0.9)
        section.right_margin = Inches(0.9)
        section.page_width = Inches(8.5)
        section.page_height = Inches(11.0)

    # Header and Footer
    header = sections[0].header
    hp = header.paragraphs[0]
    hp.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    hrun = hp.add_run("ADRV9009 RF Transceiver Board Test & Measurement Manual | eLynxSDR v0.0.2")
    hrun.font.name = "Arial"
    hrun.font.size = Pt(8.5)
    hrun.font.color.rgb = RGBColor(128, 128, 128)

    footer = sections[0].footer
    fp = footer.paragraphs[0]
    fp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    frun = fp.add_run("Confidential & Proprietary — eLynxSDR Transceiver Hardware Verification")
    frun.font.name = "Arial"
    frun.font.size = Pt(8.5)
    frun.font.color.rgb = RGBColor(128, 128, 128)

    # Document Title Block
    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_title.paragraph_format.space_before = Pt(12)
    p_title.paragraph_format.space_after = Pt(4)
    run_title = p_title.add_run("ADRV9009 RF Transceiver Board\nHardware Test, Verification & Measurement Manual")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(22)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0, 51, 102)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_after = Pt(14)
    run_sub = p_sub.add_run("Comprehensive RF Test Procedures, Datasheet Specifications, Lab Setups, and Pass/Fail Criteria\n[English & Persian Technical Documentation - راهنمای جامع آزمون و اندازه‌گیری برد]")
    run_sub.font.name = "Arial"
    run_sub.font.size = Pt(11)
    run_sub.font.color.rgb = RGBColor(90, 100, 110)

    # Metadata Table
    meta_table = doc.add_table(rows=4, cols=2)
    meta_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    meta_table.autofit = False
    set_table_borders(meta_table, "CCCCCC", "4", "single")

    meta_data = [
        ("Target Transceiver IC", "Analog Devices ADRV9009 (Dual RF Transceiver + Integrated Synthesizers)"),
        ("Host Platform / FPGA", "Zynq UltraScale+ MPSoC / Zynq-7000 (AXI4-Lite & JESD204B Subclass 1)"),
        ("Software / Driver Stack", "Linux Industrial I/O (IIO), libiio v0.24+, eLynxSDR Radar Workstation v0.0.2"),
        ("Document Version / Date", "v1.2.0 — October 2026 | Technical Standards: IEEE 802.11 / 3GPP LTE / Radar Specs")
    ]
    for row_idx, (k, v) in enumerate(meta_data):
        row = meta_table.rows[row_idx]
        c0, c1 = row.cells[0], row.cells[1]
        c0.width = Inches(2.2)
        c1.width = Inches(4.3)
        set_cell_background(c0, "F0F4F8")
        set_cell_background(c1, "FAFAFA")
        set_cell_margins(c0, 60, 60, 100, 100)
        set_cell_margins(c1, 60, 60, 100, 100)
        p0 = c0.paragraphs[0]
        p0.add_run(k).bold = True
        p0.runs[0].font.name = "Arial"
        p0.runs[0].font.size = Pt(9.5)
        p0.runs[0].font.color.rgb = RGBColor(0, 51, 102)
        p1 = c1.paragraphs[0]
        p1.add_run(v)
        p1.runs[0].font.name = "Arial"
        p1.runs[0].font.size = Pt(9.5)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 1: Executive Overview & Architecture
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("1. Executive Overview & Hardware Test Architecture")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The ADRV9009 is a highly integrated, wideband RF transceiver IC offering dual transmitters (TX1, TX2), "
        "dual receivers (RX1, RX2), and dual observation receivers (ORX1, ORX2) with integrated fractional-N synthesizers. "
        "Operating across the 75 MHz to 6000 MHz carrier range, the ADRV9009 supports up to 200 MHz receiver bandwidth "
        "and 450 MHz transmitter synthesis bandwidth. This manual specifies rigorous, laboratory-grade test procedures "
        "to validate RF performance, digital JESD204B synchronization, power supply integrity, and calibration tracking."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_callout(doc, 
        "All measurements must be performed using calibrated RF test instruments (Keysight / Rohde & Schwarz / Anritsu). "
        "RF cables must be phase-stable 50 Ω coaxial lines with < 0.5 dB insertion loss up to 6 GHz. "
        "ESD precautions (grounding strap, ESD mat) are mandatory during board probing.",
        title="MANDATORY LABORATORY SAFETY & CALIBRATION NOTICE", callout_type="warning")

    # -------------------------------------------------------------
    # SECTION 2: Datasheet Parameters Matrix
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("2. ADRV9009 Datasheet Specifications & Target Parameter Matrix")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The table below summarizes the key performance specifications extracted from the official Analog Devices "
        "ADRV9009 datasheet and hardware design manual, serving as the baseline for pass/fail validation:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    spec_tbl = doc.add_table(rows=1, cols=5)
    spec_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    spec_tbl.autofit = False
    set_table_borders(spec_tbl, "B0C4DE", "4", "single")

    headers = ["Subsystem / Parameter", "Datasheet Min", "Nominal / Typ", "Datasheet Max", "Unit / Conditions"]
    hdr_row = spec_tbl.rows[0]
    for i, h in enumerate(headers):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 100, 100)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(9.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    specs = [
        ("RF Tuning Frequency Range", "75", "—", "6000", "MHz (Continuous)"),
        ("TX Synthesis Bandwidth", "—", "200 / 400", "450", "MHz (Profile Dependent)"),
        ("RX Instantaneous Bandwidth", "—", "100 / 200", "200", "MHz (Profile Dependent)"),
        ("ORX Observation Bandwidth", "—", "200 / 400", "450", "MHz (DPD / Sniffer mode)"),
        ("TX Maximum Output Power", "+6.0", "+8.5", "+10.0", "dBm (P1dB, CW @ 2.4 GHz)"),
        ("TX Digital Attenuation Range", "0.0", "—", "41.95", "dB (0.05 dB resolution)"),
        ("TX Carrier Leakage (LOL)", "—", "-85.0", "-75.0", "dBc (With tracking cal)"),
        ("TX Image Rejection (QEC)", "70.0", "80.0", "—", "dBc (With tracking cal)"),
        ("TX Phase Noise @ 100 kHz offset", "—", "-102.0", "-98.0", "dBc/Hz (f0 = 2 GHz)"),
        ("TX Phase Noise @ 10 MHz offset", "—", "-138.0", "-135.0", "dBc/Hz (f0 = 2 GHz)"),
        ("TX Error Vector Magnitude (EVM)", "—", "0.65", "1.0", "% rms (LTE 20 MHz 64-QAM)"),
        ("RX Noise Figure (NF) @ 2 GHz", "—", "12.0", "13.5", "dB (Max gain, 2 GHz)"),
        ("RX Noise Figure (NF) @ 5 GHz", "—", "15.0", "16.5", "dB (Max gain, 5.5 GHz)"),
        ("RX Input IP3 (IIP3)", "+15.0", "+18.0", "—", "dBm (Min attenuation)"),
        ("RX Input P1dB", "-14.0", "-12.0", "—", "dBm (Max gain state)"),
        ("RX Image Rejection (QEC)", "70.0", "82.0", "—", "dBc (Tracking QEC active)"),
        ("RX Gain Control Range", "0.0", "—", "30.0", "dB (0.5 dB step MGC/AGC)"),
        ("JESD204B SerDes Data Rate", "1.2288", "9.8304", "12.288", "Gbps per lane (Subclass 1)"),
        ("Supply 1.0V Digital Core", "0.95", "1.00", "1.05", "V (Ripple < 5 mV rms)"),
        ("Supply 1.3V Analog RF/PLL", "1.24", "1.30", "1.36", "V (Ripple < 3 mV rms)"),
        ("Supply 1.8V Aux / I/O", "1.71", "1.80", "1.89", "V (Ripple < 10 mV rms)"),
        ("Operating Junction Temp", "-40", "+45", "+110", "°C (Tj on-die sensor)")
    ]

    for param, dmin, typ, dmax, unit in specs:
        row = spec_tbl.add_row()
        for i, val in enumerate([param, dmin, typ, dmax, unit]):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(spec_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 50, 50, 80, 80)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # -------------------------------------------------------------
    # SECTION 3: Test Equipment & Lab Setup
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("3. Laboratory Test Equipment & Measurement Setup")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "To perform standard verification of the ADRV9009 board, the following instruments are required in the test bench:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    eq_list = [
        ("Vector Signal Analyzer (VSA) / Spectrum Analyzer", "Keysight N9020B (MXA) / R&S FSV3000, 10 Hz – 13.6 GHz, >= 160 MHz Analysis BW."),
        ("Vector Signal Generator (VSG)", "Keysight N5182B (MXG) / R&S SMBV100B, 9 kHz – 6 GHz, with ARB AWG waveform generation."),
        ("High-Bandwidth Digital Oscilloscope", "Keysight Infiniium / R&S RTO, 4 Channels, >= 4 GHz bandwidth, 20 GSa/s sample rate."),
        ("Precision RF Power Meter & Sensors", "Keysight N1914A + E9304A Average Power Sensor (9 kHz – 6 GHz, -70 dBm to +20 dBm)."),
        ("Noise Figure Analyzer / Noise Source", "Keysight N4000A SNS Noise Source (10 MHz – 18 GHz, ENR ~ 15 dB)."),
        ("Precision DC Power Supply", "Keysight E36313A / Keithley 2230, Triple Output, low-noise ripple (< 1 mV rms)."),
        ("Host Workstation with Serial & LAN", "Ubuntu 22.04 LTS PC equipped with Gigabit Ethernet, USB-UART adapter (/dev/ttyUSB0), and IIO Oscilloscope / eLynxSDR.")
    ]
    for name, desc in eq_list:
        p_item = doc.add_paragraph()
        p_item.paragraph_format.left_indent = Inches(0.25)
        p_item.paragraph_format.space_after = Pt(2)
        r_name = p_item.add_run(f"• {name}: ")
        r_name.bold = True
        r_name.font.name = "Arial"
        r_name.font.size = Pt(9.5)
        r_name.font.color.rgb = RGBColor(0, 51, 102)
        r_desc = p_item.add_run(desc)
        r_desc.font.name = "Arial"
        r_desc.font.size = Pt(9.5)

    add_callout(doc,
        "Prior to connecting RF cables between the Signal Generator and the ADRV9009 RX ports, ALWAYS verify that the generator output power "
        "is set below -10 dBm. Connecting signals above +10 dBm to RX1/RX2 will permanently damage the on-chip Low-Noise Amplifiers (LNA).",
        title="RF INPUT DAMAGE PREVENTION WARNING", callout_type="warning")

    # -------------------------------------------------------------
    # SECTION 4: RF Transmitter (TX1 / TX2) Measurement Procedures
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("4. RF Transmitter (TX1 / TX2) Measurement Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    # 4.1 Output Power & Linearity
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.1 Transmit Output Power, Dynamic Range & Gain Flatness")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    p = doc.add_paragraph(
        "Objective: Measure maximum saturated/linear output power and dynamic attenuation control across 75 MHz to 6000 MHz."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_tx_pwr = [
        "Connect TX1 (or TX2) SMA output through a calibrated 20 dB RF attenuator to the RF Power Meter / Spectrum Analyzer.",
        "Set the ADRV9009 RF Local Oscillator to the test frequency (e.g., 2400 MHz) using eLynxSDR or IIO command: `iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 2400000000`.",
        "Configure the Direct Digital Synthesis (DDS) tone to 5 MHz offset, 0 dBFS scale on TX1: `iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 1.0`.",
        "Sweep the digital attenuation from 0 dB to 40 dB in 5 dB steps via `hardwaregain` attribute (`iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain <gain>`).",
        "Record the measured RF power at each step. Correct for cable and attenuator loss."
    ]
    for idx, s in enumerate(steps_tx_pwr, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    add_callout(doc,
        "Pass Criteria: Maximum output power >= +6 dBm at 2.4 GHz (>= +4 dBm at 5.8 GHz). "
        "Attenuation linearity error must remain < ±0.3 dB across the entire 0 dB to 40 dB range.",
        title="TX POWER PASS / FAIL CRITERIA", callout_type="math")

    # 4.2 LO Leakage (LOL) and Image Rejection (QEC)
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.2 Local Oscillator Carrier Leakage (LOL) & Quadrature Error Correction (QEC)")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    p = doc.add_paragraph(
        "Objective: Verify the on-chip automatic carrier nulling (LOL) and IQ phase/gain imbalance correction (QEC)."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_qec = [
        "Connect TX1 to Spectrum Analyzer (Center Freq = f0 = 2400 MHz, Span = 50 MHz, RBW = 10 kHz).",
        "Generate a single CW baseband tone at +10 MHz offset (f_tone = 2410 MHz).",
        "Observe the three spectral lines: Desired Tone (f0 + 10 MHz), LO Carrier Leakage (f0), and Quadrature Image (f0 - 10 MHz).",
        "Measure relative carrier suppression: LOL (dBc) = P_LO (dBm) - P_Tone (dBm).",
        "Measure relative image rejection: QEC (dBc) = P_Image (dBm) - P_Tone (dBm).",
        "Enable ADRV9009 internal tracking calibrations and observe dynamic suppression convergence."
    ]
    for idx, s in enumerate(steps_qec, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    add_callout(doc,
        "Pass Criteria: LO Carrier Leakage <= -75 dBc (nominal -85 dBc). "
        "TX Image Suppression Ratio >= 70 dBc (nominal 80 dBc).",
        title="LOL & QEC PASS / FAIL CRITERIA", callout_type="math")

    # 4.3 Phase Noise & EVM
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.3 Phase Noise, EVM & Adjacent Channel Leakage Ratio (ACLR)")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    p = doc.add_paragraph(
        "Objective: Measure spectral phase noise profile and Error Vector Magnitude under wideband modulated signals."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_pn = [
        "Connect TX1 to Signal Source Analyzer / Phase Noise Application on N9020B MXA.",
        "Transmit a clean CW tone at 2.0 GHz carrier.",
        "Measure SSB Phase Noise at 10 kHz, 100 kHz, 1 MHz, and 10 MHz offsets.",
        "Upload a standard LTE 20 MHz 64-QAM waveform into DAC DMA buffer via `iio_writedev`.",
        "Demodulate the LTE signal on the Vector Signal Analyzer and record RMS EVM and ACLR (at +20 MHz and -20 MHz offsets)."
    ]
    for idx, s in enumerate(steps_pn, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    add_callout(doc,
        "Pass Criteria: Phase noise <= -100 dBc/Hz @ 100 kHz offset; <= -136 dBc/Hz @ 10 MHz offset. "
        "EVM <= 1.0% rms (nominal 0.65%). ACLR >= 65 dBc.",
        title="PHASE NOISE & EVM PASS / FAIL CRITERIA", callout_type="math")

    # -------------------------------------------------------------
    # SECTION 5: RF Receiver (RX1 / RX2) Measurement Procedures
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("5. RF Receiver (RX1 / RX2) Measurement Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    # 5.1 Noise Figure & Sensitivity
    h2 = doc.add_heading(level=2)
    r = h2.add_run("5.1 Receiver Noise Figure (NF), Sensitivity & Minimum Detectable Signal (MDS)")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    p = doc.add_paragraph(
        "Objective: Measure receiver Noise Figure using the Y-Factor method and determine receiver sensitivity."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_rx_nf = [
        "Connect calibrated Noise Source (Keysight N4000A) to the RX1 input SMA port.",
        "Connect Noise Figure Analyzer / VSA to digitized baseband stream via IIO Ethernet / DMA capture.",
        "Perform Y-Factor measurement across 75 MHz to 6 GHz: NF = ENR - 10*log10(Y - 1), where Y = P_hot / P_cold.",
        "Calculate Minimum Detectable Signal (MDS) in dBm: MDS = -174 dBm/Hz + NF + 10*log10(BW_Hz) + SNR_min."
    ]
    for idx, s in enumerate(steps_rx_nf, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    add_callout(doc,
        "Pass Criteria: Receiver Noise Figure <= 13.5 dB at 2.4 GHz; <= 16.5 dB at 5.8 GHz at maximum gain state.",
        title="NOISE FIGURE PASS / FAIL CRITERIA", callout_type="math")

    # 5.2 IIP3 & P1dB
    h2 = doc.add_heading(level=2)
    r = h2.add_run("5.2 Input Third-Order Intercept Point (IIP3) & 1 dB Compression (P1dB)")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    p = doc.add_paragraph(
        "Objective: Evaluate receiver linearity and two-tone intermodulation distortion."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_rx_iip3 = [
        "Combine two equal-amplitude CW tones from two Signal Generators at f1 = 2400 MHz and f2 = 2401 MHz using a high-isolation RF power combiner.",
        "Inject two-tone signal into RX1 port at -25 dBm per tone.",
        "Capture baseband spectrum via IIO Oscilloscope / eLynxSDR Spectrum Analyzer.",
        "Measure the fundamental tone power (P_fund) and the 3rd-order intermodulation products at 2*f1-f2 and 2*f2-f1 (P_IM3).",
        "Calculate Input IP3: IIP3 = P_in + (P_fund - P_IM3) / 2."
    ]
    for idx, s in enumerate(steps_rx_iip3, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    add_callout(doc,
        "Pass Criteria: IIP3 >= +15 dBm (minimum gain/attenuation); IIP3 >= -12 dBm (maximum gain state). "
        "Input P1dB >= -14 dBm at maximum gain.",
        title="RECEIVER LINEARITY PASS / FAIL CRITERIA", callout_type="math")

    # -------------------------------------------------------------
    # SECTION 6: Observation Receiver (ORX) Procedures
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("6. Observation Receiver (ORX1 / ORX2) Measurement Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Objective: Validate the wideband observation receiver channels used for Digital Pre-Distortion (DPD) "
        "and wideband spectrum sniffing up to 450 MHz bandwidth."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_orx = [
        "Connect TX1 output to ORX1 input port via a 30 dB directional coupler / 30 dB attenuator.",
        "Set transceiver profile to Profile 400 (TX 400 MHz, ORX 400 MHz).",
        "Transmit a wideband Multi-Tone / Chirp signal from TX1.",
        "Capture baseband samples from ORX1 channel and verify spectrum flatness across ±200 MHz.",
        "Verify internal fast pin/SPI switching between ORX1 and ORX2 paths (< 50 µs switching time)."
    ]
    for idx, s in enumerate(steps_orx, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    add_callout(doc,
        "Pass Criteria: ORX Flatness <= ±1.5 dB across 400 MHz bandwidth. "
        "Inter-channel isolation between ORX1 and ORX2 >= 70 dB.",
        title="ORX PASS / FAIL CRITERIA", callout_type="math")

    # -------------------------------------------------------------
    # SECTION 7: Digital Interface & Clocking Tests
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("7. Digital Interface, JESD204B & Clock Distribution Verification")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Objective: Validate the high-speed SerDes links (JESD204B Subclass 1) between ADRV9009 and the FPGA, "
        "and confirm deterministic Multi-Chip Sync (MCS) phase alignment."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    steps_jesd = [
        "Connect high-speed differential probes to the JESD204B SerDes TX/RX differential lanes on the test board.",
        "Measure the SerDes eye diagram on a 20 GHz oscilloscope at 9.8304 Gbps (and 12.288 Gbps).",
        "Verify Eye Height >= 200 mVpp differential and Eye Width >= 0.65 UI (Unit Interval).",
        "Enable PRBS-7 / PRBS-31 test pattern generation in ADRV9009 and check FPGA SerDes receiver for zero bit errors (BER < 10^-12).",
        "Execute Multi-Chip Sync (MCS) SYSREF pulsing routine and verify deterministic clock phase repeatability across power cycles (< 50 ps phase skew)."
    ]
    for idx, s in enumerate(steps_jesd, 1):
        p_s = doc.add_paragraph()
        p_s.paragraph_format.left_indent = Inches(0.2)
        p_s.paragraph_format.space_after = Pt(2)
        r_num = p_s.add_run(f"Step {idx}: ")
        r_num.bold = True
        r_num.font.name = "Arial"
        r_num.font.size = Pt(9.5)
        r_txt = p_s.add_run(s)
        r_txt.font.name = "Arial"
        r_txt.font.size = Pt(9.5)

    # -------------------------------------------------------------
    # SECTION 8: Power Supply & Thermal Tests
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("8. DC Power Supply Rails, Ripple & Thermal Monitoring")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Objective: Ensure low-noise power delivery to all ADRV9009 voltage rails and verify thermal dissipation under maximum RF duty cycle."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    pwr_tbl = doc.add_table(rows=1, cols=5)
    pwr_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    pwr_tbl.autofit = False
    set_table_borders(pwr_tbl, "B0C4DE", "4", "single")

    pwr_headers = ["Power Rail", "Nominal Voltage", "Allowed Tolerance", "Max AC Ripple", "Nominal Current Draw"]
    hdr_pwr = pwr_tbl.rows[0]
    for i, h in enumerate(pwr_headers):
        cell = hdr_pwr.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 100, 100)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(9.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    pwr_specs = [
        ("VDD_DIG (Digital Core)", "1.00 V", "±5% (0.95 - 1.05 V)", "< 5 mV rms", "1.10 A (Both TX/RX active)"),
        ("VDDA_1P3 (Analog / Synth)", "1.30 V", "±5% (1.24 - 1.36 V)", "< 3 mV rms", "1.45 A (High linearity)"),
        ("VDDA_1P8 (Aux / Clock)", "1.80 V", "±5% (1.71 - 1.89 V)", "< 10 mV rms", "0.45 A"),
        ("Junction Temp (Tj)", "+45 °C (Idle)", "-40 °C to +110 °C", "—", "Heatsink + Fan Recommended")
    ]
    for rail, nom, tol, rip, cur in pwr_specs:
        row = pwr_tbl.add_row()
        for i, val in enumerate([rail, nom, tol, rip, cur]):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(pwr_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 50, 50, 80, 80)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 9: Persian Translation Section (بخش مستندات فنی به زبان فارسی)
    # -------------------------------------------------------------
    doc.add_page_break()

    p_fa_title = doc.add_paragraph()
    make_bidi_p(p_fa_title)
    p_fa_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_title.paragraph_format.space_before = Pt(16)
    p_fa_title.paragraph_format.space_after = Pt(4)
    r_fa_title = p_fa_title.add_run("دستورالعمل جامع آزمون، ارزیابی و اندازه‌گیری آزمایشگاهی\nبرد فرستنده/گیرنده رادیویی ADRV9009")
    make_bidi_run(r_fa_title, font_name="Vazirmatn", size_pt=20, bold=True, color_rgb=(0, 51, 102))

    p_fa_sub = doc.add_paragraph()
    make_bidi_p(p_fa_sub)
    p_fa_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_sub.paragraph_format.space_after = Pt(14)
    r_fa_sub = p_fa_sub.add_run("مستندات استاندارد اندازه‌گیری پارامترهای RF، کالیبراسیون، پروتکل JESD204B و معیارهای تایید/رد برد در سامانه eLynxSDR")
    make_bidi_run(r_fa_sub, font_name="Vazirmatn", size_pt=11, color_rgb=(90, 100, 110))

    # Persian Section 1
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۱. معماری تراشه ADRV9009 و راهبرد ارزیابی سخت‌افزاری")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "تراشه ADRV9009 یک فرستنده/گیرنده مجتمع رادیویی (RF Transceiver) دوکاناله با معماری تبدیل مستقیم (Direct Conversion / Zero-IF) است "
        "که دو کانال فرستنده (TX1, TX2)، دو کانال گیرنده اصلی (RX1, RX2) و دو کانال گیرنده ناظر (ORX1, ORX2) را به همراه سینتی‌سایزرهای "
        "فرکانسی کسری (Fractional-N PLL) در یک پکیج BGA ادغام کرده است. این تراشه پهنای باند کاری از ۷۵ مگاهرتز تا ۶ گیگاهرتز را پوشش داده و "
        "پهنای باند سنتز فرستنده تا ۴۵۰ مگاهرتز و پهنای باند گیرنده تا ۲۰۰ مگاهرتز را با دقت بالا فراهم می‌آورد. "
        "هدف از این مستند، ارائه دستورالعمل‌های گام‌به‌گام و استاندارد آزمایشگاهی جهت آزمون و اندازه‌گیری کلیه شاخص‌های عملکردی برد است."
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=10)

    add_callout(doc,
        "قبل از اتصال سیگنال ژنراتور به پورت‌های RX1 یا RX2، اطمینان حاصل کنید توان سیگنال ورودی کمتر از ۱۰- دسی‌بل‌میلی‌وات (dBm) تنظیم شده باشد. "
        "ورود سیگنال‌های بالاتر از ۱۰+ dBm منجر به آسیب دائمی به تقویت‌کننده‌های کم‌نویز داخلی (LNA) خواهد شد.",
        title="هشدار ایمنی تست فرکانس رادیویی", callout_type="warning", is_rtl=True)

    # Persian Section 2 - Specs Table
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۲. جدول مشخصات فنی مرجع بر اساس دیتاشیت ADRV9009")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    fa_spec_tbl = doc.add_table(rows=1, cols=5)
    fa_spec_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    fa_spec_tbl.autofit = False
    set_table_borders(fa_spec_tbl, "B0C4DE", "4", "single")

    fa_headers = ["پارامتر / شاخص عملکردی", "حداقل دیتاشیت", "مقدار نامی (Typ)", "حداکثر مجاز", "واحد / شرایط تست"]
    hdr_row = fa_spec_tbl.rows[0]
    for i, h in enumerate(fa_headers):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 100, 100)
        p = cell.paragraphs[0]
        make_bidi_p(p)
        p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        run = p.add_run(h)
        make_bidi_run(run, font_name="Vazirmatn", size_pt=9.5, bold=True, color_rgb=(255, 255, 255))

    fa_specs = [
        ("محدوده فرکانس کاری نوسان‌ساز (RF LO)", "75", "—", "6000", "مگاهرتز (پیوسته)"),
        ("پهنای باند سنتز فرستنده (TX BW)", "—", "200 / 400", "450", "مگاهرتز (بسته به پروفایل)"),
        ("پهنای باند لحظه‌ای گیرنده (RX BW)", "—", "100 / 200", "200", "مگاهرتز"),
        ("پهنای باند گیرنده ناظر (ORX BW)", "—", "200 / 400", "450", "مگاهرتز (مود اسنیفر/DPD)"),
        ("حداکثر توان خروجی فرستنده (TX P1dB)", "+6.0", "+8.5", "+10.0", "dBm (در فرکانس 2.4 GHz)"),
        ("محدوده تضعیف دیجیتال فرستنده", "0.0", "—", "41.95", "dB (با گام 0.05 dB)"),
        ("سرکوب نشتی کریر محلی فرستنده (LOL)", "—", "-85.0", "-75.0", "dBc (با کالیبراسیون فعال)"),
        ("سرکوب سیگنال تصویر فرستنده (TX QEC)", "70.0", "80.0", "—", "dBc (با کالیبراسیون ردیابی)"),
        ("نویز فاز فرستنده در آفست 100 kHz", "—", "-102.0", "-98.0", "dBc/Hz (در فرکانس 2 GHz)"),
        ("نویز فاز فرستنده در آفست 10 MHz", "—", "-138.0", "-135.0", "dBc/Hz (در فرکانس 2 GHz)"),
        ("بردار قدر مطلق خطای فرستنده (EVM)", "—", "0.65", "1.0", "% rms (مدولاسیون 64-QAM)"),
        ("ضریب نویز گیرنده (NF) در 2 GHz", "—", "12.0", "13.5", "dB (حداکثر گین)"),
        ("ضریب نویز گیرنده (NF) در 5 GHz", "—", "15.0", "16.5", "dB (حداکثر گین)"),
        ("نقطه برخورد مرتبه سوم ورودی (IIP3)", "+15.0", "+18.0", "—", "dBm (حداقل گین)"),
        ("نقطه فشرده‌سازی ۱ دسی‌بل ورودی (P1dB)", "-14.0", "-12.0", "—", "dBm (حداکثر گین)"),
        ("سرکوب تصویر در گیرنده (RX Image Rej)", "70.0", "82.0", "—", "dBc (با الگوریتم QEC)"),
        ("محدوده کنترل گین گیرنده (MGC/AGC)", "0.0", "—", "30.0", "dB (با گام 0.5 dB)"),
        ("نرخ تبادل داده سریال JESD204B", "1.2288", "9.8304", "12.288", "Gbps بر روی هر خط"),
        ("ولتاژ هسته دیجیتال (1.0V Digital)", "0.95", "1.00", "1.05", "ولت (ریپل < 5 mV rms)"),
        ("ولتاژ آنالوگ RF و سینتی‌سایزر (1.3V)", "1.24", "1.30", "1.36", "ولت (ریپل < 3 mV rms)"),
        ("ولتاژ کمکی و I/O (1.8V Aux)", "1.71", "1.80", "1.89", "ولت (ریپل < 10 mV rms)"),
        ("دمای اتصال جانکشن سیلیکون (Tj)", "-40", "+45", "+110", "درجه سانتی‌گراد (پایش داخلی)")
    ]

    for param, dmin, typ, dmax, unit in fa_specs:
        row = fa_spec_tbl.add_row()
        for i, val in enumerate([param, dmin, typ, dmax, unit]):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(fa_spec_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 50, 50, 80, 80)
            p = cell.paragraphs[0]
            make_bidi_p(p)
            p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
            run = p.add_run(val)
            make_bidi_run(run, font_name="Vazirmatn", size_pt=8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # Persian Section 3 - Measurement Steps
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۳. مراحل آزمون و اندازه‌گیری آزمایشگاهی فرستنده و گیرنده")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    fa_test_sections = [
        ("۳.۱ آزمون توان خروجی، محدوده دینامیکی و خطی بودن فرستنده (TX Output Power)",
         "۱. خروجی پورت TX1 را از طریق یک تضعیف‌کننده دقیق ۲۰ دسی‌بل با توان مناسب به اسپکتروم آنالایزر متصل نمایید.\n"
         "۲. فرکانس کریر فرستنده را روی فرکانس آزمون (مثلاً ۲۴۰۰ مگاهرتز) تنظیم نمایید.\n"
         "۳. یک تن تک‌فرکانس DDS با دامنه کامل (0 dBFS) تولید کرده و توان خروجی را اندازه‌گیری نمایید.\n"
         "۴. تضعیف دیجیتال فرستنده را از ۰ تا ۴۰ دسی‌بل با گام‌های ۵ دسی‌بل تغییر داده و خطی بودن پاسخ تضعیف‌کننده را بررسی کنید.\n"
         "معیار قبولی: توان خروجی در ۲.۴ گیگاهرتز حداقل ۶+ dBm و خطای گام تضعیف کمتر از ۰.۳± دسی‌بل باشد."),

        ("۳.۲ آزمون سرکوب نشتی نوسان‌ساز محلی (LOL) و سیگنال تصویر (QEC)",
         "۱. خروجی فرستنده را به اسپکتروم آنالایزر با رزولوشن ۱۰ کیلوهرتز متصل نمایید.\n"
         "۲. تن سینوسی در فرکانس آفست ۱۰+ مگاهرتز نسبت به کریر تولید نمایید.\n"
         "۳. دامنه مولفه مطلوب (f0 + 10MHz)، دامنه نشتی کریر (f0) و دامنه تصویر نامطلوب (f0 - 10MHz) را ثبت کنید.\n"
         "۴. سرکوب نشتی کریر (LOL) و سرکوب تصویر (QEC) را بر حسب dBc محاسبه نمایید.\n"
         "معیار قبولی: میزان سرکوب LOL کمتر از ۷۵- dBc و سرکوب تصویر QEC بهتر از ۷۰- dBc باشد."),

        ("۳.۳ آزمون ضریب نویز (Noise Figure) و حساسیت گیرنده (RX Sensitivity)",
         "۱. سورس نویز کالیبره‌شده (مانند Keysight N4000A) را به ورودی RX1 متصل نمایید.\n"
         "۲. با استفاده از روش Y-Factor و رابطه NF = ENR - 10*log10(Y - 1)، ضریب نویز گیرنده را در فرکانس‌های مختلف استخراج کنید.\n"
         "۳. حداقل سیگنال قابل آشکارسازی (MDS) را بر اساس پهنای باند و ضریب نویز محاسبه نمایید.\n"
         "معیار قبولی: ضریب نویز در فرکانس ۲.۴ گیگاهرتز کمتر از ۱۳.۵ دسی‌بل و در فرکانس ۵.۵ گیگاهرتز کمتر از ۱۶.۵ دسی‌بل باشد."),

        ("۳.۴ آزمون خطی بودن و نقطه برخورد مرتبه سوم ورودی گیرنده (RX IIP3)",
         "۱. دو تن فرکانسی هم‌توان با اختلاف فرکانسی ۱ مگاهرتز (مثلاً ۲۴۰۰ و ۲۴۰۱ مگاهرتز) را توسط کمباینر به ورودی RX1 اعمال کنید.\n"
         "۲. توان هر تن را روی ۲۵- dBm تنظیم کرده و خروجی دیجیتال بیس‌باند را ذخیره نمایید.\n"
         "۳. توان مولفه‌های مرتبه سوم مدولاسیون متقابل (IMD3) را اندازه‌گیری کرده و IIP3 را محاسبه کنید: IIP3 = Pin + (Pfund - PIM3)/2.\n"
         "معیار قبولی: مقدار IIP3 در حالت حداقل گین بهتر از ۱۵+ dBm باشد."),

        ("۳.۵ آزمون همگام‌سازی چندتراشه‌ای و سیگنالینگ دیجیتال JESD204B",
         "۱. خطوط پرسرعت دیفرانسیلی SerDes میان ADRV9009 و FPGA را با پروب‌های اسیلوسکوپ ۲۰ گیگاهرتز پایش کنید.\n"
         "۲. پترن چشمی (Eye Diagram) را در نرخ ۹.۸۳ گیگابیت بر ثانیه رسم نموده و ارتفاع چشم (> 200 mV) و عرض چشم (> 0.65 UI) را تایید کنید.\n"
         "۳. پترن تست شبه‌تصادفی PRBS-31 را فعال کرده و نرخ خطای بیت صفر (BER < 10^-12) را راستی‌آزمایی نمایید.\n"
         "۴. سیگنال پالس SYSREF را فعال نموده و قطعی بودن فاز کلاک و پایداری تاخیر را در ریبوت‌های متوالی تایید نمایید.")
    ]

    for title, desc in fa_test_sections:
        h2_fa = doc.add_heading(level=2)
        make_bidi_p(h2_fa)
        h2_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        r = h2_fa.add_run(title)
        make_bidi_run(r, font_name="Vazirmatn", size_pt=12.5, bold=True, color_rgb=(0, 102, 153))

        for line in desc.split("\n"):
            p_desc = doc.add_paragraph()
            make_bidi_p(p_desc)
            p_desc.alignment = WD_ALIGN_PARAGRAPH.RIGHT
            p_desc.paragraph_format.space_after = Pt(2)
            r = p_desc.add_run(line)
            make_bidi_run(r, font_name="Vazirmatn", size_pt=9.5)

    # Save DOCX
    docx_path = "docs/ADRV9009_Board_Test_and_Measurement_Guide.docx"
    doc.save(docx_path)
    print(f"Generated DOCX guide successfully: {docx_path}")

if __name__ == "__main__":
    build_test_guide_document()
