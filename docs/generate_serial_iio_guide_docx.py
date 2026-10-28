#!/usr/bin/env python3
"""
ADRV9009 Serial /dev/ttyUSB0, Linux Terminal & IIO Command Guide Generator
Generates a comprehensive bilingual (English and Persian) DOCX and Markdown manual
documenting serial UART connection, Linux terminal commands, libiio toolset,
register read/write operations, and scripting automation for ADRV9009.
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
        "tip": ("006644", "EAF7F0", "COMMAND TIP" if not is_rtl else "نکته کاربردی دستورات"),
        "math": ("2B547E", "F0F4F8", "REGISTER ARCHITECTURE" if not is_rtl else "معماری ثبات‌ها")
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

def build_serial_iio_guide_document():
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
    hrun = hp.add_run("ADRV9009 Serial /dev/ttyUSB0 & IIO CLI Command Guide | eLynxSDR v0.0.2")
    hrun.font.name = "Arial"
    hrun.font.size = Pt(8.5)
    hrun.font.color.rgb = RGBColor(128, 128, 128)

    footer = sections[0].footer
    fp = footer.paragraphs[0]
    fp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    frun = fp.add_run("Confidential & Proprietary — Hardware Interface & CLI Automation Manual")
    frun.font.name = "Arial"
    frun.font.size = Pt(8.5)
    frun.font.color.rgb = RGBColor(128, 128, 128)

    # Title Block
    p_title = doc.add_paragraph()
    p_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_title.paragraph_format.space_before = Pt(12)
    p_title.paragraph_format.space_after = Pt(4)
    run_title = p_title.add_run("ADRV9009 Serial Port (/dev/ttyUSB0),\nLinux Terminal & IIO Command-Line Reference")
    run_title.font.name = "Arial"
    run_title.font.size = Pt(22)
    run_title.bold = True
    run_title.font.color.rgb = RGBColor(0, 51, 102)

    p_sub = doc.add_paragraph()
    p_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_sub.paragraph_format.space_after = Pt(14)
    run_sub = p_sub.add_run("Complete Developer Manual for Serial Console Navigation, iio_info, iio_attr, Memory-Mapped AXI Registers, and Automation Scripts\n[English & Persian Technical Documentation - راهنمای جامع پورت سریال و دستورات IIO]")
    run_sub.font.name = "Arial"
    run_sub.font.size = Pt(11)
    run_sub.font.color.rgb = RGBColor(90, 100, 110)

    # Metadata Table
    meta_table = doc.add_table(rows=4, cols=2)
    meta_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    meta_table.autofit = False
    set_table_borders(meta_table, "CCCCCC", "4", "single")

    meta_data = [
        ("UART Hardware Device", "Silicon Labs / FTDI USB-to-UART Bridge (/dev/ttyUSB0, 115200 8-N-1)"),
        ("Embedded Linux OS", "Analog Devices Kuiper Linux / PetaLinux on ARM Cortex-A53 / Cortex-A9"),
        ("IIO Subsystem Stack", "Linux Industrial I/O (IIO), libiio v0.24+, iio-tools, devmem2"),
        ("Document Version / Date", "v1.2.0 — October 2026 | Technical Reference for eLynxSDR System Developers")
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
    # SECTION 1: Serial Interface & Hardware Connection
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("1. Serial Interface Architecture & Terminal Connection (/dev/ttyUSB0)")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The ADRV9009 carrier board features an FTDI USB-to-UART bridge that provides a primary debug and control "
        "console to the embedded Zynq processor. When plugged into a Linux host PC, the port is typically enumerated "
        "as `/dev/ttyUSB0` (or `/dev/ttyUSB1`)."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_code_block(doc,
        "# 1. Verify USB-UART enumeration on Host Linux:\n"
        "$ dmesg | grep -i ttyUSB\n"
        "[ 1245.890] ftdi_sio 1-1.2:1.0: FTDI USB Serial Device converter detected\n"
        "[ 1245.891] usb 1-1.2: FTDI USB Serial Device converter now attached to ttyUSB0\n\n"
        "# 2. Grant permissions to access serial port:\n"
        "$ sudo chmod 666 /dev/ttyUSB0\n"
        "# (Or add user to dialout group: sudo usermod -aG dialout $USER)\n\n"
        "# 3. Connect via picocom (Recommended):\n"
        "$ picocom -b 115200 /dev/ttyUSB0\n\n"
        "# 4. Alternative: Connect via minicom:\n"
        "$ minicom -D /dev/ttyUSB0 -b 115200 -8 -N\n\n"
        "# 5. Alternative: Connect via screen:\n"
        "$ screen /dev/ttyUSB0 115200"
    )

    add_callout(doc,
        "Serial Port Standard Parameters:\n"
        "• Baud Rate: 115200 bps | Data Bits: 8 | Parity: None | Stop Bits: 1 (8-N-1)\n"
        "• Flow Control: None (RTS/CTS disabled, XON/XOFF disabled)\n"
        "• To exit picocom, press Ctrl+A followed by Ctrl+X. To exit screen, press Ctrl+A then K.",
        title="SERIAL PORT CONFIGURATION SUMMARY", callout_type="info")

    # -------------------------------------------------------------
    # SECTION 2: IIO Architecture & Topology Discovery
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("2. Linux Industrial I/O (IIO) Framework & Topology Discovery (iio_info)")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The Linux IIO subsystem abstracts the ADRV9009 transceiver into a set of standard devices, channels, and attributes. "
        "The `iio_info` utility scans and prints the entire device hierarchy."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_code_block(doc,
        "# Scan local IIO context (run inside target board serial console):\n"
        "root@analog:~# iio_info\n\n"
        "# Scan remote IIO context from Host PC over Ethernet:\n"
        "$ iio_info -u \"ip:192.168.1.10\"\n\n"
        "# Query specific device only (adrv9009-phy):\n"
        "$ iio_info -u \"ip:192.168.1.10\" -d adrv9009-phy"
    )

    p = doc.add_paragraph("Key IIO Devices in the ADRV9009 Hardware System:")
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)
    p.runs[0].bold = True

    iio_dev_table = doc.add_table(rows=1, cols=3)
    iio_dev_table.alignment = WD_TABLE_ALIGNMENT.CENTER
    iio_dev_table.autofit = False
    set_table_borders(iio_dev_table, "B0C4DE", "4", "single")

    hdr_row = iio_dev_table.rows[0]
    for i, h in enumerate(["IIO Device Name", "Hardware Subsystem", "Primary Function & Attributes"]):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 100, 100)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(9.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    dev_data = [
        ("adrv9009-phy", "ADRV9009 Transceiver Core", "RF LO tuning (altvoltage0), hardwaregain (attenuation/gain), ENSM state, QEC/LOL cals, temp sensor"),
        ("axi-adrv9009-tx-hpc", "FPGA DAC DMA & DDS Engine", "Cyclic DMA sample buffer playback, Direct Digital Synthesis (DDS) CW tones, phase/scale/frequency controls"),
        ("axi-adrv9009-rx-hpc", "FPGA ADC DMA Core", "High-speed baseband IQ capture from RX1 / RX2 channels, buffer streaming into userspace"),
        ("hmc7044", "Clock Distribution IC", "Generates DEV_CLK (122.88 MHz) and SYSREF pulses for JESD204B deterministic phase alignment"),
        ("led-count-iio / mwipcore0", "Custom FPGA DRFM Baseband IP", "Memory-mapped AXI registers at 0x43C30000: TH0 (amplify), PDW (Doppler en), INchann, THCW (Doppler NCO), DACseles")
    ]
    for dname, hsub, pfunc in dev_data:
        row = iio_dev_table.add_row()
        for i, val in enumerate([dname, hsub, pfunc]):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(iio_dev_table.rows)%2==0 else "F7FAFC")
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
    # SECTION 3: iio_attr Complete Command Reference
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("3. iio_attr Command Reference — Frequency, Gain, ENSM & Calibrations")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The `iio_attr` command allows reading and writing device and channel attributes directly from the command line."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    # 3.1 Frequency Tuning
    h2 = doc.add_heading(level=2)
    r = h2.add_run("3.1 RF Carrier Frequency Read & Write Commands")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    add_code_block(doc,
        "# --- 1. Read Current TX / RX Local Oscillator Carrier Frequency (in Hz) ---\n"
        "$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency\n"
        "2400000000\n\n"
        "# --- 2. Set TX / RX LO Carrier Frequency to 5.8 GHz (5,800,000,000 Hz) ---\n"
        "$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 5800000000\n\n"
        "# --- 3. Set RF Carrier to 915 MHz (915,000,000 Hz) ---\n"
        "$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 915000000\n\n"
        "# --- 4. Read RX Baseband Sampling Rate (in Hz) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i sampling_frequency\n"
        "122880000\n\n"
        "# --- 5. Read / Set RF Analog Bandwidth (in Hz) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i rf_bandwidth\n"
        "100000000\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i rf_bandwidth 200000000"
    )

    # 3.2 Hardware Gain & Attenuation
    h2 = doc.add_heading(level=2)
    r = h2.add_run("3.2 Hardware Gain & Attenuation Read & Write Commands")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    add_code_block(doc,
        "# --- 1. Read TX1 Digital Attenuation (in dB, negative scale) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain\n"
        "-10.000000 dB\n\n"
        "# --- 2. Set TX1 Attenuation to 0 dB (Maximum Output Power) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain 0.0\n\n"
        "# --- 3. Set TX1 Attenuation to 25.5 dB ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain -25.5\n\n"
        "# --- 4. Read RX1 Gain (in dB) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i hardwaregain\n"
        "20.000000 dB\n\n"
        "# --- 5. Set RX1 Gain to Maximum (+30 dB) in Manual Gain Control (MGC) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i gain_control_mode manual\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i hardwaregain 30.0\n\n"
        "# --- 6. Set RX1 to Automatic Gain Control (Fast Attack AGC) ---\n"
        "$ iio_attr -d adrv9009-phy -c voltage0 -i gain_control_mode fast_attack"
    )

    # 3.3 ENSM & Tracking Calibrations
    h2 = doc.add_heading(level=2)
    r = h2.add_run("3.3 Enable State Machine (ENSM), Calibrations & Temperature Monitoring")
    r.font.name = "Arial"
    r.font.size = Pt(13)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 102, 153)

    add_code_block(doc,
        "# --- 1. Query Current Enable State Machine (ENSM) State ---\n"
        "$ iio_attr -d adrv9009-phy ensm_mode\n"
        "radio_on\n\n"
        "# --- 2. Force Transceiver into 'sleep' or 'radio_on' State ---\n"
        "$ iio_attr -d adrv9009-phy ensm_mode sleep\n"
        "$ iio_attr -d adrv9009-phy ensm_mode radio_on\n\n"
        "# --- 3. Enable Internal Real-Time Tracking Calibrations ---\n"
        "$ iio_attr -d adrv9009-phy calibrate_rx_qec_en 1\n"
        "$ iio_attr -d adrv9009-phy calibrate_tx_qec_en 1\n"
        "$ iio_attr -d adrv9009-phy calibrate_tx_lol_en 1\n"
        "$ iio_attr -d adrv9009-phy calibrate_tx_lol_ext_en 1\n\n"
        "# --- 4. Read On-Chip Junction Temperature (in millidegrees Celsius) ---\n"
        "$ iio_attr -d adrv9009-phy -c temp0 in_temp0_input\n"
        "42350\n"
        "# (42350 m°C = 42.35 °C)"
    )

    # -------------------------------------------------------------
    # SECTION 4: Direct DDS CW Tone Generation
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("4. Direct Digital Synthesis (DDS) & Continuous Wave (CW) Tone Control")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The `axi-adrv9009-tx-hpc` FPGA core includes hardware DDS synthesizers allowing direct CW tone generation "
        "without consuming CPU cycles or streaming DMA buffers."
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_code_block(doc,
        "# --- 1. Generate 10 MHz CW Tone on TX1 (altvoltage0) at Full Scale (1.0) ---\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o frequency 10000000\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 1.0\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o phase 0\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 1\n\n"
        "# --- 2. Generate Two-Tone Intermodulation Signal (10 MHz + 12 MHz on TX1) ---\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o frequency 10000000\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 0.5\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage1 -o frequency 12000000\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage1 -o scale 0.5\n\n"
        "# --- 3. Disable DDS Tone Generation (Mute TX) ---\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 0\n"
        "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage1 -o raw 0"
    )

    # -------------------------------------------------------------
    # SECTION 5: High-Speed DMA Streaming (iio_writedev & iio_readdev)
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("5. High-Speed DMA Sample Streaming (iio_writedev & iio_readdev)")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "For arbitrary waveform playback (Spot noise, LFM chirps, Radar pulses) and continuous baseband capture, "
        "use `iio_writedev` and `iio_readdev`:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_code_block(doc,
        "# --- 1. Stream Arbitrary IQ Waveform to DAC Cyclic Buffer (Playback) ---\n"
        "$ iio_writedev -b 262144 -u \"ip:192.168.1.10\" axi-adrv9009-tx-hpc < files/spot/spot10mhz_200.txt\n\n"
        "# --- 2. Capture 1 Million Samples from ADC (RX1 + RX2) to File ---\n"
        "$ iio_readdev -b 65536 -s 1000000 -u \"ip:192.168.1.10\" axi-adrv9009-rx-hpc > rx_capture.raw\n\n"
        "# --- 3. Capture Continuous Stream with Non-Blocking Buffer ---\n"
        "$ iio_readdev -b 131072 -u \"ip:192.168.1.10\" axi-adrv9009-rx-hpc | python3 process_stream.py"
    )

    # -------------------------------------------------------------
    # SECTION 6: Direct Memory Mapped (AXI) Register Control
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("6. Direct Memory-Mapped (AXI) Register Control (0x43C30000 & devmem2)")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "The custom FPGA DRFM IP core (`led_count_ip_0` / `MUXDACS`) resides at AXI4-Lite base address `0x43C30000`. "
        "Registers can be accessed via `devmem2`, IIO debug channel attributes, or `iio_reg`:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    axi_tbl = doc.add_table(rows=1, cols=4)
    axi_tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    axi_tbl.autofit = False
    set_table_borders(axi_tbl, "B0C4DE", "4", "single")

    hdr_row = axi_tbl.rows[0]
    for i, h in enumerate(["Register Name", "Physical Address", "Bit Width", "Hardware Functional Description"]):
        cell = hdr_row.cells[i]
        set_cell_background(cell, "003366")
        set_cell_margins(cell, 80, 80, 100, 100)
        p = cell.paragraphs[0]
        run = p.add_run(h)
        run.bold = True
        run.font.name = "Arial"
        run.font.size = Pt(9.5)
        run.font.color.rgb = RGBColor(255, 255, 255)

    axi_regs = [
        ("REG_AMPLY (TH0)", "0x43C30100", "16-bit", "DRFM amplitude linear attenuation / scale factor (out_count1_th0_raw)"),
        ("REG_VGPO_ENABLE (PDW)", "0x43C30110", "32-bit", "DRFM Doppler modulation enable gate (1 = Enabled, 0 = Disabled)"),
        ("REG_VGPO_OFFSET (INchann)", "0x43C30118", "32-bit", "DRFM initial phase offset word for Doppler NCO"),
        ("REG_VGPO_STEP (THCW)", "0x43C3011C", "32-bit signed", "DRFM Doppler phase increment: thcw = round(fd * 2^32 / Fs)"),
        ("REG_DRFM_SELECT (DACseles)", "0x43C30138", "32-bit", "DAC Output Mux: 1 = DRFM radar echo; 0 = DMA sample playback / noise"),
        ("REG_TH1 / REG_TH2", "0x43C30140 / 44", "32-bit", "Secondary amplitude thresholds for automatic target triggering")
    ]
    for rname, raddr, rbits, rdesc in axi_regs:
        row = axi_tbl.add_row()
        for i, val in enumerate([rname, raddr, rbits, rdesc]):
            cell = row.cells[i]
            set_cell_background(cell, "FFFFFF" if len(axi_tbl.rows)%2==0 else "F7FAFC")
            set_cell_margins(cell, 50, 50, 80, 80)
            p = cell.paragraphs[0]
            run = p.add_run(val)
            run.font.name = "Arial"
            run.font.size = Pt(8.5)
            if i == 0:
                run.bold = True
                run.font.color.rgb = RGBColor(0, 51, 102)

    doc.add_paragraph().paragraph_format.space_after = Pt(6)

    add_code_block(doc,
        "# --- 1. Direct AXI Register Write via devmem2 (Inside Serial Console) ---\n"
        "# Select DRFM Echo Path (DACseles = 1 at 0x43C30138):\n"
        "root@analog:~# devmem2 0x43C30138 w 0x00000001\n\n"
        "# Set Doppler Frequency Step for +50 kHz shift at Fs = 122.88 MSPS (THCW = +1747627 at 0x43C3011C):\n"
        "root@analog:~# devmem2 0x43C3011C w 0x001AAAEA\n\n"
        "# Enable Doppler Modulation (PDW = 1 at 0x43C30110):\n"
        "root@analog:~# devmem2 0x43C30110 w 0x00000001\n\n"
        "# --- 2. High-Speed Sysfs Raw Attribute Access (Recommended Pure IIO) ---\n"
        "root@analog:~# echo 1 > /sys/bus/iio/devices/iio:device*/out_count15_dacseles_raw\n"
        "root@analog:~# echo 1747627 > /sys/bus/iio/devices/iio:device*/out_count4_thcw_raw\n\n"
        "# --- 3. Direct Transceiver SPI Register Read via iio_reg ---\n"
        "# Read Product ID SPI Register (Address 0x000):\n"
        "root@analog:~# iio_reg adrv9009-phy 0x000\n"
        "0x00"
    )

    # -------------------------------------------------------------
    # SECTION 7: Python Automation Scripts
    # -------------------------------------------------------------
    h1 = doc.add_heading(level=1)
    r = h1.add_run("7. Automation Scripts — Python (pylibiio & pyserial) & Bash")
    r.font.name = "Arial"
    r.font.size = Pt(16)
    r.bold = True
    r.font.color.rgb = RGBColor(0, 51, 102)

    p = doc.add_paragraph(
        "Below is a complete, standalone Python script using `pylibiio` to automate frequency sweeping and power profiling:"
    )
    p.runs[0].font.name = "Arial"
    p.runs[0].font.size = Pt(10)

    add_code_block(doc,
        "#!/usr/bin/env python3\n"
        "import iio\n"
        "import time\n\n"
        "# Connect to ADRV9009 over Gigabit Ethernet\n"
        "ctx = iio.Context('ip:192.168.1.10')\n"
        "phy = ctx.find_device('adrv9009-phy')\n"
        "tx_lo = phy.find_channel('altvoltage0', True)\n"
        "tx1 = phy.find_channel('voltage0', True)\n\n"
        "print('Starting Automated RF Frequency Sweep (1.0 GHz to 5.0 GHz)...')\n"
        "for freq_hz in range(1000000000, 5000000000 + 1, 200000000):\n"
        "    tx_lo.attrs['frequency'].value = str(freq_hz)\n"
        "    tx1.attrs['hardwaregain'].value = '-5.0'\n"
        "    actual_freq = int(tx_lo.attrs['frequency'].value)\n"
        "    temp_mc = int(phy.find_channel('temp0').attrs['in_temp0_input'].value)\n"
        "    print(f'Locked at: {actual_freq/1e6:.1f} MHz | Atten: -5.0 dB | Temp: {temp_mc/1e3:.1f} °C')\n"
        "    time.sleep(0.1)\n"
        "print('Sweep Completed Successfully.')"
    )

    # -------------------------------------------------------------
    # SECTION 8: Persian Translation (بخش فارسی مستندات)
    # -------------------------------------------------------------
    doc.add_page_break()

    p_fa_title = doc.add_paragraph()
    make_bidi_p(p_fa_title)
    p_fa_title.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_title.paragraph_format.space_before = Pt(16)
    p_fa_title.paragraph_format.space_after = Pt(4)
    r_fa_title = p_fa_title.add_run("راهنمای جامع کار با پورت سریال (/dev/ttyUSB0)،\nترمینال لینوکس و دستورات خط فرمان IIO در تراشه ADRV9009")
    make_bidi_run(r_fa_title, font_name="Vazirmatn", size_pt=20, bold=True, color_rgb=(0, 51, 102))

    p_fa_sub = doc.add_paragraph()
    make_bidi_p(p_fa_sub)
    p_fa_sub.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p_fa_sub.paragraph_format.space_after = Pt(14)
    r_fa_sub = p_fa_sub.add_run("مرجع کامل تنظیمات کنسول سریال، ابزارهای iio_info و iio_attr، کنترل ثبات‌های حافظه‌ای AXI، بافرهای DMA و اسکریپت‌های اتوماسیون")
    make_bidi_run(r_fa_sub, font_name="Vazirmatn", size_pt=11, color_rgb=(90, 100, 110))

    # Persian Section 1
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۱. ساختار ارتباط سریال و اتصال به ترمینال برد (ttyUSB0)")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    p = doc.add_paragraph()
    make_bidi_p(p)
    p.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = p.add_run(
        "برد پردازشی ترنسور ADRV9009 مجهز به مبدل داخلی FTDI USB-to-UART است که با اتصال کابل USB به رایانه میزبان لینوکس، "
        "یک پورت سریال مجازی با عنوان `/dev/ttyUSB0` ایجاد می‌کند. از طریق این پورت می‌توان به ترمینال خط فرمان (Bash) لینوکس امبدد "
        "مستقر بر روی پردازنده Zynq دسترسی پیدا کرده و کلیه دستورات کنترلی IIO، ثبات‌های حافظه و اسکریپت‌های آزمون را اجرا نمود."
    )
    make_bidi_run(r, font_name="Vazirmatn", size_pt=10)

    add_callout(doc,
        "تنظیمات استاندارد پورت سریال:\n"
        "• نرخ تبادل داده (Baud Rate): ۱۱۵۲۰۰ بیت بر ثانیه | بیت داده: ۸ | بیت توقف: ۱ | توازن (Parity): ندارد (8-N-1)\n"
        "• نرم‌افزارهای ارتباطی در لینوکس: `picocom -b 115200 /dev/ttyUSB0` یا `minicom` یا `screen`.\n"
        "• جهت خروج از محیط picocom کلیدهای ترکیبی Ctrl+A و سپس Ctrl+X را فشار دهید.",
        title="خلاصه مشخصات فنی ارتباط سریال", callout_type="info", is_rtl=True)

    # Persian Section 2
    h1_fa = doc.add_heading(level=1)
    make_bidi_p(h1_fa)
    h1_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
    r = h1_fa.add_run("۲. مرجع دستورات تنظیم فرکانس، گین و کالیبراسیون با iio_attr")
    make_bidi_run(r, font_name="Vazirmatn", size_pt=15, bold=True, color_rgb=(0, 51, 102))

    fa_cmds = [
        ("تنظیم فرکانس نوسان‌ساز محلی (Carrier Frequency)",
         "# خواندن فرکانس فعلی فرستنده و گیرنده بر حسب هرتز:\n"
         "$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency\n"
         "# تنظیم فرکانس روی ۲۴۰۰ مگاهرتز (۲.۴ گیگاهرتز):\n"
         "$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 2400000000\n"
         "# تنظیم فرکانس روی ۵.۸ گیگاهرتز:\n"
         "$ iio_attr -d adrv9009-phy -c altvoltage0 -o frequency 5800000000"),

        ("کنترل تضعیف دیجیتال فرستنده و گین گیرنده",
         "# خواندن تضعیف کانال فرستنده TX1 (مقیاس دسی‌بل منفی):\n"
         "$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain\n"
         "# تنظیم توان حداکثر فرستنده (تضعیف 0 dB):\n"
         "$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain 0.0\n"
         "# تنظیم ۲۰ دسی‌بل تضعیف بر روی فرستنده:\n"
         "$ iio_attr -d adrv9009-phy -c voltage0 -o hardwaregain -20.0\n"
         "# تنظیم گین دستی گیرنده RX1 روی ۲۵ دسی‌بل:\n"
         "$ iio_attr -d adrv9009-phy -c voltage0 -i gain_control_mode manual\n"
         "$ iio_attr -d adrv9009-phy -c voltage0 -i hardwaregain 25.0"),

        ("تولید تن تک‌فرکانس سینوسی با سینتی‌سایزر سخت‌افزاری DDS",
         "# تولید تن سینوسی ۱۰ مگاهرتز روی کانال TX1 با دامنه کامل:\n"
         "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o frequency 10000000\n"
         "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o scale 1.0\n"
         "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 1\n"
         "# خاموش کردن و قطع سیگنال تن خروجی DDS:\n"
         "$ iio_attr -d axi-adrv9009-tx-hpc -c altvoltage0 -o raw 0"),

        ("کنترل ثبات‌های پردازش بیس‌باند DRFM در آدرس حافظه 0x43C30000",
         "# فعال‌سازی مد رادار اکو DRFM در ثبات DACseles (آدرس 0x43C30138):\n"
         "root@analog:~# devmem2 0x43C30138 w 0x00000001\n"
         "# اعمال استپ فاز داپلر معادل ۵۰+ کیلوهرتز در ثبات THCW (آدرس 0x43C3011C):\n"
         "root@analog:~# devmem2 0x43C3011C w 0x001AAAEA\n"
         "# فعال‌سازی مدولاسیون شیفت داپلر در ثبات PDW (آدرس 0x43C30110):\n"
         "root@analog:~# devmem2 0x43C30110 w 0x00000001\n"
         "# ارسال تضعیف دیجیتال TH0 از طریق Sysfs با سرعت بالا:\n"
         "root@analog:~# echo 1000 > /sys/bus/iio/devices/iio:device*/out_count1_th0_raw")
    ]

    for title, cmd_block in fa_cmds:
        h2_fa = doc.add_heading(level=2)
        make_bidi_p(h2_fa)
        h2_fa.alignment = WD_ALIGN_PARAGRAPH.RIGHT
        r = h2_fa.add_run(title)
        make_bidi_run(r, font_name="Vazirmatn", size_pt=12.5, bold=True, color_rgb=(0, 102, 153))

        add_code_block(doc, cmd_block)

    # Save DOCX
    docx_path = "docs/ADRV9009_Serial_UART_and_IIO_Command_Guide.docx"
    doc.save(docx_path)
    print(f"Generated DOCX serial guide successfully: {docx_path}")

if __name__ == "__main__":
    build_serial_iio_guide_document()
