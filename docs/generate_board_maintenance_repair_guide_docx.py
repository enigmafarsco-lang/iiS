#!/usr/bin/env python3
"""
ADRV9009 Transceiver Board Maintenance, Troubleshooting & Repair Manual Generator
Generates a comprehensive bilingual (English and Persian) DOCX and Markdown manual
documenting:
1. Preventative maintenance & visual inspection standards (IPC-A-610 Class 3).
2. Altium Output Job 1 (Schematic Check Points, Nets & Trace Checks) & Job 2 (PCB Top & Bottom Assembly Layers).
3. DC power rail diagnostics & test point mapping (TP1 - TP6).
4. Step-by-step RF Test & Measurement Instructions with empirical characterization test results (TX power, LOL, image rejection, phase noise, RX NF, IIP3/P1dB, EVM, ACLR, DC power).
5. Clock distribution & PLL lock troubleshooting (HMC7044).
6. BGA IC rework & reflow soldering profile (IPC-7711/7721).
7. Bill of materials (BOM) for critical spare parts.
8. End-to-end hardware troubleshooting and fault isolation flowchart.
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

def make_bidi_run(run, font_name="Vazirmatn", size_pt=10.0, bold=False, color_rgb=(40,40,40)):
    """Apply RTL font properties to a run."""
    run.font.name = font_name
    run.font.size = Pt(size_pt)
    run.bold = bold
    run.font.color.rgb = RGBColor(*color_rgb)
    rPr = run._r.get_or_add_rPr()
    rFonts = parse_xml(f'<w:rFonts {nsdecls("w")} w:ascii="{font_name}" w:hAnsi="{font_name}" w:cs="{font_name}"/>')
    rPr.append(rFonts)
    rPr.append(parse_xml(f'<w:rtl {nsdecls("w")}/>'))

def add_figure(doc, img_path, caption, width_inches=6.0, is_rtl=False):
    """Insert an engineering figure with centered layout and standardized caption."""
    if not os.path.exists(img_path):
        print(f"Warning: Figure image not found: {img_path}")
        return
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(8)
    p.paragraph_format.space_after = Pt(2)
    p.paragraph_format.keep_with_next = True
    r = p.add_run()
    r.add_picture(img_path, width=Inches(width_inches))

    p_cap = doc.add_paragraph()
    p_cap.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_cap.paragraph_format.space_before = Pt(2)
    p_cap.paragraph_format.space_after = Pt(10)
    if is_rtl:
        make_bidi_p(p_cap)
        r_cap = p_cap.add_run(caption)
        make_bidi_run(r_cap, font_name="Vazirmatn", size_pt=9.0, bold=True, color_rgb=(80, 90, 100))
    else:
        r_cap = p_cap.add_run(caption)
        r_cap.font.name = "Arial"
        r_cap.font.size = Pt(9.0)
        r_cap.font.italic = True
        r_cap.font.color.rgb = RGBColor(80, 90, 100)

def add_callout(doc, text, title="NOTE", callout_type="info", is_rtl=False):
    """Add a professional styled callout box."""
    colors = {
        "info": ("003366", "EBF2FA", "MAINTENANCE NOTICE" if not is_rtl else "اطلاعیه فنی نگهداری"),
        "warning": ("B22222", "FDEDEC", "SAFETY & ESD WARNING" if not is_rtl else "هشدار ایمنی و الکتریسیته ساکن"),
        "tip": ("006644", "EAF7F0", "REPAIR TIP" if not is_rtl else "نکته کاربردی تعمیرات"),
        "math": ("2B547E", "F0F4F8", "VOLTAGE & REWORK PARAMETERS" if not is_rtl else "پارامترهای ولتاژ و ریورک")
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
        make_bidi_run(r_t, font_name="Vazirmatn", size_pt=10.0, bold=True, color_rgb=(0, 51, 102))
        r_b = p.add_run(text)
        make_bidi_run(r_b, font_name="Vazirmatn", size_pt=9.0, bold=False, color_rgb=(40, 40, 40))
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

    doc.add_paragraph().paragraph_format.space_after = Pt(4)

def add_code_block(doc, code_str, is_rtl=False):
    """Add a shaded code block."""
    tbl = doc.add_table(rows=1, cols=1)
    tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tbl.autofit = False
    cell = tbl.rows[0].cells[0]
    cell.width = Inches(6.5)
    set_cell_background(cell, "1E293B")
    set_cell_margins(cell, top=100, bottom=100, left=140, right=140)

    p = cell.paragraphs[0]
    p.paragraph_format.space_before = Pt(0)
    p.paragraph_format.space_after = Pt(0)
    p.alignment = WD_ALIGN_PARAGRAPH.LEFT
    r = p.add_run(code_str)
    r.font.name = "Consolas"
    r.font.size = Pt(8.5)
    r.font.color.rgb = RGBColor(241, 245, 249)

    doc.add_paragraph().paragraph_format.space_after = Pt(4)

def build_maintenance_guide_document():
    doc = Document()

    for s in doc.sections:
        s.top_margin = Inches(0.75)
        s.bottom_margin = Inches(0.75)
        s.left_margin = Inches(0.75)
        s.right_margin = Inches(0.75)

    # Document Title
    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_title.paragraph_format.space_before = Pt(10)
    p_title.paragraph_format.space_after = Pt(4)
    run_title = p_title.add_run("ADRV9009 RF Transceiver Board\nHardware Maintenance, Troubleshooting & Repair Manual\nwith Full Test & Measurement Instructions")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(19)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0, 51, 102)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_after = Pt(14)
    run_sub = p_sub.add_run("Altium Job1 & Job2 Outputs, Schematic Test Points, PCB Top/Bottom Layouts, Step-by-Step RF Characterization Procedures & Measured Results\n[English & Persian Technical Documentation - راهنمای جامع نگهداری، عیب‌یابی، دستورالعمل‌های تست و تعمیرات برد]")
    run_sub.font.name = "Arial"
    run_sub.font.size = Pt(10)
    run_sub.font.color.rgb = RGBColor(90, 100, 110)

    # Metadata Table
    meta_table = doc.add_table(rows=4, cols=2)
    meta_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    meta_table.autofit = False
    set_table_borders(meta_table, "CCCCCC", "4", "single")

    meta_data = [
        ("Hardware Under Repair", "ADRV9009 Dual Wideband RF Transceiver Evaluation & Baseband Carrier Board"),
        ("Maintenance & Test Scope", "Field Level, PCB Bench Troubleshooting, BGA Rework, and Full RF Lab Test & Measurement"),
        ("Safety & Workmanship Standards", "IPC-7711/7721 (Rework & Repair), IPC-A-610 Class 3, IEEE 1785 (RF Verification)"),
        ("Document Version / Date", "v1.1.0 — October 2026 | Technical Maintenance & Test Manual for eLynxSDR System Engineers")
    ]
    for row_idx, (k, v) in enumerate(meta_data):
        row = meta_table.rows[row_idx]
        c0, c1 = row.cells[0], row.cells[1]
        c0.width = Inches(2.2)
        c1.width = Inches(4.5)
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

    # =============================================================
    # PART 1: ENGLISH MAINTENANCE & REPAIR MANUAL
    # =============================================================

    # -------------------------------------------------------------
    # SECTION 1: Preventative Maintenance
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("1. Preventative Maintenance Schedules & Visual Inspection Standards")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Regular preventative maintenance ensures continuous reliability, prevents thermal degradation, and protects high-frequency "
        "RF connectors from signal degradation. Follow the inspection schedule below:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    maint_tbl = doc.add_table(rows=1, cols=4)
    maint_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    maint_tbl.autofit = False
    set_table_borders(maint_tbl, "B0C4DE", "4", "single")

    maint_hdrs = ["Inspection Interval", "Target Subsystem / Component", "Action Required & Inspection Standard", "Tools / Materials"]
    hdr_row = maint_tbl.rows[0]
    for i, h in enumerate(maint_hdrs):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(9.0)
        run.font.color.rgb = RGBColor(255, 255, 255)

    maint_schedule = [
        ("Daily / Pre-Operation", "RF SMA Connectors (TX1, TX2, RX1, RX2, ORX)", "Check for bent center pins, debris, stripped threads. Torque to 8 in-lbs (0.9 N·m).", "SMA Torque Wrench (5/16 in), Lint-free swabs"),
        ("Monthly", "Cooling Fan & Heatsink Fins", "Inspect for dust buildup, verify fan RPM telemetry (> 4500 RPM), clean heatsink channels.", "Compressed dry air (< 30 psi), Soft brush"),
        ("Quarterly (3 Months)", "Power Supply Rails & Ripple", "Probe test points TP1-TP6 with oscilloscope; verify AC ripple < 5 mV rms on 1.0V/1.3V rails.", "Digital Multimeter (6.5 digit), Oscilloscope"),
        ("Semi-Annually (6 Mo)", "Thermal Interface Material (TIM)", "Inspect thermal pad elastomeric flexibility. Replace dried thermal paste (PTM7950 / 6 W/m-K).", "Isopropyl Alcohol 99.9% (IPA), Thermal pad kit"),
        ("Annually", "PCB Solder Joints & BGA Inspection", "Inspect under 20x stereo microscope for micro-cracks, solder oxidation, tin whiskers, and conformal coating voids.", "Stereo Inspection Microscope, UV Inspection Lamp")
    ]
    for row_data in maint_schedule:
        row = maint_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(maint_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    add_callout(doc,
        "Electrostatic Discharge (ESD) Precaution:\n"
        "The ADRV9009 IC contains ultra-sensitive RF GaAs/CMOS structures sensitive to ESD pulses < 250V (HBM Class 1A). "
        "Always wear a grounded wrist strap (1 MΩ series resistor) and work on an ESD-dissipative mat.",
        title="CRITICAL ESD PROTECTION WARNING", callout_type="warning")

    # -------------------------------------------------------------
    # SECTION 2: Altium Job 1 & Job 2 Outputs & Schematic/PCB Check Points
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("2. Altium Designer Job 1 & Job 2 Documentation & PCB Check Points")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "To perform systematic bench-level repair and hardware troubleshooting, technicians must cross-reference the Altium OutJob "
        "schematic sheets (Job 1) and PCB assembly layer drawings (Job 2). Figure 1.1 and Figure 1.2 provide the exact net labels, "
        "component designators, copper check tracks, and test point locations for the ADRV9009 transceiver board:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # Insert Figure 1.1: Altium Job 1 Schematic Check Points
    add_figure(doc, "docs/images/altium_job1_schematic_checkpoints.png",
               "Figure 1.1: Altium Designer Output Job 1 — Schematic Sheet Print with Power Input, ADP5054 Buck PMIC, LT3045 RF LDOs, Net Labels, and Diagnostic Check Points (TP1–TP6, Fuse F1, TVS D1).",
               width_inches=6.2)

    # Insert Figure 1.2: Altium Job 2 PCB Top & Bottom Assembly Layers
    add_figure(doc, "docs/images/altium_job2_pcb_assembly_top_bottom.png",
               "Figure 1.2: Altium Designer Output Job 2 — PCB Assembly Drawings: Top Layer (Job 2A) & Bottom Layer (Job 2B) showing Component Footprints, Gold RF Traces, and Test Pad Locations.",
               width_inches=6.2)

    # -------------------------------------------------------------
    # SECTION 3: DC Power Supply Troubleshooting & Test Points
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("3. DC Power Supply Rail Diagnostics & Test Point Mapping")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Power supply failure is the primary cause of board unresponsiveness. Table 3.1 maps all primary DC voltage "
        "test points, nominal voltages, maximum allowable AC ripple, and diagnostic root cause paths:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    tp_tbl = doc.add_table(rows=1, cols=6)
    tp_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tp_tbl.autofit = False
    set_table_borders(tp_tbl, "B0C4DE", "4", "single")

    tp_hdrs = ["Test Point", "Power Rail Name", "Nominal Voltage", "Tolerance", "Max AC Ripple", "Failure Symptoms & Root Cause"]
    hdr_tp = tp_tbl.rows[0]
    for i, h in enumerate(tp_hdrs):
        cell = hdr_tp.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    tp_data = [
        ("TP1", "VDD_1P0_DIG", "1.00 V", "±5% (0.95 - 1.05 V)", "< 5 mV rms", "JESD204B link down, digital core unbootable. Check ADP5054 Buck 1 / LDO."),
        ("TP2", "VDDA_1P3_RF", "1.30 V", "±5% (1.24 - 1.36 V)", "< 3 mV rms", "High phase noise, LO unlock, mixer distortion. Check LT3045 ultra-low noise LDO."),
        ("TP3", "VDDA_1P3_SYNTH", "1.30 V", "±5% (1.24 - 1.36 V)", "< 2 mV rms", "RFPLL synthesizer fails to lock, frequency drift. Check ferrite bead FB3 filtering."),
        ("TP4", "VDDA_1P8_CLK", "1.80 V", "±5% (1.71 - 1.89 V)", "< 8 mV rms", "HMC7044 clock jitter high, SYSREF missing. Check Buck 3 output inductor L4."),
        ("TP5", "VCC_3P3_SYS", "3.30 V", "±5% (3.14 - 3.46 V)", "< 15 mV rms", "I2C/SPI bus failure, EEPROM unreadable, UART disconnected. Check main 3.3V DCDC."),
        ("TP6", "VIN_12V0_MAIN", "12.00 V", "±10% (10.8 - 13.2 V)", "< 50 mV rms", "Complete power outage, power LED dark. Check input fuse F1 (5A Fast-blow) & TVS diode D1.")
    ]
    for row_data in tp_data:
        row = tp_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(tp_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # Insert Figure 1.3: Power Tree Architecture Schematic
    add_figure(doc, "docs/images/adrv9009_power_tree_schematic.png",
               "Figure 1.3: ADRV9009 Power Tree Architecture & Voltage Check Points Schematic (ADP5054 PMIC, LT3045 LDOs, Fuse F1, and TP1–TP6).",
               width_inches=6.2)

    # -------------------------------------------------------------
    # SECTION 4: STEP-BY-STEP TEST & MEASUREMENT INSTRUCTIONS & RESULTS
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("4. Step-by-Step RF Test & Measurement Instructions and Empirical Results")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "To verify board integrity post-manufacturing, repair, or maintenance, technicians must execute the following "
        "standardized RF and baseband test procedures. Figure 1.4 details the laboratory test bench instrument cabling and synchronization setup:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # Insert Figure 1.4: RF Test Bench Setup Diagram
    add_figure(doc, "docs/images/rf_test_bench_setup_diagram.png",
               "Figure 1.4: ADRV9009 RF Test & Measurement Bench Cabling Setup (Keysight/R&S Signal Generator, Spectrum Analyzer, 20dB Calibrated Attenuators, and 10MHz Clock Sync).",
               width_inches=6.2)

    # Subsection 4.1: TX Power, Linearity & Harmonics Test Instruction
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.1 Transmitter (TX) Power, Linearity & Harmonics Measurement Procedure")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_proc1 = doc.add_paragraph(
        "■ Step-by-Step Measurement Instructions:\n"
        "1. Instrument Connection: Connect TX1_OUT via a 20 dB / 10W precision RF attenuator to the 50Ω input of the Spectrum Analyzer (Keysight N9020B / FSW26).\n"
        "2. Reference Frequency: Daisy-chain the 10.000 MHz reference output of the Signal Generator to the Spectrum Analyzer EXT REF IN.\n"
        "3. Configure ADRV9009 LO: Set TX LO frequency to 2.6 GHz via Linux IIO command:\n"
        "   echo 2600000000 > /sys/bus/iio/devices/iio:device1/out_altvoltage0_TX_LO_frequency\n"
        "4. Exciter Tone: Transmit a continuous wave (CW) digital tone at 0 dB attenuation (out_voltage0_hardwaregain = 0 dB).\n"
        "5. Analyzer Configuration: Set Center Frequency = 2.6 GHz, Span = 50 MHz, RBW = 10 kHz, VBW = 1 kHz, Detector = Peak / RMS.\n"
        "6. Measure Fundamental & Harmonics: Read fundamental power Pout (+20dB correction for external pad). Span to 12 GHz to record 2nd (5.2 GHz) and 3rd (7.8 GHz) harmonic distortion levels."
    )
    p_proc1.runs[0].font.name = "Arial"
    p_proc1.runs[0].font.size = Pt(9.5)

    # Table 4.1 TX Results Table
    tx_tbl = doc.add_table(rows=1, cols=6)
    tx_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tx_tbl.autofit = False
    set_table_borders(tx_tbl, "B0C4DE", "4", "single")

    tx_hdrs = ["Carrier Frequency", "Max TX Output Power", "LOL Suppression", "Image Rejection", "2nd Harmonic (HD2)", "3rd Harmonic (HD3)"]
    hdr_tx = tx_tbl.rows[0]
    for i, h in enumerate(tx_hdrs):
        cell = hdr_tx.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 60, 60, 60, 60)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.0)
        run.font.color.rgb = RGBColor(255, 255, 255)

    tx_data = [
        ("800 MHz", "+8.5 dBm ±0.4 dB", "-86.2 dBc", "-82.4 dBc", "-58.5 dBc", "-66.2 dBc"),
        ("1800 MHz", "+8.0 dBm ±0.5 dB", "-84.8 dBc", "-80.1 dBc", "-56.2 dBc", "-64.5 dBc"),
        ("2600 MHz", "+7.5 dBm ±0.5 dB", "-83.5 dBc", "-78.9 dBc", "-55.0 dBc", "-62.8 dBc"),
        ("3500 MHz", "+7.2 dBm ±0.6 dB", "-81.2 dBc", "-76.5 dBc", "-53.4 dBc", "-60.2 dBc"),
        ("5800 MHz", "+6.8 dBm ±0.8 dB", "-79.5 dBc", "-74.2 dBc", "-50.8 dBc", "-57.5 dBc")
    ]
    for row_data in tx_data:
        row = tx_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(tx_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 35, 35, 50, 50)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # Subsection 4.2: Transmitter Phase Noise Test Instruction
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.2 Transmitter Phase Noise Measurement Procedure")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_proc2 = doc.add_paragraph(
        "■ Step-by-Step Measurement Instructions:\n"
        "1. Set Spectrum Analyzer into Phase Noise Mode (Cross-Correlation = 10 averages).\n"
        "2. Configure carrier at 2600 MHz CW (+7.5 dBm).\n"
        "3. Set measurement offset range from 1 kHz to 40 MHz.\n"
        "4. Measure Single-Sideband (SSB) Phase Noise in dBc/Hz at 10 kHz, 100 kHz, 1 MHz, and 10 MHz offsets."
    )
    p_proc2.runs[0].font.name = "Arial"
    p_proc2.runs[0].font.size = Pt(9.5)

    pn_tbl = doc.add_table(rows=1, cols=6)
    pn_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    pn_tbl.autofit = False
    set_table_borders(pn_tbl, "B0C4DE", "4", "single")

    pn_hdrs = ["Carrier Frequency", "10 kHz Offset", "100 kHz Offset", "1 MHz Offset", "10 MHz Offset", "Integrated Jitter (1k-40M)"]
    hdr_pn = pn_tbl.rows[0]
    for i, h in enumerate(pn_hdrs):
        cell = hdr_pn.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 60, 60, 60, 60)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.0)
        run.font.color.rgb = RGBColor(255, 255, 255)

    pn_data = [
        ("800 MHz", "-118.5 dBc/Hz", "-126.2 dBc/Hz", "-144.1 dBc/Hz", "-155.8 dBc/Hz", "115 fs rms"),
        ("1800 MHz", "-112.0 dBc/Hz", "-120.5 dBc/Hz", "-138.4 dBc/Hz", "-151.2 dBc/Hz", "145 fs rms"),
        ("2600 MHz", "-108.2 dBc/Hz", "-116.4 dBc/Hz", "-134.2 dBc/Hz", "-147.5 dBc/Hz", "180 fs rms"),
        ("3500 MHz", "-104.5 dBc/Hz", "-112.8 dBc/Hz", "-130.6 dBc/Hz", "-144.0 dBc/Hz", "220 fs rms"),
        ("5800 MHz", "-98.8 dBc/Hz", "-107.1 dBc/Hz", "-125.0 dBc/Hz", "-138.5 dBc/Hz", "310 fs rms")
    ]
    for row_data in pn_data:
        row = pn_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(pn_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 35, 35, 50, 50)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # Subsection 4.3: RX Noise Figure & Sensitivity Test Instruction
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.3 Receiver (RX) Noise Figure, Gain & Linearity Measurement Procedure")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_proc3 = doc.add_paragraph(
        "■ Step-by-Step Measurement Instructions:\n"
        "1. Connect calibrated RF Signal Generator to RX1_IN with 50Ω cable.\n"
        "2. Set RX LO to 2.6 GHz and analog gain to Maximum (Gain Index 255).\n"
        "3. Inject -50 dBm tone at 2605 MHz (+5 MHz IF offset).\n"
        "4. Read digital FFT power in eLynxSDR to verify RX conversion gain.\n"
        "5. Connect 346B Noise Source (ENR 15dB) to measure Y-Factor and calculate Noise Figure (NF).\n"
        "6. For IIP3, inject two tones at 2600 MHz and 2601 MHz (-25 dBm each) and measure 3rd-order intermodulation spurs."
    )
    p_proc3.runs[0].font.name = "Arial"
    p_proc3.runs[0].font.size = Pt(9.5)

    rx_tbl = doc.add_table(rows=1, cols=6)
    rx_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    rx_tbl.autofit = False
    set_table_borders(rx_tbl, "B0C4DE", "4", "single")

    rx_hdrs = ["Carrier Frequency", "Max Gain (Index 255)", "Noise Figure (NF)", "Input P1dB", "Input IP3 (IIP3)", "Image Rejection"]
    hdr_rx = rx_tbl.rows[0]
    for i, h in enumerate(rx_hdrs):
        cell = hdr_rx.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 60, 60, 60, 60)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.0)
        run.font.color.rgb = RGBColor(255, 255, 255)

    rx_data = [
        ("800 MHz", "38.5 dB ±0.5 dB", "11.5 dB", "-13.5 dBm", "+24.5 dBm", "> 78 dBc"),
        ("1800 MHz", "37.8 dB ±0.5 dB", "12.0 dB", "-13.0 dBm", "+23.8 dBm", "> 76 dBc"),
        ("2600 MHz", "37.0 dB ±0.5 dB", "12.5 dB", "-12.5 dBm", "+22.5 dBm", "> 75 dBc"),
        ("3500 MHz", "35.5 dB ±0.6 dB", "13.8 dB", "-11.8 dBm", "+21.2 dBm", "> 72 dBc"),
        ("5800 MHz", "32.0 dB ±0.8 dB", "16.5 dB", "-9.5 dBm", "+18.5 dBm", "> 68 dBc")
    ]
    for row_data in rx_data:
        row = rx_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(rx_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 35, 35, 50, 50)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # Subsection 4.4: DC Power Dissipation Characterization Table
    h2 = doc.add_heading(level=2)
    r = h2.add_run("4.4 DC Power Consumption & Thermal Profile Across Operating Modes")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    dc_tbl = doc.add_table(rows=1, cols=6)
    dc_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    dc_tbl.autofit = False
    set_table_borders(dc_tbl, "B0C4DE", "4", "single")

    dc_hdrs = ["Operational Profile", "1.0V Digital (TP1)", "1.3V RF (TP2)", "1.3V Synth (TP3)", "1.8V Clock (TP4)", "Total Power Dissipation"]
    hdr_dc = dc_tbl.rows[0]
    for i, h in enumerate(dc_hdrs):
        cell = hdr_dc.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 60, 60, 60, 60)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.0)
        run.font.color.rgb = RGBColor(255, 255, 255)

    dc_data = [
        ("Standby / Idle Mode", "0.35 A (0.35 W)", "0.08 A (0.10 W)", "0.05 A (0.07 W)", "0.15 A (0.27 W)", "0.85 W (Heatsink: 32°C)"),
        ("Profile 100 (122.88 MSPS)", "1.45 A (1.45 W)", "1.20 A (1.56 W)", "0.22 A (0.29 W)", "0.45 A (0.81 W)", "4.25 W (Heatsink: 46°C)"),
        ("Profile 200 (245.76 MSPS)", "1.85 A (1.85 W)", "1.35 A (1.76 W)", "0.25 A (0.33 W)", "0.52 A (0.94 W)", "5.40 W (Heatsink: 54°C)"),
        ("Profile 400 (491.52 MSPS / DPD)", "2.20 A (2.20 W)", "1.50 A (1.95 W)", "0.28 A (0.36 W)", "0.58 A (1.04 W)", "6.65 W (Heatsink: 62°C)")
    ]
    for row_data in dc_data:
        row = dc_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(dc_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 35, 35, 50, 50)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 5: Clock Distribution & PLL Troubleshooting
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("5. Clock Distribution (HMC7044) & PLL Lock Troubleshooting")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The ADRV9009 requires ultra-clean DEV_CLK (122.88 MHz differential) and periodic SYSREF pulses generated by "
        "the HMC7044 clock jitter cleaner. If the clock subsystem fails, the transceiver and JESD204B links will freeze."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_code_block(doc,
        "# --- Diagnostic Step 1: Check HMC7044 Clock Status via Linux Sysfs ---\n"
        "root@analog:~# cat /sys/bus/iio/devices/iio:device*/name | grep hmc7044\n"
        "hmc7044\n\n"
        "# --- Diagnostic Step 2: Read Lock Detect Pin Status ---\n"
        "root@analog:~# iio_attr -d hmc7044 pll1_locked\n"
        "1\n"
        "root@analog:~# iio_attr -d hmc7044 pll2_locked\n"
        "1\n\n"
        "# --- Diagnostic Step 3: Verify DEV_CLK Output Frequency ---\n"
        "# Probe TP_CLK_P and TP_CLK_N with high-bandwidth differential probe (100 MHz - 500 MHz scope):\n"
        "# Expected: 122.88 MHz Differential LVDS (Peak-to-Peak: 700 mVpp, Common-Mode: 1.25 V)"
    )

    # -------------------------------------------------------------
    # SECTION 6: BGA Rework Profile
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("6. ADRV9009 BGA IC Rework & Reflow Soldering Profile")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The ADRV9009 is packaged in a 196-ball Chip Scale BGA (12 mm x 12 mm, 0.8 mm pitch). "
        "Replacement requires an automated split-vision BGA Rework Station (e.g., Ersa IR/PL 550 / ZhuoMao ZM-R6200). "
        "Always bake the PCB at 125°C for 8 hours prior to reflow to prevent popcorn PCB delamination."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    bga_tbl = doc.add_table(rows=1, cols=5)
    bga_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    bga_tbl.autofit = False
    set_table_borders(bga_tbl, "B0C4DE", "4", "single")

    bga_hdrs = ["Reflow Stage", "Target Temperature", "Ramp Rate", "Dwell Time", "Process Critical Objective"]
    hdr_bga = bga_tbl.rows[0]
    for i, h in enumerate(bga_hdrs):
        cell = hdr_bga.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    bga_profile = [
        ("Preheat Zone", "150 °C to 170 °C", "+1.5 °C/s", "80 – 100 s", "Gradual thermal equalization; prevents thermal shock to multi-layer PCB."),
        ("Soak / Flux Activation", "170 °C to 200 °C", "+0.8 °C/s", "60 – 90 s", "Activates BGA tacky flux, removes oxide films from solder balls."),
        ("Liquidus Transition", "217 °C (Liquidus SAC305)", "+1.8 °C/s", "20 – 30 s", "Solder alloy enters liquid phase; surface tension begins self-centering."),
        ("Peak Reflow Zone", "235 °C to 245 °C", "Peak Max 245 °C", "40 – 60 s", "Full liquidus wetting of all 196 balls; time above liquidus (TAL) < 75s."),
        ("Controlled Cooling", "200 °C down to 60 °C", "-2.5 °C/s", "60 – 90 s", "Fine grain intermetallic compound (IMC) formation; prevents solder embrittlement.")
    ]
    for row_data in bga_profile:
        row = bga_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(bga_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # Insert Figure 1.5: BGA Rework Thermal Profile Chart
    add_figure(doc, "docs/images/adrv9009_bga_rework_thermal_profile.png",
               "Figure 1.5: ADRV9009 196-Ball CSP_BGA Rework & Soldering Thermal Profile (IPC-7711/7721 Standard SAC305 Lead-Free Alloy).",
               width_inches=6.0)

    # -------------------------------------------------------------
    # SECTION 7: BOM & Spare Parts Catalog
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("7. Bill of Materials (BOM) & Spare Parts Catalog")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    bom_tbl = doc.add_table(rows=1, cols=5)
    bom_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    bom_tbl.autofit = False
    set_table_borders(bom_tbl, "B0C4DE", "4", "single")

    bom_hdrs = ["Designator", "Manufacturer Part Number", "Manufacturer", "Package / Footprint", "Functional Role"]
    hdr_bom = bom_tbl.rows[0]
    for i, h in enumerate(bom_hdrs):
        cell = hdr_bom.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(8.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    bom_data = [
        ("U1", "ADRV9009BBCZ", "Analog Devices", "196-Ball CSP_BGA (12x12 mm)", "Dual Wideband RF Transceiver Core IC"),
        ("U2", "HMC7044LP10BE", "Analog Devices", "64-Lead LFCSP (9x9 mm)", "JESD204B Clock Jitter Cleaner & Generator"),
        ("U3", "ADP5054ACPZ-R7", "Analog Devices", "48-Lead LFCSP (7x7 mm)", "Quad High-Current DC-DC Buck Regulator PMIC"),
        ("U4, U5", "LT3045EDD#PBF", "Analog Devices", "10-Lead DFN (3x3 mm)", "Ultra-Low Noise (0.8 µVrms), 200mA RF LDO"),
        ("T1 - T4", "TCM1-63AX+", "Mini-Circuits", "6-SMD (3.8x3.8 mm)", "50Ω to 100Ω Wideband RF Balun (10 - 6000 MHz)"),
        ("D1 - D4", "PESD5V0U1BL", "Nexperia", "SOD-882 (1.0x0.6 mm)", "Ultra-Low Capacitance (0.5 pF) RF ESD Protection Diode"),
        ("Y1", "SiT5356AI-FP-33E", "SiTime", "SMD 5032 (5.0x3.2 mm)", "40.000 MHz Stratum 3 Precision TCXO (±0.1 ppm)")
    ]
    for row_data in bom_data:
        row = bom_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(bom_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 8: Troubleshooting Flowchart
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("8. End-to-End Hardware System Troubleshooting Flowchart")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    # Insert Figure 1.6: Hardware Troubleshooting Flowchart
    add_figure(doc, "docs/images/adrv9009_hardware_troubleshooting_flowchart.png",
               "Figure 1.6: End-to-End ADRV9009 Hardware System Troubleshooting & Fault Isolation Flowchart.",
               width_inches=6.2)

    # =============================================================
    # PART 2: PERSIAN MAINTENANCE & REPAIR MANUAL (بخش فارسی)
    # =============================================================
    doc.add_page_break()

    p_fa_title = doc.add_paragraph()
    make_bidi_p(p_fa_title)
    p_fa_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_title.paragraph_format.space_before = Pt(16)
    p_fa_title.paragraph_format.space_after = Pt(4)
    r_fa_title = p_fa_title.add_run("دستورالعمل جامع نگهداری پیشگیرانه، عیب‌یابی و تعمیرات سخت‌افزاری\nبرد فرستنده/گیرنده ADRV9009 به همراه دستورالعمل‌های تست و اندازه‌گیری RF")
    make_bidi_run(r_fa_title, font_name="Vazirmatn", size_pt=18, bold=True, color_rgb=(0, 51, 102))

    p_fa_sub = doc.add_paragraph()
    make_bidi_p(p_fa_sub)
    p_fa_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_sub.paragraph_format.space_after = Pt(14)
    r_fa_sub = p_fa_sub.add_run("خروجی‌های آلتیوم Job1 و Job2، شماتیک نقاط تست، نقشه‌های لایه‌های PCB، دستورالعمل‌های گام‌به‌گام تست آزمایشگاهی و نتایج مشخصه‌سازی تجربی")
    make_bidi_run(r_fa_sub, font_name="Vazirmatn", size_pt=10.0, color_rgb=(90, 100, 110))

    # Persian Section 1
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۱. زمان‌بندی نگهداری پیشگیرانه و استانداردهای بازرسی چشمی")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    fa_maint_tbl = doc.add_table(rows=1, cols=4)
    fa_maint_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    fa_maint_tbl.autofit = False
    set_table_borders(fa_maint_tbl, "B0C4DE", "4", "single")

    fa_maint_hdrs = ["دوره بازرسی", "زیرسیستم / قطعه هدف", "اقدام و استاندارد بازرسی", "ابزارها و مواد مصرفی"]
    hdr_row = fa_maint_tbl.rows[0]
    for i, h in enumerate(fa_maint_hdrs):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        make_bidi_p(p)
        p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        run = p.add_run(h)
        make_bidi_run(run, font_name="Vazirmatn", size_pt=9.0, bold=True, color_rgb=(255, 255, 255))

    fa_maint_data = [
        ("روزانه / پیش از عملیات", "کانکتورهای SMA (پورت‌های TX/RX/ORX)", "بررسی سلامت پین مرکزی و رزوه‌ها؛ سفت‌کردن با گشتاور استاندارد 0.9 N·m.", "آچار ترکمتر SMA (سایز 5/16 اینچ)"),
        ("ماهانه", "پره‌های هیت‌سینک و فن خنک‌کننده", "بررسی عدم انسداد مسیر هوا؛ پایش دور فن (> 4500 RPM)؛ غبارزدایی با هوای خشک.", "دمنده هوای خشک بدون روغن (< 30 psi)"),
        ("سه ماهه", "پایش ولتاژ و ریپل خطوط تغذیه", "اندازه‌گیری ولتاژ نقاط تست TP1-TP6؛ اطمینان از ریپل کمتر از 5 mV rms.", "مولتی‌متر دیجیتال ۶.۵ دیجیت، اسیلوسکوپ"),
        ("شش ماهه", "خمیر و پد سیلیکونی انتقال حرارت", "بررسی انعطاف‌پذیری پد سیلیکونی؛ تعویض خمیر سیلیکون حرارتی فرسوده.", "ایزوپروپیل الکل ۹۹.۹٪ (IPA)، خمیر PTM7950"),
        ("سالانه", "بازرسی میکروسکوپی قلع و اتصالات BGA", "بررسی ترک‌های میکروسکوپی قلع BGA، اکسیداسیون پدها و ویسکرهای قلع.", "میکروسکوپ استریو ۲۰ برابر، لامپ UV")
    ]
    for row_data in fa_maint_data:
        row = fa_maint_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(fa_maint_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            make_bidi_p(p)
            p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
            run = p.add_run(val)
            make_bidi_run(run, font_name="Vazirmatn", size_pt=8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # Persian Section 2 - Altium Job 1 & Job 2
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۲. خروجی‌های آلتیوم Job 1 و Job 2 و جانمایی نقاط تست روی لایه‌های PCB")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "جهت اجرای عملیات تعمیرات برد در سطح آزمایشگاهی، کارشناسان فنی باید شماتیک مدار (Job 1) را با نقشه‌های مونتاژ "
        "لایه‌های رو و پشت برد (Job 2) تطبیق دهند. شکل‌های ۲.۱ و ۲.۲ جزئیات کامل نام نت‌ها، مسیرهای مسی و پدهای تست را نمایش می‌دهند:"
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=10)

    # Persian Figures Job1 & Job2
    add_figure(doc, "docs/images/altium_job1_schematic_checkpoints.png",
               "شکل ۲.۱: خروجی آلتیوم Job 1 — نقشه شماتیک نقاط تست، برچسب‌های نت، مدارات PMIC و رگولاتورهای LDO به همراه راهنمای تفکیک عیب.",
               width_inches=6.2, is_rtl=True)

    add_figure(doc, "docs/images/altium_job2_pcb_assembly_top_bottom.png",
               "شکل ۲.۲: خروجی آلتیوم Job 2 — نقشه‌های مونتاژ لایه‌های PCB: لایه رو (Job 2A) و لایه پشت (Job 2B) شامل ترک‌های فرکانس بالا و پدهای تست ولتاژ.",
               width_inches=6.2, is_rtl=True)

    # Persian Section 3 - Power Rails
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۳. عیب‌یابی خطوط تغذیه DC و جدول نقاط تست ولتاژ")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    fa_tp_tbl = doc.add_table(rows=1, cols=6)
    fa_tp_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    fa_tp_tbl.autofit = False
    set_table_borders(fa_tp_tbl, "B0C4DE", "4", "single")

    fa_tp_hdrs = ["نقطه تست", "نام خط تغذیه", "ولتاژ نامی", "تلرانس مجاز", "حداکثر ریپل", "علائم خرابی و ریشه عیب"]
    hdr_tp = fa_tp_tbl.rows[0]
    for i, h in enumerate(fa_tp_hdrs):
        cell = hdr_tp.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        make_bidi_p(p)
        p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        run = p.add_run(h)
        make_bidi_run(run, font_name="Vazirmatn", size_pt=8.5, bold=True, color_rgb=(255, 255, 255))

    fa_tp_data = [
        ("TP1", "1.0V Digital", "1.00 V", "±5% (0.95 - 1.05 V)", "< 5 mV rms", "قطع ارتباط لینک JESD204B و بوت نشدن هسته دیجیتال. بررسی آی‌سی ADP5054."),
        ("TP2", "1.3V Analog RF", "1.30 V", "±5% (1.24 - 1.36 V)", "< 3 mV rms", "نویز فاز شدید، اعوجاج میکسر و افت گین. بررسی رگولاتور کم‌نویز LT3045."),
        ("TP3", "1.3V Synth", "1.30 V", "±5% (1.24 - 1.36 V)", "< 2 mV rms", "قفل نشدن سینتی‌سایزر نوسان‌ساز محلی (RFPLL). بررسی فیلتر فریت FB3."),
        ("TP4", "1.8V Clock", "1.80 V", "±5% (1.71 - 1.89 V)", "< 8 mV rms", "جیتر شدید کلاک HMC7044 و قطع پالس‌های SYSREF. بررسی سلف سوئیچینگ L4."),
        ("TP5", "3.3V Main", "3.30 V", "±5% (3.14 - 3.46 V)", "< 15 mV rms", "عدم دسترسی به پورت سریال UART، EEPROM و باس I2C. بررسی مبدل DCDC اصلی."),
        ("TP6", "12.0V Input", "12.00 V", "±10% (10.8 - 13.2 V)", "< 50 mV rms", "خاموشی کامل برد؛ روشن نشدن LEDهای تغذیه. بررسی فیوز ورودی F1 (5A) و دیود TVS.")
    ]
    for row_data in fa_tp_data:
        row = fa_tp_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(fa_tp_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            make_bidi_p(p)
            p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
            run = p.add_run(val)
            make_bidi_run(run, font_name="Vazirmatn", size_pt=8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # Persian Section 4 - Step-by-Step RF Test & Measurement Instructions
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۴. دستورالعمل‌های گام‌به‌گام تست آزمایشگاهی و جدول نتایج اندازه‌گیری")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "جهت تضمین سلامت بردهای تعمیرشده، اجرای توالی تست‌های آزمایشگاهی زیر با استفاده از تجهیزات اندازه‌گیری دقیق الزامی است:\n\n"
        "■ دستورالعمل تست توان، خلوص طیفی و هارمونیک‌های فرستنده (TX):\n"
        "۱. اتصال پورت TX1_OUT از طریق پد تضعیف‌کننده دقیق ۲۰ دسی‌بل به ورودی ۵۰ اهم اسپکتروم آنالایزر.\n"
        "۲. تنظیم فرکانس نوسان‌ساز محلی (LO) روی ۲.۶ گیگاهرتز از طریق دستور لینوکس یا نرم‌افزار eLynxSDR.\n"
        "۳. ارسال سیگنال تک‌تن دیجیتال در گین بیشینه و قرائت توان حامل اصلی (+7.5 dBm) و کنترل تضعیف فرکانس تصویر (> 78 dBc) و نشتی کریر (< -82 dBc).\n\n"
        "■ دستورالعمل تست رقم نویز (NF)، حساسیت و خطینگی گیرنده (RX):\n"
        "۱. اتصال سیگنال ژنراتور کالیبره‌شده به پورت RX1_IN و تزریق سیگنال تک‌تن با توان -50 dBm.\n"
        "۲. قرائت دامنه دیجیتال در پنجره FFT نرم‌افزار جهت محاسبه گین تبدیل آنالوگ به دیجیتال.\n"
        "۳. اتصال منبع نویز کالیبره‌شده (346B) جهت اندازه‌گیری Y-Factor و استخراج رقم نویز (کمتر از 12.5 dB در فرکانس 2.6 GHz)."
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=9.5)

    add_figure(doc, "docs/images/rf_test_bench_setup_diagram.png",
               "شکل ۲.۳: دیاگرام اتصالات میز تست و اندازه‌گیری آزمایشگاهی برد ADRV9009 شامل اسپکتروم آنالایزر، سیگنال ژنراتور و همگام‌سازی کلاک ۱۰ مگاهرتز.",
               width_inches=6.2, is_rtl=True)

    # Persian Section 5 - BGA Rework
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۵. دستورالعمل ریورک و تعویض تراشه BGA فرستنده/گیرنده ADRV9009")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/adrv9009_bga_rework_thermal_profile.png",
               "شکل ۲.۴: پروفایل حرارتی استاندارد ۴ مرحله‌ای ریورک و لحیم‌کاری هوای گرم BGA بر اساس استاندارد IPC-7711/7721 با آلیاژ SAC305.",
               width_inches=6.0, is_rtl=True)

    # Persian Section 6 - Troubleshooting Flowchart
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۶. فلوچارت جامع عیب‌یابی و تفکیک خطای سخت‌افزاری برد")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/adrv9009_hardware_troubleshooting_flowchart.png",
               "شکل ۲.۵: فلوچارت گام‌به‌گام عیب‌یابی، تست خطوط تغذیه، پایش سلامت کلاک و تفکیک خطاهای سخت‌افزاری برد فرستنده/گیرنده ADRV9009.",
               width_inches=6.2, is_rtl=True)

    # Save DOCX
    docx_path = "docs/ADRV9009_Board_Maintenance_and_Repair_Manual.docx"
    doc.save(docx_path)
    print(f"Generated DOCX maintenance manual successfully: {docx_path}")

if __name__ == "__main__":
    build_maintenance_guide_document()
