#!/usr/bin/env python3
"""
ADRV9009 Transceiver Board Hardware Maintenance, Troubleshooting & Repair Manual Generator
Generates a comprehensive bilingual (English and Persian) DOCX and Markdown manual
documenting:
1. Preventative Maintenance & Visual Inspection Standards (IPC-A-610 Class 3).
2. Schematic Sheet-by-Sheet Fault Tracing & Check Points (Sheets 1 to 6).
3. PCB Top Layer & Bottom Layer Fault Tracing Maps & Test Point Coordinates.
4. DC Power Rail Diagnostics & Test Point Mapping (TP1 - TP6).
5. Clock Distribution (HMC7044) & PLL Lock Diagnostics.
6. BGA IC Rework & Reflow Soldering Profile (IPC-7711/7721).
7. Bill of Materials (BOM) for Critical Spare Parts.
8. End-to-End Hardware Troubleshooting & Fault Isolation Flowchart.
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

def add_figure(doc, img_path, caption, width_inches=6.2, is_rtl=False):
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
    run_title = p_title.add_run("ADRV9009 RF Transceiver Board\nHardware Maintenance, Troubleshooting & Repair Manual")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(19)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0, 51, 102)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_after = Pt(14)
    run_sub = p_sub.add_run("Altium OutJob Schematics (Sheets 1–6), PCB Top & Bottom Layer Fault Maps, Test Points TP1–TP6, BGA Rework & Flowcharts\n[English & Persian Technical Documentation - راهنمای جامع عیب‌یابی، نقشه‌های شماتیک و تعمیرات سخت‌افزاری برد]")
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
        ("Maintenance Level", "Level 1 (Field Maintenance), Level 2 (Bench Testing), Level 3 (BGA Component Repair)"),
        ("Safety & Workmanship Standards", "IPC-7711/7721 (Rework & Repair), IPC-A-610 Class 3 (High Reliability Electronic Assemblies)"),
        ("Document Version / Date", "v2.0.0 — October 2026 | Technical Maintenance Manual for eLynxSDR System Engineers")
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

    # -------------------------------------------------------------
    # SECTION 2: Schematic Sheet-by-Sheet Fault Tracing (Sheets 1 to 6)
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("2. Altium Designer Job 1 Output: Complete Schematic Prints (Sheets 1 to 6) & Fault Tracing Reference")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Technicians must reference the exact Altium Designer OutputJob 'Job1.PDF' schematic sheets corresponding to the suspected "
        "subsystem failure. Figure 1.0 displays the complete Job 1 Multi-Sheet compilation overview, followed by high-resolution "
        "drawings of each individual schematic sheet (Sheets 1 through 6) with highlighted check points and fault isolation procedures:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # Insert Figure 1.0: Altium Job 1 Multi-Sheet Schematic Overview
    add_figure(doc, "docs/images/altium_job1_schematic_checkpoints.png",
               "Figure 1.0: Altium Designer OutJob 'Job1.PDF' — Complete Multi-Sheet Schematic Prints (Sheets 1 to 6) Compilation Overview.",
               width_inches=6.2)

    # Sheet 1: Power Entry
    h2 = doc.add_heading(level=2)
    r = h2.add_run("2.1 Schematic Sheet 1: 12V Main Power Entry, Fuse F1 & TVS D1 Protection")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)
    add_figure(doc, "docs/images/schematic_sheet1_power_entry.png",
               "Figure 1.1: Schematic Sheet 1 — 12V Main DC Power Entry, Fuse F1 (5A), TVS Diode D1, and Check Point TP6.",
               width_inches=6.2)

    # Sheet 2: ADP5054 PMIC
    h2 = doc.add_heading(level=2)
    r = h2.add_run("2.2 Schematic Sheet 2: ADP5054 Quad Buck PMIC & Inductors L1–L4")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)
    add_figure(doc, "docs/images/schematic_sheet2_adp5054_pmic.png",
               "Figure 1.2: Schematic Sheet 2 — ADP5054 Quad Buck PMIC (U3), Inductors L1–L4, and Check Points TP1 (1.0V), TP5 (3.3V), TP4 (1.8V).",
               width_inches=6.2)

    # Sheet 3: LT3045 LDOs
    h2 = doc.add_heading(level=2)
    r = h2.add_run("2.3 Schematic Sheet 3: LT3045 Ultra-Low Noise RF & Synthesizer LDOs")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)
    add_figure(doc, "docs/images/schematic_sheet3_lt3045_rf_ldo.png",
               "Figure 1.3: Schematic Sheet 3 — LT3045 Ultra-Low Noise RF LDO (U4), Synth LDO (U5), FB3 Bead, and Check Points TP2 (1.3V RF) & TP3 (1.3V Synth).",
               width_inches=6.2)

    # Sheet 4: Clock & HMC7044
    h2 = doc.add_heading(level=2)
    r = h2.add_run("2.4 Schematic Sheet 4: Clock Generation (HMC7044) & 40MHz TCXO")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)
    add_figure(doc, "docs/images/schematic_sheet4_clock_hmc7044.png",
               "Figure 1.4: Schematic Sheet 4 — SiT5356 40MHz TCXO (Y1), HMC7044 Clock Jitter Cleaner (U2), and Differential DEV_CLK (TP_CLK_P/N).",
               width_inches=6.2)

    # Sheet 5: ADRV9009 Core
    h2 = doc.add_heading(level=2)
    r = h2.add_run("2.5 Schematic Sheet 5: ADRV9009 Core (U1) & JESD204B High-Speed SERDES")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)
    add_figure(doc, "docs/images/schematic_sheet5_adrv9009_jesd204b.png",
               "Figure 1.5: Schematic Sheet 5 — ADRV9009 (U1) 196-BGA Core, 100nF AC Coupling Capacitors, and JESD204B Interface.",
               width_inches=6.2)

    # Sheet 6: RF Front-End & Baluns
    h2 = doc.add_heading(level=2)
    r = h2.add_run("2.6 Schematic Sheet 6: RF Front-End Matching Baluns (T1–T4) & ESD Protection")
    r.font.name = "Arial"
    r.font.size = Pt(12)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)
    add_figure(doc, "docs/images/schematic_sheet6_rf_frontend_baluns.png",
               "Figure 1.6: Schematic Sheet 6 — Wideband RF Baluns T1–T4 (TCM1-63AX+), ESD Diodes D2–D5, and SMA Connectors.",
               width_inches=6.2)

    # -------------------------------------------------------------
    # SECTION 3: PCB Top & Bottom Layer Fault Maps
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("3. Altium Designer Job 2 Output: PCB Assembly & Fabrication Prints (Job2.PDF) & Fault Maps")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Technicians must reference the Altium Designer OutputJob 'Job2.PDF' PCB Assembly Drawings to physically locate all "
        "active components, test pads, annular check rings, and high-frequency decoupling capacitor arrays on the top and bottom layers of the PCB. "
        "Figure 1.7 provides the dual-layer assembly compilation overview, followed by individual detailed top and bottom fault maps:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # Insert Figure 1.7: Altium Job 2 PCB Assembly Overview
    add_figure(doc, "docs/images/altium_job2_pcb_assembly_top_bottom.png",
               "Figure 1.7: Altium Designer OutJob 'Job2.PDF' — PCB Assembly & Fabrication Prints (Top & Bottom Layers Compilation Overview).",
               width_inches=6.2)

    # Insert Figure 1.8: PCB Top Layer Fault Map
    add_figure(doc, "docs/images/pcb_top_layer_fault_map.png",
               "Figure 1.8: ADRV9009 PCB Top Layer Component & Fault Tracing Map (Locations of Fuse F1, Diode D1, ADRV9009 U1, HMC7044 U2, Baluns T1–T4, and Test Points TP1–TP6).",
               width_inches=6.2)

    # Insert Figure 1.9: PCB Bottom Layer Fault Map
    add_figure(doc, "docs/images/pcb_bottom_layer_fault_map.png",
               "Figure 1.9: ADRV9009 PCB Bottom Layer Component & Decoupling Map (ADP5054 PMIC U3, LT3045 LDOs U4/U5, Inductors L1–L4, and Underside BGA 0402 Decoupling Matrix).",
               width_inches=6.2)

    # -------------------------------------------------------------
    # SECTION 4: BGA Rework Profile
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("4. ADRV9009 BGA IC Rework & Reflow Soldering Profile (IPC-7711/7721)")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    add_figure(doc, "docs/images/adrv9009_bga_rework_thermal_profile.png",
               "Figure 1.10: ADRV9009 196-Ball CSP_BGA Rework & Soldering Thermal Profile (IPC-7711/7721 Standard SAC305 Lead-Free Alloy).",
               width_inches=6.0)

    # -------------------------------------------------------------
    # SECTION 5: Critical Spare Parts BOM
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("5. Bill of Materials (BOM) & Spare Parts Catalog")
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
        ("D1", "SMBJ12A", "Littelfuse", "DO-214AA (SMB)", "12V Unidirectional TVS Transient Voltage Suppressor"),
        ("D2 - D5", "PESD5V0U1BL", "Nexperia", "SOD-882 (1.0x0.6 mm)", "Ultra-Low Capacitance (0.5 pF) RF ESD Protection Diode"),
        ("F1", "0466005.NR", "Littelfuse", "SMD 1206 (3.2x1.6 mm)", "5A 125V Fast-Acting Surface Mount Fuse"),
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
    # SECTION 6: Troubleshooting Flowchart
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("6. End-to-End Hardware Troubleshooting & Fault Isolation Flowchart")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    add_figure(doc, "docs/images/adrv9009_hardware_troubleshooting_flowchart.png",
               "Figure 1.10: End-to-End ADRV9009 Hardware System Troubleshooting & Fault Isolation Flowchart.",
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
    r_fa_title = p_fa_title.add_run("دستورالعمل جامع نگهداری پیشگیرانه، عیب‌یابی و تعمیرات سخت‌افزاری\nبرد فرستنده/گیرنده ADRV9009")
    make_bidi_run(r_fa_title, font_name="Vazirmatn", size_pt=18, bold=True, color_rgb=(0, 51, 102))

    p_fa_sub = doc.add_paragraph()
    make_bidi_p(p_fa_sub)
    p_fa_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_sub.paragraph_format.space_after = Pt(14)
    r_fa_sub = p_fa_sub.add_run("نقشه‌های تفکیکی شماتیک (شیت‌های ۱ تا ۶)، نقشه‌های لایه‌های رو و پشت PCB، پروفایل حرارتی ریورک BGA و فلوچارت‌های عیب‌یابی")
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

    # Persian Section 2 - Schematic Sheets
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۲. خروجی Job 1 نرم‌افزار آلتیوم: نقشه‌های شیت‌به‌شیت شماتیک (شیت‌های ۱ تا ۶)")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    # Persian Job 1 Overview
    add_figure(doc, "docs/images/altium_job1_schematic_checkpoints.png",
               "شکل ۲.۰: نمای کلی خروجی Job1.PDF آلتیوم — مجموعه شیت‌های ۱ تا ۶ شماتیک و نقاط تست تشخیصی.",
               width_inches=6.2, is_rtl=True)

    # Persian Sheet 1
    add_figure(doc, "docs/images/schematic_sheet1_power_entry.png",
               "شکل ۲.۱: شیت ۱ شماتیک — مدار ورودی ۱۲ ولت، فیوز ورودی F1 (5A)، دیود هرزگرد TVS D1 و نقطه تست TP6.",
               width_inches=6.2, is_rtl=True)

    # Persian Sheet 2
    add_figure(doc, "docs/images/schematic_sheet2_adp5054_pmic.png",
               "شکل ۲.۲: شیت ۲ شماتیک — مبدل باک ۴ کاناله ADP5054 (U3)، سلف‌های قدرت L1 تا L4 و نقاط تست TP1 (1.0V) و TP5 (3.3V).",
               width_inches=6.2, is_rtl=True)

    # Persian Sheet 3
    add_figure(doc, "docs/images/schematic_sheet3_lt3045_rf_ldo.png",
               "شکل ۲.۳: شیت ۳ شماتیک — رگولاتورهای فوق‌کم‌نویز LT3045 (U4, U5)، فیلتر فریت FB3 و نقاط تست TP2 (1.3V RF) و TP3 (1.3V Synth).",
               width_inches=6.2, is_rtl=True)

    # Persian Sheet 4
    add_figure(doc, "docs/images/schematic_sheet4_clock_hmc7044.png",
               "شکل ۲.۴: شیت ۴ شماتیک — اسیلاتور ۴۰ مگاهرتز TCXO (Y1)، آی‌سی توزیع کلاک HMC7044 (U2) و نقاط تست کلاک تفاضلی TP_CLK_P/N.",
               width_inches=6.2, is_rtl=True)

    # Persian Sheet 5
    add_figure(doc, "docs/images/schematic_sheet5_adrv9009_jesd204b.png",
               "شکل ۲.۵: شیت ۵ شماتیک — تراشه مرکزی ADRV9009 (U1)، خازن‌های کوپلاژ AC خطوط JESD204B و اتصال کانکتور FMC.",
               width_inches=6.2, is_rtl=True)

    # Persian Sheet 6
    add_figure(doc, "docs/images/schematic_sheet6_rf_frontend_baluns.png",
               "شکل ۲.۶: شیت ۶ شماتیک — بالون‌های تطبیق امپدانس RF (T1 تا T4)، دیودهای محافظتی ESD و کانکتورهای SMA.",
               width_inches=6.2, is_rtl=True)

    # Persian Section 3 - PCB Fault Maps
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۳. خروجی Job 2 نرم‌افزار آلتیوم: نقشه‌های جانمایی و مونتاژ لایه‌های رو و پشت PCB")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    # Persian Job 2 Overview
    add_figure(doc, "docs/images/altium_job2_pcb_assembly_top_bottom.png",
               "شکل ۲.۷: نمای کلی خروجی Job2.PDF آلتیوم — نقشه‌های مونتاژ و جانمایی لایه‌های رو و پشت برد.",
               width_inches=6.2, is_rtl=True)

    add_figure(doc, "docs/images/pcb_top_layer_fault_map.png",
               "شکل ۲.۸: نقشه عیب‌یابی لایه روی برد (PCB Top Layer) شامل موقعیت فیوز F1، دیود TVS، تراشه‌های U1 و U2 و نقاط تست TP1 تا TP6.",
               width_inches=6.2, is_rtl=True)

    add_figure(doc, "docs/images/pcb_bottom_layer_fault_map.png",
               "شکل ۲.۹: نقشه عیب‌یابی لایه پشت برد (PCB Bottom Layer) شامل موقعیت آی‌سی ADP5054، رگولاتورهای LT3045 و ماتریس خازن‌های دیکوپلینگ BGA.",
               width_inches=6.2, is_rtl=True)

    # Persian Section 4 - BGA Rework
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۴. دستورالعمل ریورک و تعویض تراشه BGA فرستنده/گیرنده ADRV9009")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/adrv9009_bga_rework_thermal_profile.png",
               "شکل ۲.۱۰: پروفایل حرارتی استاندارد ۴ مرحله‌ای ریورک و لحیم‌کاری هوای گرم BGA بر اساس استاندارد IPC-7711/7721 با آلیاژ SAC305.",
               width_inches=6.0, is_rtl=True)

    # Persian Section 5 - Flowchart
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۵. فلوچارت جامع عیب‌یابی و تفکیک خطای سخت‌افزاری برد")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/adrv9009_hardware_troubleshooting_flowchart.png",
               "شکل ۲.۱۱: فلوچارت گام‌به‌گام عیب‌یابی، تست خطوط تغذیه، پایش سلامت کلاک و تفکیک خطاهای سخت‌افزاری برد فرستنده/گیرنده ADRV9009.",
               width_inches=6.2, is_rtl=True)

    # Save DOCX
    docx_path = "docs/ADRV9009_Board_Maintenance_and_Repair_Manual.docx"
    doc.save(docx_path)
    print(f"Generated complete DOCX maintenance manual successfully: {docx_path}")

if __name__ == "__main__":
    build_maintenance_guide_document()
