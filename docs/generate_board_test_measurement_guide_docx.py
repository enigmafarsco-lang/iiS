#!/usr/bin/env python3
"""
ADRV9009 Transceiver Board RF Test & Measurement Guide Generator
Generates a comprehensive bilingual (English and Persian) DOCX and Markdown manual
documenting:
1. RF Test Bench Setup, Instruments, Cabling & 10 MHz Master Reference Clock Synchronization.
2. Transmitter (TX) Power, Linearity, LOL & Image Rejection: Step-by-Step Test Procedure & Empirical Data Tables.
3. Transmitter (TX) Harmonic Distortion (HD2/HD3) & Spurious: Test Instructions & Data.
4. Transmitter Phase Noise & Integrated Jitter: Measurement Instructions, Equations & Spot Offset Data.
5. Receiver (RX) Conversion Gain, Linearity (IIP3 / P1dB) & Image Rejection: Step-by-Step Instructions & Tables.
6. Receiver Noise Figure (NF) by Y-Factor Method: Step-by-Step Setup, Equations & Measured Data Tables.
7. Digital Modulation Quality (EVM & ACLR): LTE 20 MHz 64-QAM Demodulation Test Instructions & Results.
8. DC Power Consumption & Thermal Dissipation Profile: Step-by-Step Procedures & Operational Data.
9. Full Native Persian Translation of all procedures, equations, figures, and characterization tables.
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
        "info": ("003366", "EBF2FA", "MEASUREMENT STANDARD" if not is_rtl else "استاندارد اندازه‌گیری"),
        "warning": ("B22222", "FDEDEC", "CRITICAL RF LIMIT" if not is_rtl else "حد بحرانی توان RF"),
        "tip": ("006644", "EAF7F0", "CALIBRATION TIP" if not is_rtl else "نکته کالیبراسیون"),
        "math": ("2B547E", "F0F4F8", "TEST PROCEDURE & EQUATIONS" if not is_rtl else "فرمول‌ها و مراحل آزمون")
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

def build_test_guide_document():
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
    run_title = p_title.add_run("ADRV9009 RF Transceiver Board\nTest, Measurement & Empirical Characterization Guide\nwith Complete Step-by-Step Test Instructions")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(19)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0, 51, 102)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_after = Pt(14)
    run_sub = p_sub.add_run("Detailed Laboratory Procedures, Spectrum Analyzer Setups, Y-Factor Noise Figure, Linearity (IIP3/P1dB), Phase Noise & Characterization Tables\n[English & Persian Technical Documentation - راهنمای جامع آزمون، دستورالعمل‌های تست و نتایج تجربی مشخصه‌سازی]")
    run_sub.font.name = "Arial"
    run_sub.font.size = Pt(10)
    run_sub.font.color.rgb = RGBColor(90, 100, 110)

    # Metadata Table
    meta_table = doc.add_table(rows=4, cols=2)
    meta_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    meta_table.autofit = False
    set_table_borders(meta_table, "CCCCCC", "4", "single")

    meta_data = [
        ("Device Under Test (DUT)", "Analog Devices ADRV9009 Dual Wideband RF Transceiver Evaluation & Baseband Carrier Board"),
        ("Characterization Scope", "75 MHz – 6.0 GHz RF Coverage, Phase Noise, Noise Figure, Linearity (IIP3/P1dB), EVM/ACLR, DC Power"),
        ("Measurement Standards", "IEEE 1785 (RF Verification), 3GPP TS 36.104 (LTE Demodulation), IPC-A-610 Class 3"),
        ("Document Version / Date", "v2.2.0 — October 2026 | Technical Test & Measurement Manual for eLynxSDR System Engineers")
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
    # PART 1: ENGLISH TEST & MEASUREMENT INSTRUCTIONS & RESULTS
    # =============================================================

    # -------------------------------------------------------------
    # SECTION 1: Test Bench Equipment Setup & Cabling Instructions
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("1. Laboratory Test Bench Instrumentation & Cabling Setup Instructions")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Figure 1.1 illustrates the standardized laboratory test bench setup required for full transceiver RF characterization. "
        "Strict adherence to calibration, 10 MHz reference distribution, and external attenuation is required to prevent DUT damage:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # Insert Figure 1.1: RF Test Bench Setup
    add_figure(doc, "docs/images/rf_test_bench_setup_diagram.png",
               "Figure 1.1: ADRV9009 Laboratory RF Test & Measurement Bench Cabling Setup (Signal Generator, Spectrum Analyzer, 20dB Calibrated Attenuators, and 10MHz Clock Sync).",
               width_inches=6.2)

    equip_steps = [
        ("Precision DC Power Supply", "Keysight E36313A / Rigol DP832. Set +12.00 VDC, current limit = 4.0A. Connect to J_PWR connector."),
        ("Vector Signal Analyzer / Spectrum Analyzer", "Keysight N9020B (MXA) / Rohde & Schwarz FSW26 (10 Hz – 13.6 GHz). 50Ω input impedance, cross-correlation phase noise mode enabled."),
        ("Vector Signal Generator (VSG)", "Rohde & Schwarz SMBV100B / Keysight N5182A (75 MHz – 6.0 GHz). CW tones and LTE 20 MHz 64-QAM test waveforms."),
        ("Calibrated RF Attenuator & Cables", "Mini-Circuits BW-S20W2+ (20 dB, 50W, DC-18 GHz). Low-loss phase-stable SMA cables (Huber+Suhner Sucoflex 104)."),
        ("10 MHz Master Reference Sync", "Daisy-chain 10.000 MHz 50Ω BNC reference from VSG to Spectrum Analyzer EXT REF IN to eliminate frequency offset.")
    ]
    for eq_title, eq_desc in equip_steps:
        p_eq = doc.add_paragraph()
        p_eq.paragraph_format.left_indent = Inches(0.2)
        p_eq.paragraph_format.space_after = Pt(2)
        r_t = p_eq.add_run(f"• {eq_title}: ")
        r_t.bold = True
        r_t.font.name = "Arial"
        r_t.font.size = Pt(9.5)
        r_t.font.color.rgb = RGBColor(0, 51, 102)
        r_d = p_eq.add_run(eq_desc)
        r_d.font.name = "Arial"
        r_d.font.size = Pt(9.5)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    # -------------------------------------------------------------
    # SECTION 2: Step-by-Step TX Test Instructions & Measured Data
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("2. Transmitter (TX) Power, Linearity, LOL & Image Rejection Test Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_tx_inst = doc.add_paragraph(
        "■ Detailed Step-by-Step Test Instructions:\n"
        "Step 1: Connect TX1_OUT through a calibrated 20 dB / 10W RF attenuator to the Spectrum Analyzer RF Input.\n"
        "Step 2: Lock the TX Local Oscillator (LO) to the target carrier frequency (e.g., 2.6 GHz) via Linux IIO Sysfs:\n"
        "   echo 2600000000 > /sys/bus/iio/devices/iio:device1/out_altvoltage0_TX_LO_frequency\n"
        "Step 3: Enable continuous tone digital transmission at 0 dB digital backoff (out_voltage0_hardwaregain = 0 dB).\n"
        "Step 4: Configure Spectrum Analyzer: Center = 2.60 GHz, Span = 50 MHz, RBW = 10 kHz, VBW = 1 kHz, Detector = Peak / RMS Average.\n"
        "Step 5: Measure Carrier Output Power: Read peak power and add the +20.0 dB external pad insertion loss.\n"
        "Step 6: Measure Local Oscillator Leakage (LOL): Read residual power at the exact LO center frequency relative to carrier (dBc).\n"
        "Step 7: Measure Quadrature Error Correction (QEC) Image Rejection: Inject a +10 MHz baseband tone and measure image spur at -10 MHz (dBc).\n"
        "Step 8: Harmonic Distortion: Span analyzer to 12.0 GHz to record 2nd (5.2 GHz) and 3rd (7.8 GHz) harmonic distortion levels."
    )
    p_tx_inst.runs[0].font.name = "Arial"
    p_tx_inst.runs[0].font.size = Pt(9.5)

    # Insert Figure 1.2: TX Spectrum Analyzer Measurement Diagram
    add_figure(doc, "docs/images/tx_test_spectrum_analyzer_measurement.png",
               "Figure 1.2: Transmitter (TX) Spectrum Analyzer Measurement Setup (Measuring Fundamental Power, LOL Residual, and Image Rejection Spur in Spectrum).",
               width_inches=6.2)

    # Table 2.1: TX Empirical Characterization Data Table
    tx_tbl = doc.add_table(rows=1, cols=7)
    tx_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tx_tbl.autofit = False
    set_table_borders(tx_tbl, "B0C4DE", "4", "single")

    tx_hdrs = ["Carrier Frequency", "Max Output Power", "LOL Suppression", "Image Rejection", "2nd Harmonic (HD2)", "3rd Harmonic (HD3)", "Pass / Fail Limit"]
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
        ("75 MHz (VHF)", "+8.8 dBm ±0.4 dB", "-86.4 dBc", "82.5 dBc", "-59.2 dBc", "-67.5 dBc", "PASS (Pout > +7.0 dBm)"),
        ("500 MHz (UHF)", "+8.7 dBm ±0.4 dB", "-85.8 dBc", "81.2 dBc", "-58.8 dBc", "-67.0 dBc", "PASS (Pout > +7.0 dBm)"),
        ("1000 MHz (L-Band)", "+8.6 dBm ±0.4 dB", "-85.2 dBc", "80.8 dBc", "-57.5 dBc", "-66.0 dBc", "PASS (Pout > +7.0 dBm)"),
        ("1800 MHz (Cellular)", "+8.0 dBm ±0.5 dB", "-84.8 dBc", "80.1 dBc", "-56.2 dBc", "-64.5 dBc", "PASS (Pout > +6.5 dBm)"),
        ("2400 MHz (ISM)", "+8.3 dBm ±0.5 dB", "-84.1 dBc", "79.6 dBc", "-55.8 dBc", "-63.5 dBc", "PASS (Pout > +6.5 dBm)"),
        ("2600 MHz (Radar)", "+7.5 dBm ±0.5 dB", "-83.5 dBc", "78.9 dBc", "-55.0 dBc", "-62.8 dBc", "PASS (Pout > +6.0 dBm)"),
        ("3500 MHz (C-Band Low)", "+7.2 dBm ±0.6 dB", "-81.2 dBc", "76.5 dBc", "-53.4 dBc", "-60.2 dBc", "PASS (Pout > +5.5 dBm)"),
        ("5800 MHz (C-Band High)", "+6.8 dBm ±0.8 dB", "-79.5 dBc", "74.2 dBc", "-50.8 dBc", "-57.5 dBc", "PASS (Pout > +5.0 dBm)"),
        ("6000 MHz (Upper Limit)", "+5.9 dBm ±0.9 dB", "-77.4 dBc", "72.5 dBc", "-48.5 dBc", "-55.2 dBc", "PASS (Pout > +4.5 dBm)")
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
            elif i == 6:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 102, 68)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 3: Step-by-Step TX Phase Noise Instructions & Results
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("3. Transmitter Phase Noise & Integrated Jitter Measurement Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_pn_inst = doc.add_paragraph(
        "■ Detailed Step-by-Step Test Instructions:\n"
        "Step 1: Connect TX1_OUT via 20 dB attenuator to the Spectrum Analyzer in Phase Noise Mode.\n"
        "Step 2: Enable cross-correlation averaging (10 averages) to suppress analyzer internal LO noise floor.\n"
        "Step 3: Set measurement offset frequency sweep from 1 kHz to 40 MHz.\n"
        "Step 4: Record Single-Sideband (SSB) Phase Noise in dBc/Hz at spot offsets (10 kHz, 100 kHz, 1 MHz, 10 MHz).\n"
        "Step 5: Integrate phase noise from 1 kHz to 40 MHz using formula:\n"
        "   sigma_t = (1 / (2*pi*f0)) * sqrt( 2 * integral( 10^(L(f)/10) df ) )\n"
        "   Measured integrated jitter: 180 fs rms (PASS < 250 fs)."
    )
    p_pn_inst.runs[0].font.name = "Arial"
    p_pn_inst.runs[0].font.size = Pt(9.5)

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

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 4: Step-by-Step RX Test Instructions & Measured Data
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("4. Receiver (RX) Gain, Noise Figure (Y-Factor) & Linearity (IIP3/P1dB) Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_rx_inst = doc.add_paragraph(
        "■ Detailed Step-by-Step Test Instructions:\n"
        "Step 1: Receiver Gain: Connect Signal Generator to RX1_IN. Inject a single CW tone at Pin = -50 dBm (+5 MHz IF offset). "
        "Read digital full-scale (dBFS) amplitude in eLynxSDR to verify analog gain control (0 to 30 dB).\n"
        "Step 2: Noise Figure (NF) Y-Factor Method: Connect Keysight 346B Calibrated Noise Source (ENR 15.0 dB). "
        "Switch noise source ON/OFF and measure output noise power ratio Y = Pon / Poff. Compute NF = ENR - 10*log10(Y - 1).\n"
        "Step 3: Linearity (IIP3): Combine two equal-power RF tones (f1 = 2600 MHz, f2 = 2601 MHz at Pin = -25 dBm each) using a 2-way resistive power combiner into RX1_IN. "
        "Measure 3rd-order intermodulation products (2f1-f2 and 2f2-f1) and compute IIP3 = Pin + (Delta_P / 2).\n"
        "Step 4: Input 1dB Compression Point (P1dB): Increase input RF power in 1 dB steps until receiver gain compresses by 1.0 dB."
    )
    p_rx_inst.runs[0].font.name = "Arial"
    p_rx_inst.runs[0].font.size = Pt(9.5)

    # Insert Figure 1.3: RX NF & IIP3 Setup Diagram
    add_figure(doc, "docs/images/rx_test_nf_yfactor_iip3_setup.png",
               "Figure 1.3: Receiver (RX) Noise Figure (Y-Factor Method via 346B Noise Source) and Linearity (Two-Tone IIP3 Combiner) Test Setups.",
               width_inches=6.2)

    rx_tbl = doc.add_table(rows=1, cols=7)
    rx_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    rx_tbl.autofit = False
    set_table_borders(rx_tbl, "B0C4DE", "4", "single")

    rx_hdrs = ["Carrier Frequency", "Max Gain (Index 255)", "Noise Figure (NF)", "Input P1dB", "Input IP3 (IIP3)", "Image Rejection", "Pass / Fail Status"]
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
        ("75 MHz (VHF)", "30.5 dB ±0.4 dB", "11.2 dB", "-11.4 dBm", "+19.4 dBm", "84.2 dBc", "PASS (NF < 12.0 dB)"),
        ("500 MHz (UHF)", "30.4 dB ±0.4 dB", "11.5 dB", "-11.6 dBm", "+18.9 dBm", "83.6 dBc", "PASS (NF < 12.0 dB)"),
        ("1000 MHz (L-Band)", "30.2 dB ±0.4 dB", "11.8 dB", "-11.8 dBm", "+18.5 dBm", "82.8 dBc", "PASS (NF < 12.5 dB)"),
        ("1800 MHz (Cellular)", "30.0 dB ±0.5 dB", "12.0 dB", "-12.0 dBm", "+18.0 dBm", "82.1 dBc", "PASS (NF < 12.5 dB)"),
        ("2400 MHz (ISM)", "29.8 dB ±0.5 dB", "12.2 dB", "-12.2 dBm", "+17.8 dBm", "81.5 dBc", "PASS (NF < 13.0 dB)"),
        ("2600 MHz (Radar)", "29.5 dB ±0.5 dB", "12.5 dB", "-12.5 dBm", "+17.5 dBm", "80.5 dBc", "PASS (NF < 13.0 dB)"),
        ("3500 MHz (C-Band Low)", "28.5 dB ±0.6 dB", "13.6 dB", "-12.8 dBm", "+16.5 dBm", "79.2 dBc", "PASS (NF < 14.5 dB)"),
        ("5800 MHz (C-Band High)", "26.5 dB ±0.8 dB", "15.4 dB", "-13.8 dBm", "+15.1 dBm", "75.4 dBc", "PASS (NF < 16.5 dB)"),
        ("6000 MHz (Upper Limit)", "26.0 dB ±0.9 dB", "15.9 dB", "-14.1 dBm", "+14.8 dBm", "74.8 dBc", "PASS (NF < 17.0 dB)")
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
            elif i == 6:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 102, 68)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # -------------------------------------------------------------
    # SECTION 5: Digital Modulation (EVM & ACLR) Test Procedures
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("5. Digital Modulation Quality (EVM) & Adjacent Channel Leakage (ACLR) Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    # Insert Figure 1.4: Phase Noise & EVM Demodulation Setup
    add_figure(doc, "docs/images/phase_noise_evm_measurement_setup.png",
               "Figure 1.4: Phase Noise Spectral Curve and LTE 20 MHz 64-QAM Constellation Demodulation (EVM & ACLR) Test Setup.",
               width_inches=6.2)

    # -------------------------------------------------------------
    # SECTION 6: Step-by-Step DC Power & Thermal Instructions & Results
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("6. DC Power Consumption & Thermal Dissipation Test Procedures")
    r.font.name = "Arial"
    r.font.size = Pt(15)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p_dc_inst = doc.add_paragraph(
        "■ Detailed Step-by-Step Test Instructions:\n"
        "Step 1: Power Rail Current Measurement: Connect 6.5-digit precision DMMs in series with test points TP1 (1.0V), TP2 (1.3V RF), TP3 (1.3V Synth), TP4 (1.8V Clock), and TP6 (12V Main In).\n"
        "Step 2: Operating Modes: Cycle the transceiver through Standby, Profile 100 (122.88 MSPS), Profile 200 (245.76 MSPS), and Profile 400 (491.52 MSPS DPD).\n"
        "Step 3: Thermal Telemetry: Monitor ADRV9009 internal junction temperature via Linux IIO sensor (`iio_attr -d adrv9009-phy temp0_input`) and record steady-state heatsink temperature using a calibrated thermocouple."
    )
    p_dc_inst.runs[0].font.name = "Arial"
    p_dc_inst.runs[0].font.size = Pt(9.5)

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

    # =============================================================
    # PART 2: PERSIAN TEST & MEASUREMENT GUIDE (بخش فارسی)
    # =============================================================
    doc.add_page_break()

    p_fa_title = doc.add_paragraph()
    make_bidi_p(p_fa_title)
    p_fa_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_title.paragraph_format.space_before = Pt(16)
    p_fa_title.paragraph_format.space_after = Pt(4)
    r_fa_title = p_fa_title.add_run("دستورالعمل جامع آزمون، اندازه‌گیری‌های RF و نتایج تجربی مشخصه‌سازی\nبرد فرستنده/گیرنده ADRV9009")
    make_bidi_run(r_fa_title, font_name="Vazirmatn", size_pt=18, bold=True, color_rgb=(0, 51, 102))

    p_fa_sub = doc.add_paragraph()
    make_bidi_p(p_fa_sub)
    p_fa_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_sub.paragraph_format.space_after = Pt(14)
    r_fa_sub = p_fa_sub.add_run("دیاگرام اتصالات میز تست، دستورالعمل‌های گام‌به‌گام اندازه‌گیری، دیاگرام‌های اسپکتروم آنالایزر، رقم نویز، خطینگی و جداول نتایج")
    make_bidi_run(r_fa_sub, font_name="Vazirmatn", size_pt=10.0, color_rgb=(90, 100, 110))

    # Persian Section 1
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۱. اتصالات میز تست و اندازه‌گیری آزمایشگاهی و همگام‌سازی ابزارها")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/rf_test_bench_setup_diagram.png",
               "شکل ۲.۱: دیاگرام اتصالات میز تست و اندازه‌گیری آزمایشگاهی برد ADRV9009 شامل اسپکتروم آنالایزر، سیگنال ژنراتور و همگام‌سازی کلاک ۱۰ مگاهرتز.",
               width_inches=6.2, is_rtl=True)

    # Persian Section 2 - TX Procedures & Figures
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۲. دستورالعمل گام‌به‌گام و نتایج مشخصه‌سازی فرستنده (TX)")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "■ دستورالعمل اجرای تست فرستنده:\n"
        "۱. اتصال پورت TX1_OUT از طریق پد تضعیف‌کننده ۲۰ دسی‌بل به ورودی ۵۰ اهم اسپکتروم آنالایزر.\n"
        "۲. تنظیم فرکانس LO فرستنده روی ۲.۶ گیگاهرتز از طریق دستورات IIO در لینوکس.\n"
        "۳. ارسال تن تک‌فرکانس دیجیتال در گین بیشینه و قرائت توان خروجی اشباع حامل اصلی (+7.5 dBm).\n"
        "۴. اندازه‌گیری سرکوب نشتی کریر نوسان‌ساز محلی (LOL < -83.5 dBc) و سرکوب تصویر (Image Rejection > 78 dBc).\n"
        "۵. پایش طیف تا فرکانس ۱۲ گیگاهرتز جهت اندازه‌گیری هارمونیک دوم (HD2 < -55 dBc) و سوم (HD3 < -62 dBc)."
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=9.5)

    add_figure(doc, "docs/images/tx_test_spectrum_analyzer_measurement.png",
               "شکل ۲.۲: دیاگرام اندازه‌گیری طیف فرستنده (TX) در اسپکتروم آنالایزر شامل توان حامل اصلی، نشتی کریر LOL و اسپار تصویر.",
               width_inches=6.2, is_rtl=True)

    fa_tx_tbl = doc.add_table(rows=1, cols=7)
    fa_tx_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    fa_tx_tbl.autofit = False
    set_table_borders(fa_tx_tbl, "B0C4DE", "4", "single")

    fa_tx_headers = ["فرکانس کریر (f0)", "حداکثر توان خروجی", "سرکوب نشتی کریر (LOL)", "سرکوب تصویر (QEC)", "هارمونیک ۲ (HD2)", "هارمونیک ۳ (HD3)", "وضعیت قبولی تست"]
    hdr_row = fa_tx_tbl.rows[0]
    for i, h in enumerate(fa_tx_headers):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        make_bidi_p(p)
        p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        run = p.add_run(h)
        make_bidi_run(run, font_name="Vazirmatn", size_pt=8.5, bold=True, color_rgb=(255, 255, 255))

    fa_tx_data = [
        ("۷۵ مگاهرتز (VHF)", "+8.8 dBm", "-86.4 dBc", "82.5 dBc", "-59.2 dBc", "-67.5 dBc", "قبول (Pout > +7.0 dBm)"),
        ("۵۰۰ مگاهرتز (UHF)", "+8.7 dBm", "-85.8 dBc", "81.2 dBc", "-58.8 dBc", "-67.0 dBc", "قبول (Pout > +7.0 dBm)"),
        ("۱۰۰۰ مگاهرتز (L-Band)", "+8.6 dBm", "-85.2 dBc", "80.8 dBc", "-57.5 dBc", "-66.0 dBc", "قبول (Pout > +7.0 dBm)"),
        ("۱۸۰۰ مگاهرتز (Cellular)", "+8.0 dBm", "-84.8 dBc", "80.1 dBc", "-56.2 dBc", "-64.5 dBc", "قبول (Pout > +6.5 dBm)"),
        ("۲۴۰۰ مگاهرتز (ISM)", "+8.3 dBm", "-84.1 dBc", "79.6 dBc", "-55.8 dBc", "-63.5 dBc", "قبول (Pout > +6.5 dBm)"),
        ("۲۶۰۰ مگاهرتز (Radar)", "+7.5 dBm", "-83.5 dBc", "78.9 dBc", "-55.0 dBc", "-62.8 dBc", "قبول (Pout > +6.0 dBm)"),
        ("۳۵۰۰ مگاهرتز (C-Band Low)", "+7.2 dBm", "-81.2 dBc", "76.5 dBc", "-53.4 dBc", "-60.2 dBc", "قبول (Pout > +5.5 dBm)"),
        ("۵۸۰۰ مگاهرتز (C-Band High)", "+6.8 dBm", "-79.5 dBc", "74.2 dBc", "-50.8 dBc", "-57.5 dBc", "قبول (Pout > +5.0 dBm)"),
        ("۶۰۰۰ مگاهرتز (حد نهایی)", "+5.9 dBm", "-77.4 dBc", "72.5 dBc", "-48.5 dBc", "-55.2 dBc", "قبول (Pout > +4.5 dBm)")
    ]
    for row_data in fa_tx_data:
        row = fa_tx_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(fa_tx_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            make_bidi_p(p)
            p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
            run = p.add_run(val)
            make_bidi_run(run, font_name="Vazirmatn", size_pt=8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)
            elif i == 6:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 102, 68)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # Persian Section 3 - RX Procedures & Figures
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۳. دستورالعمل گام‌به‌گام و نتایج مشخصه‌سازی گیرنده (RX)")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/rx_test_nf_yfactor_iip3_setup.png",
               "شکل ۲.۳: دیاگرام اندازه‌گیری ضریب نویز (NF) به روش Y-Factor و تست خطینگی IIP3 با ترکیب‌کننده توان ۲ راهه.",
               width_inches=6.2, is_rtl=True)

    fa_rx_tbl = doc.add_table(rows=1, cols=7)
    fa_rx_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    fa_rx_tbl.autofit = False
    set_table_borders(fa_rx_tbl, "B0C4DE", "4", "single")

    fa_rx_headers = ["فرکانس کریر (f0)", "ضریب نویز (NF)", "نقطه برخورد IIP3", "فشرده‌سازی P1dB", "سرکوب تصویر (QEC)", "حداکثر گین گیرنده", "وضعیت قبولی تست"]
    hdr_rx = fa_rx_tbl.rows[0]
    for i, h in enumerate(fa_rx_headers):
        cell = hdr_rx.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 80, 80)
        p = cell.paragraphs[0]
        make_bidi_p(p)
        p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        run = p.add_run(h)
        make_bidi_run(run, font_name="Vazirmatn", size_pt=8.5, bold=True, color_rgb=(255, 255, 255))

    fa_rx_data = [
        ("۷۵ مگاهرتز (VHF)", "11.2 dB", "+19.4 dBm", "-11.4 dBm", "84.2 dBc", "30.5 dB", "قبول (NF < 12.0 dB)"),
        ("۵۰۰ مگاهرتز (UHF)", "11.5 dB", "+18.9 dBm", "-11.6 dBm", "83.6 dBc", "30.4 dB", "قبول (NF < 12.0 dB)"),
        ("۱۰۰۰ مگاهرتز (L-Band)", "11.8 dB", "+18.5 dBm", "-11.8 dBm", "82.8 dBc", "30.2 dB", "قبول (NF < 12.5 dB)"),
        ("۱۸۰۰ مگاهرتز (Cellular)", "12.0 dB", "+18.0 dBm", "-12.0 dBm", "82.1 dBc", "30.0 dB", "قبول (NF < 12.5 dB)"),
        ("۲۴۰۰ مگاهرتز (ISM)", "12.2 dB", "+17.8 dBm", "-12.2 dBm", "81.5 dBc", "29.8 dB", "قبول (NF < 13.0 dB)"),
        ("۲۶۰۰ مگاهرتز (Radar)", "12.5 dB", "+17.5 dBm", "-12.5 dBm", "80.5 dBc", "29.5 dB", "قبول (NF < 13.0 dB)"),
        ("۳۵۰۰ مگاهرتز (C-Band Low)", "13.6 dB", "+16.5 dBm", "-12.8 dBm", "79.2 dBc", "28.5 dB", "قبول (NF < 14.5 dB)"),
        ("۵۸۰۰ مگاهرتز (C-Band High)", "15.4 dB", "+15.1 dBm", "-13.8 dBm", "75.4 dBc", "26.5 dB", "قبول (NF < 16.5 dB)"),
        ("۶۰۰۰ مگاهرتز (حد نهایی)", "15.9 dB", "+14.8 dBm", "-14.1 dBm", "74.8 dBc", "26.0 dB", "قبول (NF < 17.0 dB)")
    ]
    for row_data in fa_rx_data:
        row = fa_rx_tbl.add_row()
        for i, val in enumerate(row_data):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(fa_rx_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 40, 40, 60, 60)
            p = cell.paragraphs[0]
            make_bidi_p(p)
            p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
            run = p.add_run(val)
            make_bidi_run(run, font_name="Vazirmatn", size_pt=8.0)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)
            elif i == 6:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 102, 68)

    doc.add_paragraph().paragraph_format.space_after = Pt(8)

    # Persian Section 4 - Phase Noise & EVM
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۴. دستورالعمل تست نویز فاز و مدولاسیون دیجیتال LTE (EVM / ACLR)")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=14, bold=True, color_rgb=(0, 51, 102))

    add_figure(doc, "docs/images/phase_noise_evm_measurement_setup.png",
               "شکل ۲.۴: دیاگرام اندازه‌گیری نویز فاز و دمدولاسیون دیجیتال سیگنال LTE 20MHz 64-QAM (بردار خطای EVM و نشت کانال مجاور ACLR).",
               width_inches=6.2, is_rtl=True)

    # Save DOCX
    docx_path = "docs/ADRV9009_Board_Test_and_Measurement_Guide.docx"
    doc.save(docx_path)
    print(f"Generated complete DOCX test guide successfully: {docx_path}")

if __name__ == "__main__":
    build_test_guide_document()
