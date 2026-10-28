#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
eLynxSDR Software Persian User Guide Generator (RTL DOCX)
راهنمای جامع کاربری و مرجع فنی سامانه نرم‌افزاری eLynxSDR به زبان فارسی با قالب راست‌به‌چپ (RTL)
"""

import os
import sys
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.oxml import OxmlElement, parse_xml
from docx.oxml.ns import nsdecls, qn

def make_paragraph_rtl(p, align=WD_ALIGN_PARAGRAPH.RIGHT):
    """Set RTL layout and alignment on a paragraph."""
    p.alignment = align
    pPr = p._p.get_or_add_pPr()
    bidi = parse_xml(f'<w:bidi {nsdecls("w")}/>')
    pPr.append(bidi)

def make_run_rtl(run, font_name="B Nazanin", font_size=11, bold=False, italic=False, color_rgb=(0x26, 0x26, 0x26)):
    """Set RTL font properties on a text run."""
    run.font.name = font_name
    run.font.size = Pt(font_size)
    run.bold = bold
    run.italic = italic
    run.font.color.rgb = RGBColor(*color_rgb)
    rPr = run._r.get_or_add_rPr()
    rtl = parse_xml(f'<w:rtl {nsdecls("w")}/>')
    rPr.append(rtl)
    # Set complex script font
    rFonts = parse_xml(f'<w:rFonts {nsdecls("w")} w:cs="{font_name}" w:ascii="{font_name}" w:hAnsi="{font_name}"/>')
    rPr.append(rFonts)

def set_cell_background(cell, fill_hex):
    """Set background color of a table cell."""
    tcPr = cell._tc.get_or_add_tcPr()
    shd = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{fill_hex}"/>')
    tcPr.append(shd)

def set_cell_margins(cell, top=100, bottom=100, left=150, right=150):
    """Set internal cell margins in dxa."""
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

def make_table_rtl(table):
    """Set bidiVisual on table for RTL column ordering."""
    tblPr = table._tbl.tblPr
    bidiVisual = parse_xml(f'<w:bidiVisual {nsdecls("w")}/>')
    tblPr.append(bidiVisual)

def add_persian_heading(doc, text, level=1):
    """Add a styled RTL Persian heading."""
    p = doc.add_paragraph()
    make_paragraph_rtl(p, WD_ALIGN_PARAGRAPH.RIGHT)
    
    sizes = {1: 18, 2: 14, 3: 12}
    colors = {
        1: (0x00, 0x33, 0x66), # Deep Navy
        2: (0x2B, 0x54, 0x7E), # Slate Blue
        3: (0x00, 0x55, 0x55)  # Dark Teal
    }
    
    font_size = sizes.get(level, 12)
    color_rgb = colors.get(level, (0x00, 0x33, 0x66))
    
    p.paragraph_format.space_before = Pt(14 if level == 1 else 10)
    p.paragraph_format.space_after = Pt(6)
    
    run = p.add_run(text)
    make_run_rtl(run, font_name="B Nazanin", font_size=font_size, bold=True, color_rgb=color_rgb)
    return p

def add_persian_paragraph(doc, text, space_after=6):
    """Add an RTL Persian body paragraph."""
    p = doc.add_paragraph()
    make_paragraph_rtl(p, WD_ALIGN_PARAGRAPH.RIGHT)
    p.paragraph_format.line_spacing = 1.25
    p.paragraph_format.space_after = Pt(space_after)
    run = p.add_run(text)
    make_run_rtl(run, font_name="B Nazanin", font_size=11.5, bold=False, color_rgb=(0x22, 0x22, 0x22))
    return p

def add_persian_callout(doc, text, title="نکته مهم", callout_type="info"):
    """Add a styled Persian RTL callout box with right border accent."""
    colors = {
        "info": ("003366", "EBF2FA", (0x00, 0x33, 0x66)),
        "warning": ("B22222", "FDEDEC", (0xB2, 0x22, 0x22)),
        "tip": ("006644", "EAF7F0", (0x00, 0x66, 0x44)),
        "math": ("2B547E", "F0F4F8", (0x2B, 0x54, 0x7E))
    }
    border_hex, bg_hex, text_color = colors.get(callout_type, colors["info"])

    tbl = doc.add_table(rows=1, cols=1)
    tbl.alignment = WD_TABLE_ALIGNMENT.CENTER
    tbl.autofit = False
    make_table_rtl(tbl)

    cell = tbl.rows[0].cells[0]
    cell.width = Inches(6.5)
    set_cell_background(cell, bg_hex)
    set_cell_margins(cell, top=140, bottom=140, left=180, right=180)

    # Set right border thick for RTL callout
    tcPr = cell._tc.get_or_add_tcPr()
    borders = parse_xml(
        f'<w:tcBorders {nsdecls("w")}>'
        f'<w:top w:val="none"/>'
        f'<w:left w:val="none"/>'
        f'<w:bottom w:val="none"/>'
        f'<w:right w:val="single" w:sz="24" w:space="0" w:color="{border_hex}"/>'
        f'</w:tcBorders>'
    )
    tcPr.append(borders)

    p = cell.paragraphs[0]
    make_paragraph_rtl(p, WD_ALIGN_PARAGRAPH.RIGHT)
    p.paragraph_format.line_spacing = 1.2
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(2)

    run_title = p.add_run(f"[{title}] ")
    make_run_rtl(run_title, font_name="B Nazanin", font_size=11, bold=True, color_rgb=text_color)

    run_text = p.add_run(text)
    make_run_rtl(run_text, font_name="B Nazanin", font_size=11, bold=False, color_rgb=(0x22, 0x22, 0x22))

    p_after = doc.add_paragraph()
    p_after.paragraph_format.space_before = Pt(0)
    p_after.paragraph_format.space_after = Pt(6)

def add_persian_figure(doc, img_path, caption_text, width_inches=6.2):
    """Add image with Persian RTL centered caption."""
    if os.path.exists(img_path):
        p_img = doc.add_paragraph()
        p_img.alignment = WD_ALIGN_PARAGRAPH.CENTER
        p_img.paragraph_format.space_before = Pt(8)
        p_img.paragraph_format.space_after = Pt(4)
        run_img = p_img.add_run()
        run_img.add_picture(img_path, width=Inches(width_inches))

        p_cap = doc.add_paragraph()
        make_paragraph_rtl(p_cap, WD_ALIGN_PARAGRAPH.CENTER)
        p_cap.paragraph_format.space_before = Pt(2)
        p_cap.paragraph_format.space_after = Pt(12)
        run_cap = p_cap.add_run(caption_text)
        make_run_rtl(run_cap, font_name="B Nazanin", font_size=10, italic=True, bold=True, color_rgb=(0x55, 0x55, 0x55))
    else:
        p_warn = doc.add_paragraph(f"[تصویر یافت نشد: {img_path}]")
        make_paragraph_rtl(p_warn, WD_ALIGN_PARAGRAPH.RIGHT)

def format_persian_table(table, col_widths, headers, data):
    """Format RTL Persian table with deep navy header and alternating rows."""
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.autofit = False
    make_table_rtl(table)
    set_table_borders(table)

    # Header Row
    hdr_cells = table.rows[0].cells
    for i, title in enumerate(headers):
        hdr_cells[i].text = ""
        hdr_cells[i].width = col_widths[i]
        set_cell_background(hdr_cells[i], "003366")
        set_cell_margins(hdr_cells[i], top=100, bottom=100, left=120, right=120)
        p = hdr_cells[i].paragraphs[0]
        make_paragraph_rtl(p, WD_ALIGN_PARAGRAPH.CENTER)
        run = p.add_run(title)
        make_run_rtl(run, font_name="B Nazanin", font_size=10.5, bold=True, color_rgb=(0xFF, 0xFF, 0xFF))

    # Data Rows
    for r_idx, row_data in enumerate(data):
        row_cells = table.add_row().cells
        bg_col = "F8F9FA" if (r_idx % 2 == 1) else "FFFFFF"
        for c_idx, val in enumerate(row_data):
            row_cells[c_idx].text = ""
            row_cells[c_idx].width = col_widths[c_idx]
            set_cell_background(row_cells[c_idx], bg_col)
            set_cell_margins(row_cells[c_idx], top=80, bottom=80, left=120, right=120)
            p = row_cells[c_idx].paragraphs[0]
            make_paragraph_rtl(p, WD_ALIGN_PARAGRAPH.RIGHT)
            run = p.add_run(str(val))
            make_run_rtl(run, font_name="B Nazanin", font_size=10, bold=False, color_rgb=(0x22, 0x22, 0x22))

def build_persian_user_guide():
    doc = Document()

    # Geometry: 1 inch margins
    for section in doc.sections:
        section.top_margin = Inches(1.0)
        section.bottom_margin = Inches(1.0)
        section.left_margin = Inches(1.0)
        section.right_margin = Inches(1.0)

    # Cover Page
    p_sp = doc.add_paragraph()
    p_sp.paragraph_format.space_before = Pt(60)

    p_title = doc.add_paragraph()
    make_paragraph_rtl(p_title, WD_ALIGN_PARAGRAPH.CENTER)
    run_title = p_title.add_run("سامانه نرم‌افزاری eLynxSDR\nراهنمای جامع کاربری و مرجع فنی و عملیاتی")
    make_run_rtl(run_title, font_name="B Nazanin", font_size=24, bold=True, color_rgb=(0x00, 0x33, 0x66))

    p_sub = doc.add_paragraph()
    make_paragraph_rtl(p_sub, WD_ALIGN_PARAGRAPH.CENTER)
    p_sub.paragraph_format.space_before = Pt(12)
    p_sub.paragraph_format.space_after = Pt(36)
    run_sub = p_sub.add_run("پردازش سیگنال‌های پیشرفته راداری، موتور اگزیتر چندشکلی،\nشبیه‌ساز اهداف راداری DRFM و کنترل فرستنده/گیرنده رادیو شناسه پهن‌باند")
    make_run_rtl(run_sub, font_name="B Nazanin", font_size=13, bold=False, color_rgb=(0x55, 0x6B, 0x82))

    p_meta = doc.add_paragraph()
    make_paragraph_rtl(p_meta, WD_ALIGN_PARAGRAPH.CENTER)
    p_meta.paragraph_format.space_before = Pt(100)
    run_meta = p_meta.add_run(
        "نسخه نرم‌افزار: v0.0.2-Release (نگارش لینوکس / IIO)\n"
        "معماری تراشه گیرنده/فرستنده: Analog Devices ADRV9009 / Zynq UltraScale+ MPSoC\n"
        "شماره مرجع سند: eLynx-UG-2026-REV3-FA\n"
        "طبقه‌بندی سند: راهنمای فنی و دستورالعمل عملیاتی سیستم"
    )
    make_run_rtl(run_meta, font_name="B Nazanin", font_size=10.5, color_rgb=(0x66, 0x66, 0x66))

    doc.add_page_break()

    # Table of Contents
    add_persian_heading(doc, "فهرست مطالب", level=1)
    
    toc_data = [
        ("فصل ۱: نمای کلی و معماری سخت‌افزار و نرم‌افزار", "۱.۱ معماری سخت‌افزار و تراشه‌های RF\n۱.۲ معماری نرم‌افزار و خط پردازش چندریسمانی\n۱.۳ ماتریس ارتباطات زیرسیستم‌ها"),
        ("فصل ۲: راه‌اندازی و پیکربندی اتصال به سامانه", "۲.۱ کشف شبکه و مقداردهی اولیه بستر IIO\n۲.۲ انتخاب پروفایل فرستنده/گیرنده و سوئیچ بین پروفایل‌ها"),
        ("فصل ۳: زیرسیستم گیرنده و تحلیل طیف فرکانسی", "۳.۱ نمایشگر بلادرنگ طیف FFT و آبشار طیف‌نگار (Waterfall)\n۳.۲ تنظیمات عمومی، وضعیت‌های ENSM و کالیبراسیون\n۳.۳ پارامترهای پروفایل و فیلترهای دیجیتال FIR\n۳.۴ تنظیمات پردازش سیگنال طیف و توابع پنجره‌گذاری\n۳.۵ پیکربندی زنجیره فرستنده (TX) و تضعیف دیجیتال\n۳.۶ زنجیره گیرنده ناظر (OBSRX) و حلقه‌های ردیابی\n۳.۷ ماتریس سوئیچ‌های RF و مسیریابی سیگنال\n۳.۸ کالیبراسیون فرکانسی و منحنی‌های تصحیح دامنه/فاز"),
        ("فصل ۴: زیرسیستم شبیه‌ساز اهداف راداری (DRFM)", "۴.۱ اصول ریاضی و معادلات سینماتیک رادار و داپلر\n۴.۲ نگاشت ثبات‌های سخت‌افزاری AXI و تخصیص بیت‌ها\n۴.۳ شبیه‌سازی تاخیر فاصله و جبران تاخیر سخت‌افزاری پایه\n۴.۴ تغییر برخط پارامترهای DRFM بدون وقفه در ارسال سیگنال اگزیتر"),
        ("فصل ۵: زیرسیستم اگزیتر و تولید سیگنال‌های چندگانه", "۵.۱ تولید موج پیوسته (CW) تک‌تن با DDS\n۵.۲ تولید نویز نقطه‌ای (Spot Noise) برای جنگ الکترونیک\n۵.۳ تولید روبش فرکانسی خطی و پله‌ای (Sweep)\n۵.۴ تولید شکل‌موج‌های پالسی و ضربه (Impulse)\n۵.۵ تولید نویز پهن‌باند (WideBand) و نویز شانه‌ای (Bridge Noise)\n۵.۶ شبیه‌ساز همزمان ۵ هدف مجزای راداری (Multi-Target)\n۵.۷ تولید پالس‌های مدوله‌شده فرکانسی خطی (LFM Chirp)"),
        ("فصل ۶: پیکربندی سطح پایین و واحد کنترل سخت‌افزار", "۶.۱ ثبات‌های عمومی ADRV9009 و سنسورهای دمایی\n۶.۲ همگام‌سازی چندتراشه‌ای فاز (MCS)"),
        ("فصل ۷: دستورالعمل‌های عملیاتی و فرآیندهای گام‌به‌گام", "۷.۱ فرآیند استاندارد بوت و راه‌اندازی سامانه\n۷.۲ دستورالعمل شبیه‌سازی اکوی هدف راداری با DRFM\n۷.۳ تولید و ارسال شکل‌موج‌های اگزیتر و کنترل تضعیف توان"),
        ("فصل ۸: عیب‌یابی، کدهای خطا و مشخصات کامل فنی", "۸.۱ ماتریس جامع عیب‌یابی و راهکارهای حل مشکل\n۸.۲ جدول مشخصات فنی و پارامترهای عملیاتی سیستم")
    ]

    for title, subs in toc_data:
        p_t = doc.add_paragraph()
        make_paragraph_rtl(p_t, WD_ALIGN_PARAGRAPH.RIGHT)
        p_t.paragraph_format.space_before = Pt(6)
        p_t.paragraph_format.space_after = Pt(1)
        r_t = p_t.add_run(title)
        make_run_rtl(r_t, font_name="B Nazanin", font_size=11, bold=True, color_rgb=(0x00, 0x33, 0x66))

        p_s = doc.add_paragraph()
        make_paragraph_rtl(p_s, WD_ALIGN_PARAGRAPH.RIGHT)
        p_s.paragraph_format.space_before = Pt(0)
        p_s.paragraph_format.space_after = Pt(6)
        p_s.paragraph_format.right_indent = Inches(0.25)
        r_s = p_s.add_run(subs)
        make_run_rtl(r_s, font_name="B Nazanin", font_size=10, color_rgb=(0x55, 0x55, 0x55))

    doc.add_page_break()

    # CHAPTER 1
    add_persian_heading(doc, "فصل ۱: نمای کلی و معماری سخت‌افزار و نرم‌افزار", level=1)
    
    add_persian_paragraph(
        doc,
        "سامانه نرم‌افزاری eLynxSDR یک پلتفرم جامع و فوق‌پیشرفته در حوزه رادیو شناسه (SDR) و حافظه فرکانس رادیویی دیجیتال (DRFM) است "
        "که جهت شبیه‌سازی پیشرفته اهداف راداری، آزمایش سامانه‌های جنگ الکترونیک، پایش و تحلیل بلادرنگ طیف فرکانسی و سنتز شکل‌موج‌های پیچیده راداری طراحی و پیاده‌سازی شده است. "
        "این سامانه از طریق یک رابط گرافیکی یکپارچه و بهینه، امکان ارتباط با تراشه‌های فرستنده/گیرنده پیشرفته Analog Devices ADRV9009 و پردازنده‌های قدرتمند FPGA بر پایه معماری Xilinx Zynq UltraScale+ را فراهم می‌آورد."
    )

    add_persian_heading(doc, "۱.۱ معماری سخت‌افزار و تراشه‌های RF", level=2)
    add_persian_paragraph(
        doc,
        "بستر سخت‌افزاری سیستم مبتنی بر تراشه مجتمع ADRV9009 با ساختار 2T2R2ORX است که پهنای باند تیونینگ فرکانسی وسیعی از ۷۵ مگاهرتز تا ۶.۰ گیگاهرتز را با رزولوشن ۱ هرتز پوشش می‌دهد. "
        "این تراشه دارای دو کانال گیرنده مستقیم با پهنای باند آنی تا ۲۰۰ مگاهرتز، دو کانال فرستنده با پهنای باند تا ۴۵۰ مگاهرتز و دو کانال گیرنده ناظر (ORX) با پهنای باند تا ۴۵۰ مگاهرتز می‌باشد. "
        "انتقال داده‌های دیجیتال بین تراشه RF و پردازنده FPGA از طریق گذرگاه سریال پرسرعت چندبانده JESD204B انجام می‌پذیرد."
    )

    headers_hw = ["بخش سخت‌افزاری", "نوع پیاده‌سازی / قطعه", "مشخصات فنی و عملیاتی", "نوع گذرگاه / واسط"]
    col_w_hw = [Inches(1.8), Inches(1.5), Inches(2.0), Inches(1.2)]
    data_hw = [
        ["تراشه گیرنده/فرستنده RF", "ADRV9009 2T2R2ORX", "۷۵ مگاهرتز تا ۶.۰ گیگاهرتز، پهنای باند تا ۴۵۰ مگاهرتز", "SPI / JESD204B"],
        ["موتور پردازشی FPGA", "Zynq UltraScale+ / Kintex-7", "محاسبه بلادرنگ FFT، پایپ‌لاین DRFM، باتک‌ساز NCO", "AXI4-Lite / AXI-Stream"],
        ["موتور انتقال DAC DMA", "Cyclic High-Speed DMA", "تولید پیوسته و بدون وقفه سیگنال‌های اگزیتر", "AXI DMA / JESD204B TX"],
        ["گیرنده ناظر (Observation)", "Dual ORX (ORX1 / ORX2)", "پایش سیگنال با پویایی بالا و حلقه فیدبک DPD", "JESD204B RX / IIO"],
        ["واسط شبکه میزبان", "Gigabit Ethernet (TCP/IP)", "سرویس iiod لینوکس و پل دسترسی به ثبات‌های AXI", "libiio TCP / Socket LAN"],
        ["سوئیچ‌های فرکانس بالا", "Solid-State PIN / SPDT", "جابجایی مسیر در کسر میکروثانیه", "FPGA GPIO / AXI IO"]
    ]
    format_persian_table(doc.add_table(rows=1, cols=4), col_w_hw, headers_hw, data_hw)

    add_persian_heading(doc, "۱.۲ معماری نرم‌افزار و خط پردازش چندریسمانی", level=2)
    add_persian_paragraph(
        doc,
        "نرم‌افزار eLynxSDR با زبان C++ مدرن و فریم‌ورک‌های قدرتمند Qt و Qwt پیاده‌سازی شده و از یک معماری چندریسمانی (Multi-Threaded) بسیار کارآمد بهره می‌برد: \n"
        "• ریسمان ارتباطات (Communications Thread): مدیریت سوکت‌های ناهمگام پرسرعت TCP/UDP، خواندن/نوشتن ثبات‌های AXI و دریافت رخدادهای دورسنجی سخت‌افزار.\n"
        "• ریسمان داده‌برداری IIO (IIO Ingestion Thread): دریافت بدون وقفه بافرهای نمونه‌های مختلط IQ از درایور IIO لینوکس.\n"
        "• ریسمان محاسبات DSP (DSP Processing Thread): اجرای تبدیلات سریع فوریه (FFT)، اعمال توابع پنجره‌گذاری، میانگین‌گیری خطی/نمایی، آشکارسازی قله و ماندگاری تصویر.\n"
        "• موتور رندرینگ OpenGL / Qwt: ترسیم بلادرنگ شکل‌موج‌های طیف، آبشار دورنگ دوبعدی و سه‌بعدی با نرخ فریم بالا (تا ۶۰ فریم بر ثانیه).\n"
        "• موتور اگزیتر (Exciter Engine): سنتز تحلیلی شکل‌موج‌های استاندارد و پیچیده راداری با دقت ممیز شناور دوبل و بارگذاری بدون وقفه در بافر حلقوی DAC DMA."
    )

    doc.add_page_break()

    # CHAPTER 2
    add_persian_heading(doc, "فصل ۲: راه‌اندازی و پیکربندی اتصال به سامانه", level=1)
    
    add_persian_heading(doc, "۲.۱ پنجره تنظیمات اتصال و کشف شبکه", level=2)
    add_persian_paragraph(
        doc,
        "پس از اجرای نرم‌افزار، پنجره تنظیمات اتصال (Connection Dialog) به کاربر نمایش داده می‌شود تا آدرس آی‌پی سخت‌افزار و درگاه ارتباطی را تعیین نماید. "
        "این ارتباط از طریق درگاه شبکه گیگابیت اترنت و پروتکل استاندارد Industrial I/O (IIO) برقرار می‌گردد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-45-30.png", "شکل ۲.۱: پنجره پیکربندی اتصال شبکه به سخت‌افزار eLynxSDR")

    headers_c = ["المان کنترلی", "نوع پارامتر", "مقدار پیش‌فرض", "توضیحات و عملکرد عملیاتی"]
    col_w_c = [Inches(1.8), Inches(1.3), Inches(1.2), Inches(2.2)]
    data_c = [
        ["آدرس آی‌پی (IP Address)", "رشته IPv4", "192.168.1.10", "آدرس آی‌پی ثابت یا اختصاصی سخت‌افزار رادیو شناسه."],
        ["پورت ارتباطی (Port)", "عدد صحیح / TCP", "30718 / 50000", "درگاه کنترل سخت‌افزار جهت تبادل دستورات و ثبات‌های AXI."],
        ["دکمه جستجوی شبکه (Scan)", "کلید اقدام", "پویش خودکار", "جستجوی خودکار گره‌های فعال سخت‌افزار در زیرشبکه جاری."],
        ["دکمه‌های Connect / Cancel", "کلید اقدام", "N/A", "آغاز ارتباط IIO، باز کردن سوکت‌های فرمان و بارگذاری پنجره اصلی."]
    ]
    format_persian_table(doc.add_table(rows=1, cols=4), col_w_c, headers_c, data_c)

    add_persian_callout(doc, "پیش از اتصال، اطمینان حاصل کنید که کارت شبکه سیستم کامپیوتر میزبان بر روی همان رنج زیرشبکه با آی‌پی استاتیک (مانند 192.168.1.100 و Subnet Mask 255.255.255.0) تنظیم شده باشد.", "پیکربندی شبکه اترنت", "info")

    add_persian_heading(doc, "۲.۲ انتخاب پروفایل فرستنده/گیرنده و سوئیچ بین پروفایل‌ها", level=2)
    add_persian_paragraph(
        doc,
        "تراشه ADRV9009 بر اساس فایل‌های پروفایل مشخصه‌های کلاک و فیلترهای دیجیتال داخلی که در پردازنده ARM تعبیه شده پیکربندی می‌شود. "
        "کاربر در پنجره انتخاب پروفایل می‌تواند یکی از پروفایل‌های از پیش کالیبره‌شده متناسب با پهنای باند و نرخ نمونه‌برداری مورد نیاز را انتخاب نماید."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-00.png", "شکل ۲.۲: پنجره انتخاب و بارگذاری پروفایل فرستنده/گیرنده ADRV9009")

    headers_p = ["شناسه پروفایل", "نرخ نمونه‌برداری (MSPS)", "پهنای باند گیرنده (RX)", "پهنای باند TX / ORX", "کاربرد عملیاتی"]
    col_w_p = [Inches(1.8), Inches(1.3), Inches(1.1), Inches(1.1), Inches(1.2)]
    data_p = [
        ["پروفایل 100", "122.88 MSPS", "100.00 MHz", "200.00 MHz", "پایش استاندارد، پویایی دامنه بالا و پهنای باند باریک."],
        ["پروفایل 200", "245.76 MSPS", "200.00 MHz", "450.00 MHz", "پایش راداری پهن‌باند و شبیه‌سازی DRFM با نرخ میانی."],
        ["پروفایل 400", "491.52 MSPS", "200.00 MHz", "450.00 MHz", "پردازش پالس‌های فوق‌پهن‌باند و شبیه‌سازی دقیق چیرپ."]
    ]
    format_persian_table(doc.add_table(rows=1, cols=5), col_w_p, headers_p, data_p)

    add_persian_callout(doc, "در پروفایل‌های 100 و 200، نرم‌افزار به صورت خودکار نویز خام محیط را بدون دستکاری منحنی کالیبراسیون در کانال ORx نمایش می‌دهد تا کف نویز واقعی حفظ شود. در پروفایل 400، منحنی‌های کامل کالیبراسیون چندنقطه‌ای در پهنای باند اعمال می‌گردد.", "رفتار کالیبراسیون بر اساس پروفایل", "tip")

    doc.add_page_break()

    # CHAPTER 3
    add_persian_heading(doc, "فصل ۳: زیرسیستم گیرنده و تحلیل طیف فرکانسی", level=1)
    
    add_persian_heading(doc, "۳.۱ نمایشگر بلادرنگ طیف و آبشار طیف‌نگار (Waterfall)", level=2)
    add_persian_paragraph(
        doc,
        "محیط اصلی نرم‌افزار شامل دو بخش اصلی نمایشگر نمودار طیف فرکانسی (FFT Spectrum) در بالا و طیف‌نگار زمان‌فرکانس آبشاری (Waterfall) در پایین است. "
        "این پنجره به صورت کاملاً ریسپانسیو طراحی شده و امکان تغییر اندازه و بیشینه‌سازی (Maximize) در مانیتورهای Full HD، 2K و 4K را داراست."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-10.png", "شکل ۳.۱: نمای اصلی نمایشگر طیف فرکانسی، آبشار و زبانه تنظیمات عمومی (Global)")

    add_persian_paragraph(
        doc,
        "نوار ابزار بالای صفحه امکانات فوری و حیاتی را در اختیار کاربر قرار می‌دهد: \n"
        "• نمایش حوزه زمان (Time Domain): سوئیچ به حالت اسیلوسکوپ جهت مشاهده سیگنال‌های ولتاژ I و Q در حوزه زمان.\n"
        "• جدول CW (CW Table): نمایش فرکانس‌های پیوسته شناسایی‌شده، نسبت سیگنال به نویز (SNR) و توان قله.\n"
        "• نشانگرها (Markers): قرار دادن مارکرهای فرکانسی با خوانش تفاضلی دلتا.\n"
        "• آبشار دو بعدی و سه‌بعدی (Waterfall / Waterfall 3D): سوئیچ بین حالت رول پیوسته دورنگ و مش سه‌بعدی بر پایه OpenGL.\n"
        "• بزرگ‌نمایی، کوچک‌نمایی و مقیاس خودکار (Autoscale): تنظیم پویای محور افقی فرکانس و محور عمودی توان (-140 dBm تا +10 dBm).\n"
        "• تریگرهای کالیبراسیون (Calibration): اجرای بلادرنگ فرآیند کالیبراسیون فاز، گین و خطای مدهای تعادل کوادراتور."
    )

    add_persian_heading(doc, "۳.۲ تنظیمات عمومی، وضعیت‌های ENSM و نوسان‌ساز محلی (LO)", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Global پارامترهای اصلی فرستنده/گیرنده را کنترل می‌کند: \n"
        "• وضعیت ماشین حالات (Active ENSM): نمایش و تغییر حالت به radio_on، radio_off، sleep و alert.\n"
        "• نوسان‌ساز محلی (TRX Local Oscillator): تنظیم فرکانس مرکزی RF از ۷۵ مگاهرتز تا ۶۰۰۰ مگاهرتز با دقت ۱ هرتز.\n"
        "• سنسورهای دمایی (Sensors): پایش دمای پیوند سیلیکونی تراشه گیرنده ADRV9009 جهت محافظت در برابر حرارت بالا."
    )

    add_persian_heading(doc, "۳.۳ پارامترهای پروفایل و فیلترهای دیجیتال FIR", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Profile جزییات فیلترهای دیجیتال FIR، نرخ‌های درون‌یابی (Interpolation) و کاهش نرخ (Decimation) را در پروفایل فعال نمایش می‌دهد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-16.png", "شکل ۳.۲: زبانه مشخصات پروفایل و فیلترهای دیجیتال FIR تراشه")

    add_persian_heading(doc, "۳.۴ تنظیمات پردازش سیگنال طیف و توابع پنجره‌گذاری", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Spectrum پارامترهای موتور تبدیل فوریه سریع (FFT) را برای بهینه‌سازی رزولوشن فرکانسی و کاهش نشت طیفی تنظیم می‌کند."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-22.png", "شکل ۳.۳: زبانه تنظیمات پردازش سیگنال FFT، توابع پنجره‌گذاری و میانگین‌گیری")

    headers_s = ["پارامتر پردازشی", "گزینه‌های قابل انتخاب", "تنظیم پیشنهادی", "تاثیر ریاضی و کاربردی"]
    col_w_s = [Inches(1.8), Inches(1.5), Inches(1.2), Inches(2.0)]
    data_s = [
        ["طول FFT (FFT Size)", "1024 تا 65536 نقطه", "4096 / 8192", "تعیین تفکیک‌پذیری فرکانسی: دلتا f برابر Fs بر طول FFT است."],
        ["تابع پنجره (Window)", "Blackman-Harris، Flat-Top، Hann، Hamming", "Blackman-Harris", "تضعیف لوب‌های فرعی نشت طیفی (بیش از ۹۲ دسی‌بل در Blackman-Harris)."],
        ["تعداد میانگین‌گیری (Average)", "۱ (غیرفعال) تا ۱۲۸ فریم", "8 – 16", "کاهش واریانس نویز حرارتی و افزایش نسبت سیگنال به نویز بصری."],
        ["نگهداشت قله (Peak Hold)", "فعال / غیرفعال", "فعال (آشکارسازی پالس)", "حفظ بیشینه توان در هر سطل فرکانسی جهت شناسایی پالس‌های گذرا."],
        ["ماندگاری (Persistence)", "۰٪ تا ۱۰۰٪", "30% – 50%", "ایجاد افکت فسفری ماندگاری رد سیگنال بر روی نمایشگر."]
    ]
    format_persian_table(doc.add_table(rows=1, cols=4), col_w_s, headers_s, data_s)

    doc.add_page_break()

    add_persian_heading(doc, "۳.۵ پیکربندی زنجیره فرستنده (TX) و تضعیف دیجیتال", level=2)
    add_persian_paragraph(
        doc,
        "زبانه TX امکان فعال‌سازی کانال‌های فرستنده TX1 و TX2، تنظیم تضعیف دیجیتال به دسی‌بل و فعال‌سازی تقویت‌کننده توان (PA) را مدیریت می‌کند."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-26.png", "شکل ۳.۴: زبانه کنترل زنجیره فرستنده (TX) و تنظیمات تضعیف توان")

    add_persian_heading(doc, "۳.۶ زنجیره گیرنده ناظر (OBSRX) و حلقه‌های ردیابی", level=2)
    add_persian_paragraph(
        doc,
        "زبانه OBSRX دو کانال دیدبانی مستقل ORX1 و ORX2 را جهت پایش اسنیفر، تصحیح اعوجاج پیش از تقویت (DPD) و حلقه‌های ردیابی کالیبراسیون تنظیم می‌نماید."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-31.png", "شکل ۳.۵: زبانه تنظیمات گیرنده ناظر (OBSRX) و مدهای کنترل گین")

    add_persian_heading(doc, "۳.۷ ماتریس سوئیچ‌های RF و مسیریابی سیگنال", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Switch کنترل سوئیچ‌های الکترونیکی فرکانس بالای برد را برای تغییر مسیر سیگنال میان آنتن‌ها و مسیرهای لوپ‌بک کالیبراسیون فراهم می‌سازد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-36.png", "شکل ۳.۶: زبانه ماتریس سوئیچ‌های فرکانس بالای RF و مسیریابی سیگنال")

    add_persian_heading(doc, "۳.۸ کالیبراسیون فرکانسی و منحنی‌های تصحیح دامنه/فاز", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Calibration جداول و منحنی‌های تصحیح خطای گین و عدم تعادل فاز را در بازه کامل فرکانسی ۷۵ مگاهرتز تا ۶.۰ گیگاهرتز ذخیره و مدیریت می‌نماید."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-43.png", "شکل ۳.۷: زبانه مدیریت کالیبراسیون فرکانسی و جداول تصحیح فاز و گین")

    doc.add_page_break()

    # CHAPTER 4
    add_persian_heading(doc, "فصل ۴: زیرسیستم شبیه‌ساز اهداف راداری (DRFM)", level=1)
    
    add_persian_paragraph(
        doc,
        "زیرسیستم حافظه دیجیتال فرکانس رادیویی (DRFM) وظیفه شبیه‌سازی بلادرنگ اهداف راداری، اعمال تاخیر فاصله، انتقال فرکانسی داپلر و مدولاسیون دامنه اکو را بر عهده دارد. "
        "موتور پردازشی DRFM به صورت کاملاً سخت‌افزاری در داخل بستر FPGA پیاده‌سازی شده و امکان تست‌های سخت‌افزار در حلقه (HIL) را فراهم می‌آورد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-48.png", "شکل ۴.۱: زبانه پیکربندی شبیه‌ساز اهداف راداری و پارامترهای کینماتیک DRFM")

    add_persian_heading(doc, "۴.۱ اصول ریاضی و معادلات سینماتیک رادار و داپلر", level=2)
    add_persian_paragraph(
        doc,
        "موتور DRFM پارامترهای فیزیکی هدف را مطابق با روابط بنیادین امواج الکترومغناطیسی با دقت بالا محاسبه می‌نماید:"
    )

    math_fa = (
        "۱. رابطه تاخیر فاصله (Range Delay):\n"
        "   تاخیر زمانی رفت و برگشت پالس رادار (tau) برای هدفی در فاصله R برابر است با:\n"
        "      tau = (2 * R) / c\n"
        "   که در آن c سرعت انتشار امواج الکترومغناطیسی در خلاء (~299,792,458 m/s) است.\n"
        "   • کمینه فاصله شبیه‌سازی: R_min = 150 m ==> tau_min = 1.00069 us\n"
        "   • بیشینه فاصله شبیه‌سازی: R_max = 25,000 m (25 km) ==> tau_max = 166.782 us\n"
        "   • پله تفکیک فاصله: Delta_R = 10 m ==> Delta_tau = 66.713 ns\n"
        "   • جبران تاخیر سخت‌افزاری پایه: تاخیر ذاتی عبور از خطوط سخت‌افزاری (~80 ns) به صورت خودکار جبران می‌شود:\n"
        "      tau_total = tau + tau_baseline = (2 * R / c) + 80 ns\n\n"
        "۲. رابطه فرکانس داپلر (Doppler Frequency Shift):\n"
        "   هدفی با سرعت شعاعی v در برابر راداری با فرکانس حامل f0، جابجایی فرکانسی داپلر fd را تجربه می‌کند:\n"
        "      fd = (2 * v * f0) / c = (2 * v) / lambda\n"
        "   • بازه سرعت هدف: -1000 m/s تا +1000 m/s\n"
        "   • بازه فرکانس حامل: تا 6.0 گیگاهرتز (مثال: در f0 = 3 GHz و v = 150 m/s فرکانس داپلر fd = 3000 Hz خواهد بود)\n"
        "   • تفکیک‌پذیری فرکانس داپلر: 10 Hz\n\n"
        "۳. گام فاز اسیلاتور کنترل عددی FPGA (کلمه thcw):\n"
        "   فرکانس داپلر توسط آکومولاتور فاز ۳۲ بیتی NCO در کلاک Fs = 122.88 MHz سنتز می‌شود:\n"
        "      thcw = round( fd * (2^32) / Fs )\n"
        "   برای نمونه، به ازای fd = 1000 Hz در کلاک 122.88 MHz مقدار thcw = 34953 محاسبه می‌شود."
    )
    add_persian_callout(doc, math_fa, "معادلات سینماتیک رادار و گام فاز NCO", "math")

    add_persian_heading(doc, "۴.۲ نگاشت ثبات‌های سخت‌افزاری AXI و تخصیص بیت‌ها", level=2)
    add_persian_paragraph(
        doc,
        "ثبات‌های DRFM مستقیماً بر روی گذرگاه حافظه‌نگاشت AXI4-Lite نوشته می‌شوند. "
        "نکته حیاتی: تغییر مقادیر DRFM (مانند فاصله، داپلر، تضعیف TH0 و فعال‌سازی DRFM) هیچ‌گونه وقفه‌ای در سیگنال در حال پخش اگزیتر ایجاد نمی‌کند و بافر DMA متوقف نمی‌گردد."
    )

    headers_reg = ["نام ثبات", "آدرس پایه / آفست AXI", "میدان‌های بیت", "نوع داده", "عملکرد و توضیحات فنی"]
    col_w_reg = [Inches(1.5), Inches(1.3), Inches(1.0), Inches(1.0), Inches(1.7)]
    data_reg = [
        ["REG_DRFM_CTRL", "0x43C00000 / 0x00", "[0] DRFM_EN\n[1] DAC_SEL", "uint32_t", "بیت ۰: فعال‌ساز اصلی DRFM. بیت ۱: سوئیچ خروجی DAC (0=اگزیتر DMA، 1=اکوی DRFM)."],
        ["REG_DRFM_RANGE", "0x43C00000 / 0x04", "[31:0] DELAY_CYCLES", "uint32_t", "تعداد سیکل‌های کلاک تاخیر فاصله: N = round(tau * Fs)."],
        ["REG_DRFM_THCW", "0x43C00000 / 0x08", "[31:0] NCO_PHASE_INC", "uint32_t", "کلمه افزایش فاز ۳۲ بیتی thcw برای سنتز داپلر."],
        ["REG_DRFM_AMPL", "0x43C00000 / 0x0C", "[15:0] TH0_GAIN", "uint16_t", "ضریب مقیاس دامنه دیجیتال و تضعیف اکوی هدف (TH0)."],
        ["REG_DRFM_STATUS", "0x43C00000 / 0x10", "[0] FIFO_RDY\n[1] SYNC_OK", "uint32_t", "وضعیت بلادرنگ بافر حافظه DRFM و قفل فاز همگام‌سازی."]
    ]
    format_persian_table(doc.add_table(rows=1, cols=5), col_w_reg, headers_reg, data_reg)

    add_persian_callout(doc, "در زمان راه‌اندازی اولیه در صورت انتخاب مد DRFM، نرم‌افزار به صورت خودکار یک دنباله ۴ نمونه‌ای صفر ('0 0 0 0') به بافر DAC ارسال می‌کند تا کلاک DMA بدون پاک کردن فایل‌های اگزیتر فعال بماند.", "عملکرد بلادرنگ و بدون وقفه DRFM", "tip")

    doc.add_page_break()

    # CHAPTER 5
    add_persian_heading(doc, "فصل ۵: زیرسیستم اگزیتر و تولید سیگنال‌های چندگانه", level=1)
    
    add_persian_paragraph(
        doc,
        "زیرسیستم اگزیتر (Exciter Subsystem) امکان تولید انواع شکل‌موج‌های استاندار و پیچیده راداری و جنگ الکترونیک را با دقت محاسباتی ممیز شناور دوبل فراهم می‌آورد. "
        "نمونه‌های تولیدشده به کلمات ۱۶ بیتی علامت‌دار تبدیل شده و از طریق DMA بافر حلقوی به تراشه ارسال می‌گردند."
    )

    add_persian_heading(doc, "۵.۱ تولید موج پیوسته (CW) تک‌تن با DDS", level=2)
    add_persian_paragraph(
        doc,
        "زبانه CW امکان تولید تن سینوسی تک‌فرکانس با انحراف فرکانس و تضعیف دلخواه را فراهم می‌کند. سیگنال می‌تواند به صورت مستقل به خروجی‌های Voltage 0/1 (TX1) و Voltage 2/3 (TX2) هدایت شود."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-46-58.png", "شکل ۵.۱: زبانه اگزیتر موج پیوسته (CW) تک‌تن")

    add_persian_heading(doc, "۵.۲ تولید نویز نقطه‌ای (Spot Noise) برای جنگ الکترونیک", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Spot Noise نویز گوسی باریک‌باند با پهنای باند تنظیمی (۱ تا ۵۰ مگاهرتز) و جابجایی فرکانسی تولید می‌کند که برای ایجاد تداخل و جمر هدفمند کاربرد دارد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-02.png", "شکل ۵.۲: زبانه اگزیتر نویز نقطه‌ای (Spot Noise)")

    add_persian_heading(doc, "۵.۳ تولید روبش فرکانسی خطی و پله‌ای (Sweep)", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Sweep سیگنال روبش فرکانسی را با تنظیم فرکانس آغاز و پایان، زمان روبش بر حسب میلی‌ثانیه و نوع روبش خطی یا پله‌ای سنتز می‌نماید."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-06.png", "شکل ۵.۳: زبانه اگزیتر روبش فرکانسی (Sweep)")

    doc.add_page_break()

    add_persian_heading(doc, "۵.۴ تولید شکل‌موج‌های پالسی و ضربه (Impulse)", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Impulse پالس‌های فرکانس رادیویی با پهنای پالس دقیق (Tp) بر حسب میکروثانیه، دوره تناوب پالس (PRI) و شکل‌دهی پوش دامنه را سنتز می‌کند."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-10.png", "شکل ۵.۴: زبانه اگزیتر تولید شکل‌موج‌های پالسی و ضربه (Impulse)")

    add_persian_heading(doc, "۵.۵ تولید نویز پهن‌باند (WideBand) و نویز شانه‌ای (Bridge Noise)", level=2)
    add_persian_paragraph(
        doc,
        "• نویز پهن‌باند (WideBand): تولید نویز یکنواخت تصادفی در پهنای باندهای وسیع تا ۲۰۰ مگاهرتز با کنترل دقیق توان خروجی (dBm).\n"
        "• نویز شانه‌ای (Bridge Noise): تولید نویز چندحاملی شانه‌ای با فواصل فرکانسی مشخص جهت تست چندکاناله گیرنده‌ها."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-14.png", "شکل ۵.۵: زبانه اگزیتر تولید نویز پهن‌باند (WideBand)")
    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-18.png", "شکل ۵.۶: زبانه اگزیتر تولید نویز شانه‌ای (Bridge Noise)")

    doc.add_page_break()

    add_persian_heading(doc, "۵.۶ شبیه‌ساز همزمان ۵ هدف مجزای راداری (Multi-Target)", level=2)
    add_persian_paragraph(
        doc,
        "زبانه Multi-Target امکان سنتز همزمان تا ۵ هدف مجزا و مستقل راداری را با قابلیت تعیین نوع شکل‌موج (Spot, CW, LFM)، پهنای باند و جابجایی فرکانس داپلر برای هر کانال فراهم می‌سازد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-24.png", "شکل ۵.۷: زبانه شبیه‌ساز همزمان ۵ هدف مجزای راداری (Multi-Target)")

    headers_mt = ["کانال هدف", "نوع شکل‌موج", "بازه پهنای باند", "بازه جابجایی داپلر", "کاربرد عملیاتی"]
    col_w_mt = [Inches(1.4), Inches(1.4), Inches(1.2), Inches(1.2), Inches(1.3)]
    data_mt = [
        ["هدف ۱ (Target 1)", "Spot / CW / LFM", "1.0 تا 50.0 MHz", "-50.0 تا +50.0 MHz", "اکوی اصلی بدنه / هدف پیشرو"],
        ["هدف ۲ (Target 2)", "Spot / CW / LFM", "1.0 تا 50.0 MHz", "-50.0 تا +50.0 MHz", "پرواز دسته‌جمعی / هدف ثانویه"],
        ["هدف ۳ (Target 3)", "Spot / CW / LFM", "1.0 تا 50.0 MHz", "-50.0 تا +50.0 MHz", "ابر چف (Chaff) / توده کلاتر"],
        ["هدف ۴ (Target 4)", "Spot / CW / LFM", "1.0 تا 50.0 MHz", "-50.0 تا +50.0 MHz", "فریب‌دهنده هماهنگ (Decoy)"],
        ["هدف ۵ (Target 5)", "Spot / CW / LFM", "1.0 تا 50.0 MHz", "-50.0 تا +50.0 MHz", "پرتابه سرعت بالا / تهدید موشکی"]
    ]
    format_persian_table(doc.add_table(rows=1, cols=5), col_w_mt, headers_mt, data_mt)

    add_persian_heading(doc, "۵.۷ تولید پالس‌های مدوله‌شده فرکانسی خطی (LFM Chirp)", level=2)
    add_persian_paragraph(
        doc,
        "زبانه LFM پالس‌های راداری چیرپ با فرکانس شروع (Start MHz)، پهنای باند چیرپ (BW MHz) و مدت زمان پالس (T us) را مطابق با رابطه زیر تولید می‌نماید: \n"
        "   s(t) = exp( j * 2*pi * ( f_start * t + (B / (2*T)) * t^2 ) ) ,  0 <= t <= T"
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-29.png", "شکل ۵.۸: زبانه اگزیتر تولید پالس‌های مدوله‌شده فرکانسی خطی (LFM Chirp)")

    doc.add_page_break()

    # CHAPTER 6
    add_persian_heading(doc, "فصل ۶: پیکربندی سطح پایین و واحد کنترل سخت‌افزار", level=1)
    
    add_persian_paragraph(
        doc,
        "منوی Configs دسترسی مستقیم به ثبات‌های داخلی تراشه ADRV9009، پایش سنسورهای دمایی و اجرای همگام‌سازی چندتراشه‌ای (MCS) را میسر می‌سازد."
    )

    add_persian_figure(doc, "pictures/Screenshot from 2026-10-28 16-47-35.png", "شکل ۶.۱: پنجره تنظیمات سطح پایین سخت‌افزار ADRV9009 و همگام‌سازی فاز")

    headers_ctrl = ["بلوک کنترلی", "ثبات‌ها و پارامترها", "مقدار پایش", "عملکرد عملیاتی"]
    col_w_ctrl = [Inches(1.8), Inches(1.6), Inches(1.2), Inches(1.9)]
    data_ctrl = [
        ["تنظیمات عمومی (Global)", "حالات ENSM، مسیر پروفایل", "radio_on", "تعیین وضعیت کاری فرستنده/گیرنده."],
        ["نوسان‌ساز محلی (LO)", "ثبات‌های سنتز کننده PLL", "1800.000 MHz", "تنظیم فرکانس مرکزی میکسر RF."],
        ["سنسورهای حرارتی", "ADRV9009 PHY Temp, AD7291 Temp", "22.00 °C / فعال", "پایش دمای پیوند سیلیکونی تراشه."],
        ["جهش فرکانسی (Hopping)", "مد هوپینگ، مین/ماکس/پله/تاخیر", "غیرفعال / جهش سریع", "پیکربندی جداول جهش سریع فرکانسی."],
        ["گین و ردیابی گیرنده", "گین سخت‌افزاری، RSSI، کوادراتور/HD2", "30.00 dB / فعال", "کنترل دستی و خودکار گین و حلقه‌های ردیابی."],
        ["تضعیف فرستنده و PA", "حفاظت تقویت‌کننده، تضعیف دیجیتال", "0 dB Atten / ایمن", "حفاظت تقویت‌کننده توان در برابر بار اضافی."],
        ["همگام‌سازی چندتراشه (MCS)", "پالس SYSREF / Multi-Chip Sync", "Sync Locked", "هم‌فاز سازی تقسیم‌کننده‌های کلاک در چند برد."]
    ]
    format_persian_table(doc.add_table(rows=1, cols=4), col_w_ctrl, headers_ctrl, data_ctrl)

    doc.add_page_break()

    # CHAPTER 7
    add_persian_heading(doc, "فصل ۷: دستورالعمل‌های عملیاتی و فرآیندهای گام‌به‌گام", level=1)

    add_persian_heading(doc, "۷.۱ فرآیند استاندارد بوت و راه‌اندازی سامانه", level=2)
    add_persian_paragraph(
        doc,
        "جهت راه‌اندازی استاندارد سامانه از حالت خاموش تا کارکرد پایدار، مراحل زیر را به ترتیب اجرا نمایید: \n"
        "۱. تغذیه سخت‌افزار SDR را وصل کرده و ۳۰ ثانیه جهت بالا آمدن سیستم‌عامل تعبیه‌شده لینوکس منتظر بمانید.\n"
        "۲. نرم‌افزار eLynxSDR را بر روی کامپیوتر میزبان اجرا کنید.\n"
        "۳. در پنجره اتصال (شکل ۲.۱)، آدرس آی‌پی (192.168.1.10) را وارد کرده و بر روی Connect کلیک کنید.\n"
        "۴. در پنجره انتخاب پروفایل (شکل ۲.۲)، پروفایل متناسب با کاربرد مورد نظر (پروفایل ۱۰۰، ۲۰۰ یا ۴۰۰) را انتخاب کرده و OK را بزنید.\n"
        "۵. در زبانه Global (شکل ۳.۱) بررسی کنید که Active ENSM در وضعیت radio_on و دمای تراشه کمتر از ۶۵ درجه سانتی‌گراد باشد.\n"
        "۶. فرکانس مرکزی مورد نظر (مثلاً ۱۸۰۰ مگاهرتز) را در فیلد TRX Local Oscillator وارد کرده و Enter بزنید.\n"
        "۷. در زبانه Spectrum (شکل ۳.۳)، طول FFT برابر ۴۰۹۶، پنجره Blackman-Harris و میانگین‌گیری ۸ فریم را تنظیم نمایید.\n"
        "۸. صحت دریافت بلادرنگ طیف فرکانسی و آبشار را بر روی صفحه تایید کنید."
    )

    add_persian_heading(doc, "۷.۲ دستورالعمل شبیه‌سازی اکوی هدف راداری با DRFM", level=2)
    add_persian_paragraph(
        doc,
        "جهت شبیه‌سازی اهداف متحرک راداری با تاخیر فاصله و داپلر دقیق: \n"
        "۱. زبانه DRFM (شکل ۴.۱) را از نوار کناری انتخاب کنید.\n"
        "۲. فاصله هدف را بر حسب متر (مثلاً ۵۰۰۰ متر) تعیین کنید (محدوده معتبر: ۱۵۰ متر تا ۲۵ کیلومتر با گام ۱۰ متر).\n"
        "۳. سرعت هدف (m/s) یا جابجایی فرکانس داپلر (fd بر حسب هرتز تا ۶ گیگاهرتز با تفکیک ۱۰ هرتز) را وارد نمایید.\n"
        "۴. تضعیف دامنه اکو (TH0) را متناسب با سطح مقطع راداری (RCS) تنظیم کنید.\n"
        "۵. کلید DRFM Enable را روشن نمایید. ارسال سیگنال اگزیتر فعال بدون وقفه ادامه یافته و ثبات‌های سخت‌افزاری DRFM در لحظه آپدیت می‌شوند."
    )

    add_persian_heading(doc, "۷.۳ تولید و ارسال شکل‌موج‌های اگزیتر و کنترل تضعیف توان", level=2)
    add_persian_paragraph(
        doc,
        "جهت ارسال سیگنال‌های اگزیتر: \n"
        "۱. به زبانه Exciter در نوار ناوبری بالا بروید.\n"
        "۲. زیرزبانه شکل‌موج مورد نظر (CW, Spot, Sweep, Impulse, WideBand, Bridge Noise, Multi Target, LFM) را انتخاب کنید.\n"
        "۳. پارامترهای شکل‌موج (مثلاً برای LFM: فرکانس شروع 0 MHz، پهنای باند 10 MHz و مدت زمان 10 us) را تنظیم کنید.\n"
        "۴. کانال‌های خروجی DAC (Voltage 0/1 برای TX1 و Voltage 2/3 برای TX2) را تیک بزنید.\n"
        "۵. بر روی دکمه Set یا Generate_Send کلیک کنید تا سیگنال در بافر حلقوی بارگذاری شده و به طور پیوسته پخش گردد."
    )

    doc.add_page_break()

    # CHAPTER 8
    add_persian_heading(doc, "فصل ۸: عیب‌یابی، کدهای خطا و مشخصات کامل فنی", level=1)

    add_persian_heading(doc, "۸.۱ ماتریس جامع عیب‌یابی و راهکارهای رفع مشکل", level=2)
    headers_tb = ["نشانه / خطای رخ‌داده", "علت احتمالی بروز مشکل", "اقدام تشخیصی و اصلاحی پیشنهادی"]
    col_w_tb = [Inches(2.0), Inches(2.0), Inches(2.5)]
    data_tb = [
        ["عدم برقراری اتصال / Timeout", "عدم تطابق آی‌پی، مسدود بودن پورت توسط فایروال یا توقف سرویس iiod.", "بررسی آی‌پی استاتیک در رنج 192.168.1.x، دستور پینگ به 192.168.1.10 و بررسی سرویس iiod."],
        ["طیف فرکانسی خط صاف / عدم دریافت نویز", "وضعیت ENSM در حالت radio_off یا عدم قفل PLL نوسان‌ساز.", "رفتن به Configs -> ADRV9009 و زدن Reload Settings، اطمینان از فرکانس در بازه ۷۵ تا ۶۰۰۰ مگاهرتز."],
        ["عدم تطابق مقیاس فرکانس با اسیلوسکوپ IIO", "عدم انتساب صحیح نرخ نمونه‌برداری Fs در ساختار تبدیل فوریه.", "در نسخه v0.0.2 اصلاح گردیده است؛ اطمینان از تطابق پروفایل با کلاک سخت‌افزار (122.88 / 245.76 / 491.52 MSPS)."],
        ["قطع شدن سیگنال اگزیتر هنگام تغییر DRFM", "بارگذاری مجدد بافر DMA با تغییر پارامترهای رجیستر.", "در نسخه v0.0.2 اصلاح گردیده است؛ مقادیر DRFM مستقیماً در ثبات‌های AXI بدون توقف بافر نوشته می‌شوند."],
        ["هشدار دمای بالای پیوند تراشه (> 75 °C)", "عدم کارکرد فن خنک‌کننده یا ارسال مداوم با بیشینه توان خروجی.", "بررسی فن در منوی Devices و افزایش تضعیف دیجیتال فرستنده جهت کاهش توان حرارتی."]
    ]
    format_persian_table(doc.add_table(rows=1, cols=3), col_w_tb, headers_tb, data_tb)

    add_persian_heading(doc, "۸.۲ جدول مشخصات فنی و پارامترهای عملیاتی سیستم", level=2)
    headers_spec_full = ["پارامتر سیستمی", "مشخصه / رتبه فنی", "ملاحظات مهندسی و کاربردی"]
    col_w_spec_full = [Inches(2.2), Inches(2.0), Inches(2.3)]
    data_spec_full = [
        ["محدوده فرکانس کاری RF", "۷۵.۰ مگاهرتز تا ۶۰۰۰.۰ مگاهرتز (۶.۰ گیگاهرتز)", "تیونینگ پیوسته فرکانس با تفکیک‌پذیری ۱ هرتز"],
        ["پهنای باند آنی گیرنده (RX)", "تا ۲۰۰.۰ مگاهرتز", "پشتیبانی از پروفایل ۱۰۰ (۱۰۰ مگاهرتز) و پروفایل ۲۰۰/۴۰۰ (۲۰۰ مگاهرتز)"],
        ["پهنای باند آنی فرستنده (TX/ORX)", "تا ۴۵۰.۰ مگاهرتز", "سنتز چیرپ پهن‌باند و فیدبک حلقه بازخورد دیدبانی"],
        ["رزولوشن مبدل‌های ADC / DAC", "۱۶ بیت مختلط IQ (کانال‌های I و Q)", "پویایی دامنه بالا (> 80 dB SFDR)"],
        ["پوشش تاخیر فاصله DRFM", "۱۵۰ متر تا ۲۵,۰۰۰ متر (۲۵ کیلومتر)", "پله فاصله ۱۰ متر؛ تاخیر ۱.۰ میکروثانیه تا ۱۶۶.۷ میکروثانیه"],
        ["تاخیر سخت‌افزاری پایه DRFM", "حدود ۸۰ نانوثانیه", "جبران‌سازی خودکار در ثبات‌های تاخیر سخت‌افزاری"],
        ["سنتز فرکانس داپلر DRFM", "حامل تا ۶.۰ گیگاهرتز با تفکیک ۱۰ هرتز", "آکومولاتور فاز ۳۲ بیتی NCO در بستر سخت‌افزار FPGA"],
        ["انواع شکل‌موج‌های اگزیتر", "CW, Spot, Sweep, Impulse, WideBand, Bridge, Multi-Target, LFM", "سنتز تحلیلی IQ با ارسال پیوسته حلقوی DMA"],
        ["تعداد اهداف همزمان راداری", "تا ۵ هدف کاملاً مستقل", "تنظیم مستقل شکل‌موج، پهنای باند و جابجایی داپلر"],
        ["طول تبدیلات فوریه FFT", "۱۰۲۴ تا ۶۵۵۳۶ نقطه", "پشتیبانی از پنجره‌های Blackman-Harris, Flat-Top, Hann, Hamming"],
        ["سازگاری نمایشگر میزبان", "مقیاس‌پذیر و ریسپانسیو (Full HD, 2K, 4K)", "رندرینگ بلادرنگ با شتاب‌دهنده گرافیکی OpenGL"]
    ]
    format_persian_table(doc.add_table(rows=1, cols=3), col_w_spec_full, headers_spec_full, data_spec_full)

    # Save
    out_path = os.path.join(os.path.dirname(__file__), "eLynxSDR_Software_User_Guide_FA.docx")
    doc.save(out_path)
    print(f"Successfully generated Persian User Guide DOCX at: {out_path}")

if __name__ == "__main__":
    build_persian_user_guide()
