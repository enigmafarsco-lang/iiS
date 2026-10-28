#!/usr/bin/env python3
"""
ADRV9009 Transceiver Board Maintenance, Troubleshooting & Repair Manual Generator
Generates a comprehensive bilingual (English and Persian) DOCX and Markdown manual
documenting preventative maintenance, thermal management, power rail diagnostics,
BGA rework profiles, BOM spare parts, and troubleshooting flowcharts.
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
        elif line.startswith("$") or line.startswith(">") or line.startswith("root@"):
            run.font.color.rgb = RGBColor(220, 220, 170)
            run.bold = True
        else:
            run.font.color.rgb = RGBColor(220, 220, 220)

    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(4)

def build_maintenance_guide_document():
    doc = Document()

    # Page setup
    sections = doc.sections
    for section in sections:
        section.top_margin = Inches(0.8)
        section.bottom_margin = Inches(0.8)
        section.left_margin = Inches(0.85)
        section.right_margin = Inches(0.85)
        section.page_width = Inches(8.5)
        section.page_height = Inches(11.0)

    # Header and Footer
    header = sections[0].header
    hp = header.paragraphs[0]
    hp.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    hrun = hp.add_run("ADRV9009 Hardware Maintenance & Repair Manual | eLynxSDR v0.0.2")
    hrun.font.name = "Arial"
    hrun.font.size = Pt(8.5)
    hrun.font.color.rgb = RGBColor(128, 128, 128)

    footer = sections[0].footer
    fp = footer.paragraphs[0]
    fp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    frun = fp.add_run("Confidential & Proprietary — Hardware Maintenance, Diagnostics & Repair Manual")
    frun.font.name = "Arial"
    frun.font.size = Pt(8.5)
    frun.font.color.rgb = RGBColor(128, 128, 128)

    # Document Title Block
    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_title.paragraph_format.space_before = Pt(12)
    p_title.paragraph_format.space_after = Pt(4)
    run_title = p_title.add_run("ADRV9009 RF Transceiver Board\nHardware Maintenance, Troubleshooting & Repair Manual")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(22)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0, 51, 102)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_after = Pt(14)
    run_sub = p_sub.add_run("Preventative Maintenance Schedules, Power Rail Diagnostics, BGA Rework Profiles, and Spare Parts BOM\n[English & Persian Technical Documentation - راهنمای جامع نگهداری، عیب‌یابی و تعمیرات برد]")
    run_sub.font.name = "Arial"
    run_sub.font.size = Pt(11)
    run_sub.font.color.rgb = RGBColor(90, 100, 110)

    # Metadata Table
    meta_table = doc.add_table(rows=4, cols=2)
    meta_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    meta_table.autofit = False
    set_table_borders(meta_table, "CCCCCC", "4", "single")

    meta_data = [
        ("Hardware Under Repair", "ADRV9009 Dual Wideband RF Transceiver Evaluation & Baseband Carrier Board"),
        ("Maintenance Level", "Level 1 (Field Maintenance), Level 2 (Bench Testing), Level 3 (BGA Component Repair)"),
        ("Safety & Workmanship Standards", "IPC-7711/7721 (Rework & Repair), IPC-A-610 Class 3 (High Reliability Electronic Assemblies)"),
        ("Document Version / Date", "v1.0.0 — October 2026 | Technical Maintenance Manual for eLynxSDR System Engineers")
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

    # -------------------------------------------------------------
    # SECTION 1: Preventative Maintenance
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("1. Preventative Maintenance Schedules & Visual Inspection")
    r.font.name = "Arial"
    r.font.size = Pt(16)
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
    # SECTION 2: Power Supply Troubleshooting & Test Points
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("2. DC Power Supply Rail Diagnostics & Test Point Mapping")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Power supply failure is the most common cause of board unresponsiveness. Table 2.1 maps all primary DC voltage "
        "test points, nominal voltages, maximum allowable AC ripple, and diagnostic steps:"
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

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # -------------------------------------------------------------
    # SECTION 3: Clock Distribution & PLL Troubleshooting
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("3. Clock Distribution (HMC7044) & PLL Lock Troubleshooting")
    r.font.name = "Arial"
    r.font.size = Pt(16)
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

    add_callout(doc,
        "If PLL2 is unlocked (pll2_locked = 0):\n"
        "1. Check the 40 MHz TCXO / VCXO reference oscillator power rail (3.3V at pin 4).\n"
        "2. Inspect solder joints of the HMC7044 loop filter capacitors (C112, C115).\n"
        "3. Replace VCXO oscillator if frequency drift exceeds ±5 ppm (> 200 Hz at 40 MHz).",
        title="CLOCK PLL UNLOCK REPAIR ACTION", callout_type="tip")

    # -------------------------------------------------------------
    # SECTION 4: RF Frontend Component Repair & Replacement
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("4. RF Frontend Component Repair & SMT Replacement Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Excessive RF input power (> +10 dBm) can destroy the internal LNAs or external ESD protection diodes. "
        "Follow these SMT replacement guidelines when servicing RF frontend parts:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # Repair steps
    repair_steps = [
        ("1. ESD Diode & Balun De-soldering", "Apply no-clean tacky flux (Kester 959T / Amtech NC-559). Set hot air rework pencil to 320°C with 4 mm nozzle. Heat component for 25–35 seconds and gently lift with fine ESD tweezers (00-SA)."),
        ("2. Pad Site Dressing", "Use lead-free solder wick (Chemtronics Soder-Wick 1.5mm) soaked in flux at 340°C soldering iron tip to clean pads flat. Clean residual flux with 99.9% IPA and inspect pads under 20x magnification."),
        ("3. Replacement SMT Soldering", "Apply micro-dots of SAC305 solder paste using a 30-gauge dispensing needle. Align replacement component (0402/0201 or balun). Reflow using hot air at 310°C (30s) until solder fillets form on both terminals."),
        ("4. Post-Repair RF Verification", "Connect port to Network Analyzer (VNA) and measure Return Loss (S11 / S22). Verify Return Loss > 12 dB across 75 MHz – 6000 MHz.")
    ]
    for stitle, sdesc in repair_steps:
        p_st = doc.add_paragraph()
        p_st.paragraph_format.left_indent = Inches(0.2)
        p_st.paragraph_format.space_after = Pt(2)
        r_t = p_st.add_run(f"• {stitle}: ")
        r_t.bold = True
        r_t.font.name = "Arial"
        r_t.font.size = Pt(9.5)
        r_t.font.color.rgb = RGBColor(0, 51, 102)
        r_d = p_st.add_run(sdesc)
        r_d.font.name = "Arial"
        r_d.font.size = Pt(9.5)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # -------------------------------------------------------------
    # SECTION 5: BGA Rework Profile
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("5. ADRV9009 BGA IC Rework & Reflow Soldering Profile")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The ADRV9009 is packaged in a 196-ball Chip Scale BGA (12 mm x 12 mm, 0.8 mm pitch). "
        "Replacement requires an automated split-vision BGA Rework Station (e.g., Ersa IR/PL 550 / ZhuoMao ZM-R6200). "
        "Always bake the PCB at 125°C for 8 hours prior to reflow to prevent popcorn PCB delamination."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # BGA Table
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

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # -------------------------------------------------------------
    # SECTION 6: BOM & Spare Parts
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("6. Bill of Materials (BOM) & Spare Parts Catalog")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Table 6.1 lists the approved manufacturer part numbers for critical onboard active components:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

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
    # SECTION 7: Persian Translation Section (بخش فارسی مستندات)
    # -------------------------------------------------------------
    doc.add_page_break()

    p_fa_title = doc.add_paragraph()
    make_bidi_p(p_fa_title)
    p_fa_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_title.paragraph_format.space_before = Pt(16)
    p_fa_title.paragraph_format.space_after = Pt(4)
    r_fa_title = p_fa_title.add_run("دستورالعمل جامع نگهداری پیشگیرانه، عیب‌یابی و تعمیرات سخت‌افزاری\nبرد فرستنده/گیرنده ADRV9009")
    make_bidi_run(r_fa_title, font_name="Vazirmatn", size_pt=20, bold=True, color_rgb=(0, 51, 102))

    p_fa_sub = doc.add_paragraph()
    make_bidi_p(p_fa_sub)
    p_fa_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_sub.paragraph_format.space_after = Pt(14)
    r_fa_sub = p_fa_sub.add_run("مستندات استاندارد پایش دوره‌ای، عیب‌یابی خطوط تغذیه، تعویض قطعات SMT/BGA، پروفایل ریفلو و جدول قطعات یدکی")
    make_bidi_run(r_fa_sub, font_name="Vazirmatn", size_pt=11, color_rgb=(90, 100, 110))

    # Persian Section 1
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۱. زمان‌بندی نگهداری پیشگیرانه و بازرسی چشمی")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "نگهداری دوره‌ای و بازرسی فنی از عوامل حیاتی در تضمین پایداری و عملکرد طولانی‌مدت بردهای فرکانس بالا (RF) می‌باشد. "
        "با توجه به حساسیت فوق‌العاده تراشه‌های مدرن ترنسور رادیویی به گرد و غبار، دمای بالا، اکسیداسیون اتصالات و سایش کانکتورهای کواکسیال، "
        "رعایت زمان‌بندی زیر الزامی است:"
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=10)

    # Persian Maint Table
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

    # Persian Section 2 - Power Rails
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۲. عیب‌یابی خطوط تغذیه DC و جدول نقاط تست ولتاژ")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

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

    # Persian Section 3 - BGA Rework
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۳. دستورالعمل ریورک و تعویض تراشه BGA فرستنده/گیرنده ADRV9009")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "تراشه ADRV9009 در پکیج BGA با ۱۹۶ توپ قلع (ابعاد ۱۲ در ۱۲ میلی‌متر با گام ۰.۸ میلی‌متر) قرار دارد. "
        "جهت دمونتاژ و مونتاژ مجدد، رعایت مراحل زیر بر اساس استاندارد IPC-7711 الزامی است:\n"
        "۱. پخت برد (Baking): برد مدار چاپی پیش از هرگونه حرارت‌دهی باید به مدت ۸ ساعت در دمای ۱۲۵ درجه سانتی‌گراد در کوره رطوبت‌زدایی قرار گیرد تا از تاول‌زدگی (Popcorning) جلوگیری شود.\n"
        "۲. تنظیم پروفایل حرارتی: پیش‌گرمایش تدریجی تا ۱۷۰ درجه، ناحیه تثبیت فلاکس (Soak) تا ۲۰۰ درجه، و اوج حرارت مایع‌شدن قلع بدون‌سرب SAC305 در دمای ۲۳۵ تا ۲۴۵ درجه به مدت ۴۵ ثانیه.\n"
        "۳. تراز نوری اسپلیت‌ویژن: استفاده از سیستم منشوری دستگاه ریورک BGA جهت انطباق دقیق پدهای برد با پایه‌های تراشه.\n"
        "۴. بازرسی اشعه ایکس (X-Ray): تایید عدم اتصال کوتاه میان پایه‌ها و کنترل حباب‌های هوای داخل قلع به کمتر از ۱۵ درصد."
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=10)

    # Save DOCX
    docx_path = "docs/ADRV9009_Board_Maintenance_and_Repair_Manual.docx"
    doc.save(docx_path)
    print(f"Generated DOCX maintenance manual successfully: {docx_path}")

if __name__ == "__main__":
    build_maintenance_guide_document()
