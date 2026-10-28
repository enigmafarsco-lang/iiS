#!/usr/bin/env python3
"""
generate_repair_diagrams.py
Generates high-resolution engineering schematics, PCB layer callouts (Top & Bottom),
thermal rework profile, and troubleshooting flowchart for the ADRV9009 Hardware
Maintenance and Repair Manual.
"""

import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, Rectangle, Circle, Arrow, Polygon
import numpy as np

OUTPUT_DIR = "/home/user/iiS/docs/images"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Set common font styling
plt.rcParams['font.sans-serif'] = 'DejaVu Sans'
plt.rcParams['font.family'] = 'sans-serif'

# ==============================================================================
# 1. Power Tree & Test Points Schematic Diagram
# ==============================================================================
def generate_power_tree_schematic():
    fig, ax = plt.subplots(figsize=(14, 9), dpi=300)
    ax.set_facecolor('#F8FAFC')
    fig.patch.set_facecolor('#FFFFFF')
    ax.set_xlim(0, 140)
    ax.set_ylim(0, 90)
    ax.axis('off')

    # Title & Subtitle
    ax.text(70, 86, "ADRV9009 POWER TREE & VOLTAGE TEST POINT SCHEMATIC", 
            ha='center', va='center', fontsize=15, fontweight='bold', color='#0F172A')
    ax.text(70, 82.5, "DC-DC Buck PMIC (ADP5054), Ultra-Low Noise LDOs (LT3045), Protection & Test Points", 
            ha='center', va='center', fontsize=10, color='#475569')

    # Draw Main 12V Input Section
    # Box: 12V Barrel / Main DC Input
    p_in = FancyBboxPatch((4, 40), 16, 12, boxstyle="round,pad=0.3,rounding_size=1", 
                          fc='#E2E8F0', ec='#334155', lw=2)
    ax.add_patch(p_in)
    ax.text(12, 47.5, "MAIN DC IN\n+12.0 VDC", ha='center', va='center', fontsize=9, fontweight='bold', color='#0F172A')
    ax.text(12, 42.5, "J_PWR (2-Pin)", ha='center', va='center', fontsize=7.5, color='#475569')

    # Wire to Fuse F1
    ax.plot([20, 26], [46, 46], color='#1E293B', lw=2.5)
    
    # Fuse F1 Box
    p_fuse = FancyBboxPatch((26, 42), 12, 8, boxstyle="round,pad=0.2,rounding_size=0.8", 
                            fc='#FEF08A', ec='#CA8A04', lw=2)
    ax.add_patch(p_fuse)
    ax.text(32, 46.5, "FUSE F1", ha='center', va='center', fontsize=9, fontweight='bold', color='#854D0E')
    ax.text(32, 44, "5A 125V SMD", ha='center', va='center', fontsize=7.5, color='#854D0E')

    # Wire to TVS Diode & TP6
    ax.plot([38, 48], [46, 46], color='#1E293B', lw=2.5)

    # TVS Diode D1
    p_tvs = FancyBboxPatch((42, 28), 10, 7, boxstyle="round,pad=0.2,rounding_size=0.8", 
                           fc='#FEE2E2', ec='#DC2626', lw=1.5)
    ax.add_patch(p_tvs)
    ax.text(47, 32.5, "TVS D1\nSMBJ12A", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#991B1B')
    ax.plot([47, 47], [46, 35], color='#1E293B', lw=1.5, ls='--')
    ax.plot([47, 47], [28, 24], color='#1E293B', lw=1.5)
    # Ground symbol
    ax.plot([44, 50], [24, 24], color='#1E293B', lw=2)
    ax.plot([45.5, 48.5], [22.5, 22.5], color='#1E293B', lw=2)
    ax.plot([46.5, 47.5], [21, 21], color='#1E293B', lw=2)

    # TP6 Test Point Callout
    c_tp6 = Circle((47, 56), 3.5, fc='#EF4444', ec='#991B1B', lw=2)
    ax.add_patch(c_tp6)
    ax.text(47, 56, "TP6", ha='center', va='center', fontsize=8.5, fontweight='bold', color='#FFFFFF')
    ax.plot([47, 47], [46, 52.5], color='#EF4444', lw=2)
    ax.text(47, 61.5, "12.0V VIN_MAIN\n(±10%, <50mV rip)", ha='center', va='bottom', fontsize=7.5, fontweight='bold', color='#B91C1C')

    # Connect to ADP5054 PMIC
    ax.plot([48, 58], [46, 46], color='#1E293B', lw=2.5)

    # ADP5054 PMIC Central Box
    p_pmic = FancyBboxPatch((58, 16), 26, 60, boxstyle="round,pad=0.5,rounding_size=1.5", 
                            fc='#EFF6FF', ec='#1D4ED8', lw=2.5)
    ax.add_patch(p_pmic)
    ax.text(71, 72, "ADP5054 PMIC (U3)", ha='center', va='center', fontsize=11, fontweight='bold', color='#1E3A8A')
    ax.text(71, 69, "Quad High-Current Buck", ha='center', va='center', fontsize=8, color='#3B82F6')

    # 4 Output channels inside ADP5054
    channels = [
        ("Buck 1: 1.0V @ 3.0A (L1)", 60),
        ("Buck 2: 3.3V @ 1.5A (L2)", 46),
        ("Buck 3: 1.8V @ 1.5A (L3)", 32),
        ("Buck 4: 1.5V Pre-Reg (L4)", 18),
    ]
    for ch_text, y_pos in channels:
        p_ch = FancyBboxPatch((60, y_pos), 22, 7, boxstyle="round,pad=0.2,rounding_size=0.6",
                              fc='#DBEAFE', ec='#60A5FA', lw=1.2)
        ax.add_patch(p_ch)
        ax.text(71, y_pos + 3.5, ch_text, ha='center', va='center', fontsize=8, fontweight='bold', color='#1E40AF')

    # --- Buck 1 -> TP1 & Digital Core ---
    ax.plot([82, 102], [63.5, 63.5], color='#1E293B', lw=2)
    # TP1 Badge
    c_tp1 = Circle((92, 63.5), 3, fc='#10B981', ec='#047857', lw=1.5)
    ax.add_patch(c_tp1)
    ax.text(92, 63.5, "TP1", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FFFFFF')
    ax.text(92, 68, "1.00V (±5%)", ha='center', va='bottom', fontsize=7, fontweight='bold', color='#047857')

    p_load1 = FancyBboxPatch((102, 58), 34, 11, boxstyle="round,pad=0.3,rounding_size=1",
                             fc='#ECFDF5', ec='#059669', lw=1.8)
    ax.add_patch(p_load1)
    ax.text(119, 64.5, "VDD_1P0_DIG (ADRV9009 Core)", ha='center', va='center', fontsize=8.5, fontweight='bold', color='#065F46')
    ax.text(119, 60.5, "Digital Baseband & JESD204B SERDES\nRipple < 5 mV rms | Max: 2.5 A", ha='center', va='center', fontsize=7, color='#047857')

    # --- Buck 2 -> TP5 & System 3.3V ---
    ax.plot([82, 102], [49.5, 49.5], color='#1E293B', lw=2)
    c_tp5 = Circle((92, 49.5), 3, fc='#3B82F6', ec='#1D4ED8', lw=1.5)
    ax.add_patch(c_tp5)
    ax.text(92, 49.5, "TP5", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FFFFFF')
    ax.text(92, 54, "3.30V (±5%)", ha='center', va='bottom', fontsize=7, fontweight='bold', color='#1D4ED8')

    p_load5 = FancyBboxPatch((102, 44), 34, 11, boxstyle="round,pad=0.3,rounding_size=1",
                             fc='#EFF6FF', ec='#2563EB', lw=1.8)
    ax.add_patch(p_load5)
    ax.text(119, 50.5, "VCC_3P3_SYS (System Rail)", ha='center', va='center', fontsize=8.5, fontweight='bold', color='#1E40AF')
    ax.text(119, 46.5, "EEPROM, UART FTDI, I2C / SPI & LEDs\nRipple < 15 mV rms | Max: 1.0 A", ha='center', va='center', fontsize=7, color='#1D4ED8')

    # --- Buck 3 -> TP4 & Clock 1.8V ---
    ax.plot([82, 102], [35.5, 35.5], color='#1E293B', lw=2)
    c_tp4 = Circle((92, 35.5), 3, fc='#8B5CF6', ec='#6D28D9', lw=1.5)
    ax.add_patch(c_tp4)
    ax.text(92, 35.5, "TP4", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FFFFFF')
    ax.text(92, 40, "1.80V (±5%)", ha='center', va='bottom', fontsize=7, fontweight='bold', color='#6D28D9')

    p_load4 = FancyBboxPatch((102, 30), 34, 11, boxstyle="round,pad=0.3,rounding_size=1",
                             fc='#F5F3FF', ec='#7C3AED', lw=1.8)
    ax.add_patch(p_load4)
    ax.text(119, 36.5, "VDDA_1P8_CLK (Clock Distribution)", ha='center', va='center', fontsize=8.5, fontweight='bold', color='#5B21B6')
    ax.text(119, 32.5, "HMC7044 Jitter Cleaner & SiT5356 TCXO\nRipple < 8 mV rms | Max: 800 mA", ha='center', va='center', fontsize=7, color='#6D28D9')

    # --- Buck 4 -> LT3045 LDOs -> TP2 & TP3 ---
    ax.plot([82, 93], [21.5, 21.5], color='#1E293B', lw=2)
    ax.plot([93, 93], [12, 26], color='#1E293B', lw=2)
    ax.plot([93, 98], [26, 26], color='#1E293B', lw=2)
    ax.plot([93, 98], [12, 12], color='#1E293B', lw=2)

    # LT3045 (U4) RF LDO
    p_ldo1 = FancyBboxPatch((98, 22), 14, 8, boxstyle="round,pad=0.2,rounding_size=0.6",
                            fc='#FEF3C7', ec='#D97706', lw=1.5)
    ax.add_patch(p_ldo1)
    ax.text(105, 26.5, "LT3045 (U4)", ha='center', va='center', fontsize=8, fontweight='bold', color='#92400E')
    ax.text(105, 24, "Ultra-Low Noise", ha='center', va='center', fontsize=6.5, color='#B45309')

    # LT3045 (U5) Synth LDO
    p_ldo2 = FancyBboxPatch((98, 8), 14, 8, boxstyle="round,pad=0.2,rounding_size=0.6",
                            fc='#FEF3C7', ec='#D97706', lw=1.5)
    ax.add_patch(p_ldo2)
    ax.text(105, 12.5, "LT3045 (U5)", ha='center', va='center', fontsize=8, fontweight='bold', color='#92400E')
    ax.text(105, 10, "+ FB3 Bead", ha='center', va='center', fontsize=6.5, color='#B45309')

    # Wire to TP2 & TP3 and Loads
    ax.plot([112, 117], [26, 26], color='#1E293B', lw=2)
    c_tp2 = Circle((117, 26), 2.8, fc='#F59E0B', ec='#B45309', lw=1.5)
    ax.add_patch(c_tp2)
    ax.text(117, 26, "TP2", ha='center', va='center', fontsize=7, fontweight='bold', color='#FFFFFF')

    p_load2 = FancyBboxPatch((122, 22), 16, 8, boxstyle="round,pad=0.2,rounding_size=0.6",
                             fc='#FFFBEB', ec='#F59E0B', lw=1.5)
    ax.add_patch(p_load2)
    ax.text(130, 26.5, "VDDA_1P3_RF", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#92400E')
    ax.text(130, 24, "1.30V (<3mV rms)", ha='center', va='center', fontsize=6.5, color='#B45309')

    ax.plot([112, 117], [12, 12], color='#1E293B', lw=2)
    c_tp3 = Circle((117, 12), 2.8, fc='#F59E0B', ec='#B45309', lw=1.5)
    ax.add_patch(c_tp3)
    ax.text(117, 12, "TP3", ha='center', va='center', fontsize=7, fontweight='bold', color='#FFFFFF')

    p_load3 = FancyBboxPatch((122, 8), 16, 8, boxstyle="round,pad=0.2,rounding_size=0.6",
                             fc='#FFFBEB', ec='#F59E0B', lw=1.5)
    ax.add_patch(p_load3)
    ax.text(130, 12.5, "VDDA_1P3_SYNTH", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#92400E')
    ax.text(130, 10, "1.30V (<2mV rms)", ha='center', va='center', fontsize=6.5, color='#B45309')

    # Legend / Info Box at bottom left
    p_leg = FancyBboxPatch((4, 4), 50, 15, boxstyle="round,pad=0.3,rounding_size=0.8",
                           fc='#FFFFFF', ec='#94A3B8', lw=1)
    ax.add_patch(p_leg)
    ax.text(6, 16.5, "CHECK POINT REFERENCE LEGEND:", fontsize=8, fontweight='bold', color='#0F172A')
    ax.text(6, 13, "• Red [TP6]: 12.0V Main Input after Fuse F1 & TVS D1", fontsize=7.5, color='#334155')
    ax.text(6, 10, "• Green [TP1]: 1.00V Core | Blue [TP5]: 3.30V System | Purple [TP4]: 1.80V Clock", fontsize=7.5, color='#334155')
    ax.text(6, 7, "• Amber [TP2, TP3]: 1.30V Ultra-Low Noise Analog RF & Synthesizer VCO rails", fontsize=7.5, color='#334155')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "adrv9009_power_tree_schematic.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 2. PCB Top Layer Layout & Test Point Callout Diagram
# ==============================================================================
def generate_pcb_top_layer():
    fig, ax = plt.subplots(figsize=(14, 10), dpi=300)
    ax.set_facecolor('#0B2014')  # Dark PCB Green
    fig.patch.set_facecolor('#05130B')
    ax.set_xlim(-5, 145)
    ax.set_ylim(-5, 105)
    ax.axis('off')

    # PCB Board Outline (Rounded rectangle with gold mounting holes)
    board = FancyBboxPatch((0, 0), 140, 100, boxstyle="round,pad=0.5,rounding_size=3",
                           fc='#133820', ec='#D4AF37', lw=3.5)
    ax.add_patch(board)

    # 4 Corner Mounting Holes
    for mx, my in [(6, 6), (134, 6), (6, 94), (134, 94)]:
        c_outer = Circle((mx, my), 3.5, fc='#D4AF37', ec='#854D0E', lw=1.5)
        c_inner = Circle((mx, my), 2.2, fc='#05130B', ec='#D4AF37', lw=1)
        ax.add_patch(c_outer)
        ax.add_patch(c_inner)

    # Title Banner on PCB Silkscreen
    ax.text(70, 96, "ADRV9009 EVALUATION BOARD - TOP LAYER COMPONENT & TEST POINT MAP",
            ha='center', va='center', fontsize=11, fontweight='bold', color='#FFFFFF')
    ax.text(70, 92.5, "IPC-7711/7721 Hardware Diagnostics & Voltage Check Points Reference",
            ha='center', va='center', fontsize=8, color='#A7F3D0')

    # Top Edge: RF SMA Connectors (TX1, TX2, RX1, RX2, ORX)
    rf_connectors = [
        ("TX1_OUT", 25, "TX1 Out (Balun T1)"),
        ("TX2_OUT", 48, "TX2 Out (Balun T2)"),
        ("RX1_IN", 75, "RX1 In (Balun T3)"),
        ("RX2_IN", 98, "RX2 In (Balun T4)"),
        ("ORX_IN", 120, "ORX In (Balun T5)")
    ]
    for name, x_pos, desc in rf_connectors:
        # Gold SMA Jack Body
        p_sma = FancyBboxPatch((x_pos - 6, 82), 12, 8, boxstyle="round,pad=0.2,rounding_size=0.5",
                               fc='#D4AF37', ec='#FDE047', lw=1.5)
        ax.add_patch(p_sma)
        # Center Pin
        c_pin = Circle((x_pos, 86), 1.5, fc='#FFFFFF', ec='#854D0E', lw=1)
        ax.add_patch(c_pin)
        ax.text(x_pos, 86, name, ha='center', va='center', fontsize=6.5, fontweight='bold', color='#000000')

        # RF Balun & ESD Diode below SMA
        p_balun = Rectangle((x_pos - 3.5, 71), 7, 6, fc='#334155', ec='#94A3B8', lw=1)
        ax.add_patch(p_balun)
        ax.text(x_pos, 74, "Balun\n" + name.split('_')[0], ha='center', va='center', fontsize=5.5, color='#F8FAFC')
        # RF Microstrip Trace to U1
        ax.plot([x_pos, x_pos, 70], [71, 62, 58], color='#D4AF37', lw=1.8)

    # Main Center IC: ADRV9009 (U1) 196-Ball BGA
    p_u1 = FancyBboxPatch((55, 36), 30, 24, boxstyle="round,pad=0.3,rounding_size=0.8",
                          fc='#1E293B', ec='#E2E8F0', lw=2)
    ax.add_patch(p_u1)
    # Pin 1 Dot
    c_pin1 = Circle((58, 57), 1.2, fc='#FFFFFF')
    ax.add_patch(c_pin1)
    ax.text(70, 50, "U1: ADRV9009BBCZ", ha='center', va='center', fontsize=9.5, fontweight='bold', color='#FFFFFF')
    ax.text(70, 44, "196-Ball CSP_BGA (12x12mm)\nDual RF Transceiver", ha='center', va='center', fontsize=7, color='#94A3B8')

    # Top Left: Power Entry, Fuse F1, TVS D1, TP6
    # DC Power Jack (Molex / Barrel)
    p_jack = FancyBboxPatch((6, 60), 12, 14, boxstyle="round,pad=0.2,rounding_size=0.8",
                            fc='#334155', ec='#E2E8F0', lw=1.5)
    ax.add_patch(p_jack)
    ax.text(12, 67, "12V IN\nJ_PWR", ha='center', va='center', fontsize=7, fontweight='bold', color='#FFFFFF')

    # Fuse F1 (Yellow SMD)
    p_f1 = Rectangle((10, 46), 7, 5, fc='#EAB308', ec='#CA8A04', lw=1.5)
    ax.add_patch(p_f1)
    ax.text(13.5, 48.5, "F1: 5A", ha='center', va='center', fontsize=6, fontweight='bold', color='#000000')

    # TVS Diode D1
    p_d1 = Rectangle((10, 35), 7, 5, fc='#EF4444', ec='#991B1B', lw=1.5)
    ax.add_patch(p_d1)
    ax.text(13.5, 37.5, "D1: TVS", ha='center', va='center', fontsize=6, fontweight='bold', color='#FFFFFF')

    # Test Point TP6 (12V Input Check Point)
    c_tp6 = Circle((22, 48.5), 2.8, fc='#DC2626', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp6)
    ax.text(22, 48.5, "TP6", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    # Bottom Left: Clocking Section (HMC7044 U2 & TCXO Y1)
    p_u2 = FancyBboxPatch((18, 14), 18, 14, boxstyle="round,pad=0.2,rounding_size=0.6",
                          fc='#1E293B', ec='#60A5FA', lw=1.5)
    ax.add_patch(p_u2)
    ax.text(27, 22, "U2: HMC7044", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#93C5FD')
    ax.text(27, 17, "Clock Gen / Jitter Cleaner", ha='center', va='center', fontsize=5.5, color='#CBD5E1')

    # TCXO Y1
    p_y1 = Rectangle((6, 16), 8, 6, fc='#64748B', ec='#E2E8F0', lw=1)
    ax.add_patch(p_y1)
    ax.text(10, 19, "Y1: 40M\nTCXO", ha='center', va='center', fontsize=5, fontweight='bold', color='#FFFFFF')

    # Test Point TP4 (1.8V Clock Check Point)
    c_tp4 = Circle((39, 21), 2.8, fc='#8B5CF6', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp4)
    ax.text(39, 21, "TP4", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    # Clock Differential Probing Points
    c_clk_p = Circle((39, 14), 1.8, fc='#D4AF37', ec='#FFFFFF', lw=1)
    c_clk_n = Circle((44, 14), 1.8, fc='#D4AF37', ec='#FFFFFF', lw=1)
    ax.add_patch(c_clk_p)
    ax.add_patch(c_clk_n)
    ax.text(41.5, 9.5, "CLK_DIFF (122.88M)", ha='center', va='center', fontsize=5.5, color='#E2E8F0')

    # Bottom Edge: High Speed FMC Connector (J1)
    p_fmc = FancyBboxPatch((55, 4), 65, 8, boxstyle="round,pad=0.2,rounding_size=0.5",
                           fc='#334155', ec='#D4AF37', lw=1.5)
    ax.add_patch(p_fmc)
    ax.text(87.5, 8, "J1: HIGH-SPEED FMC CONNECTOR (JESD204B SERDES & CONTROL BUS)",
            ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FDE047')

    # Voltage Test Points around ADRV9009 (Top Layer Check Points)
    # TP1: 1.0V Digital Core
    c_tp1 = Circle((50, 48), 2.8, fc='#10B981', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp1)
    ax.text(50, 48, "TP1", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    # TP2: 1.3V Analog RF
    c_tp2 = Circle((90, 52), 2.8, fc='#F59E0B', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp2)
    ax.text(90, 52, "TP2", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    # TP3: 1.3V Synth VCO
    c_tp3 = Circle((90, 40), 2.8, fc='#F59E0B', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp3)
    ax.text(90, 40, "TP3", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    # TP5: 3.3V System & U6 EEPROM
    p_u6 = Rectangle((100, 20), 8, 6, fc='#1E293B', ec='#94A3B8', lw=1)
    ax.add_patch(p_u6)
    ax.text(104, 23, "U6\nEEPROM", ha='center', va='center', fontsize=5.5, color='#F8FAFC')
    c_tp5 = Circle((112, 23), 2.8, fc='#3B82F6', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp5)
    ax.text(112, 23, "TP5", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    # GND Clips (Spring Probe Clips)
    for gx, gy, gname in [(30, 48.5, "GND1"), (112, 35, "GND2")]:
        c_gnd = Circle((gx, gy), 2.5, fc='#000000', ec='#D4AF37', lw=1.5)
        ax.add_patch(c_gnd)
        ax.text(gx, gy, "GND", ha='center', va='center', fontsize=5.5, fontweight='bold', color='#D4AF37')

    # Callout Pointers / Annotations Box on Right
    p_info = FancyBboxPatch((100, 62), 36, 18, boxstyle="round,pad=0.3,rounding_size=0.8",
                            fc='#0F172A', ec='#38BDF8', lw=1.5)
    ax.add_patch(p_info)
    ax.text(118, 76, "QUICK TEST GUIDE", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#38BDF8')
    ax.text(118, 71, "1. Check Fuse F1 & TP6 (12V)", ha='center', va='center', fontsize=6.5, color='#E2E8F0')
    ax.text(118, 67, "2. Check TP1 (1.0V) & TP5 (3.3V)", ha='center', va='center', fontsize=6.5, color='#E2E8F0')
    ax.text(118, 63, "3. Check TP2/TP3 (1.3V RF/Synth)", ha='center', va='center', fontsize=6.5, color='#E2E8F0')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "adrv9009_pcb_top_layer.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 3. PCB Bottom Layer Layout & Power IC Callout Diagram
# ==============================================================================
def generate_pcb_bottom_layer():
    fig, ax = plt.subplots(figsize=(14, 10), dpi=300)
    ax.set_facecolor('#0B2014')  # Dark PCB Green
    fig.patch.set_facecolor('#05130B')
    ax.set_xlim(-5, 145)
    ax.set_ylim(-5, 105)
    ax.axis('off')

    # PCB Board Outline (Rounded rectangle)
    board = FancyBboxPatch((0, 0), 140, 100, boxstyle="round,pad=0.5,rounding_size=3",
                           fc='#133820', ec='#D4AF37', lw=3.5)
    ax.add_patch(board)

    # 4 Corner Mounting Holes
    for mx, my in [(6, 6), (134, 6), (6, 94), (134, 94)]:
        c_outer = Circle((mx, my), 3.5, fc='#D4AF37', ec='#854D0E', lw=1.5)
        c_inner = Circle((mx, my), 2.2, fc='#05130B', ec='#D4AF37', lw=1)
        ax.add_patch(c_outer)
        ax.add_patch(c_inner)

    # Title Banner on PCB Silkscreen
    ax.text(70, 96, "ADRV9009 EVALUATION BOARD - BOTTOM LAYER POWER IC & DECOUPLING MAP",
            ha='center', va='center', fontsize=11, fontweight='bold', color='#FFFFFF')
    ax.text(70, 92.5, "ADP5054 Buck PMIC, LT3045 LDO Regulators & High-Frequency Decoupling Array",
            ha='center', va='center', fontsize=8, color='#A7F3D0')

    # Central BGA Underside Decoupling Capacitor Matrix (directly below U1)
    p_bga_sub = FancyBboxPatch((55, 36), 30, 24, boxstyle="round,pad=0.3,rounding_size=0.8",
                               fc='#164E2E', ec='#D4AF37', lw=1.5, ls='--')
    ax.add_patch(p_bga_sub)
    ax.text(70, 56, "UNDERSIDE OF U1 (ADRV9009)", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FDE047')
    
    # Draw Decoupling Matrix (0402 / 0201 caps)
    for cx in np.linspace(58, 82, 7):
        for cy in np.linspace(39, 51, 4):
            c_cap = Rectangle((cx - 1, cy - 0.7), 2, 1.4, fc='#CBD5E1', ec='#475569', lw=0.5)
            ax.add_patch(c_cap)
    ax.text(70, 34, "0402 / 0201 Bypass Caps (0.1uF / 10nF / 1nF NPO)", ha='center', va='center', fontsize=6, color='#CBD5E1')

    # Left Section: ADP5054 PMIC (U3) & High-Current Inductors L1-L4
    p_u3 = FancyBboxPatch((15, 45), 20, 20, boxstyle="round,pad=0.3,rounding_size=0.8",
                          fc='#1E293B', ec='#3B82F6', lw=2)
    ax.add_patch(p_u3)
    ax.text(25, 57, "U3: ADP5054", ha='center', va='center', fontsize=8.5, fontweight='bold', color='#60A5FA')
    ax.text(25, 52, "Quad Buck PMIC\n(48-LFCSP 7x7mm)", ha='center', va='center', fontsize=6.5, color='#94A3B8')

    # 4 Power Inductors (L1, L2, L3, L4)
    inductors = [
        ("L1 (1.0uH)", 38, 58, "1.0V Rail"),
        ("L2 (2.2uH)", 38, 48, "3.3V Rail"),
        ("L3 (2.2uH)", 38, 38, "1.8V Rail"),
        ("L4 (1.5uH)", 38, 28, "1.5V Pre-Reg")
    ]
    for lname, lx, ly, lrail in inductors:
        p_ind = FancyBboxPatch((lx, ly), 11, 7, boxstyle="round,pad=0.2,rounding_size=0.4",
                               fc='#0F172A', ec='#CA8A04', lw=1.5)
        ax.add_patch(p_ind)
        ax.text(lx + 5.5, ly + 4.5, lname, ha='center', va='center', fontsize=6, fontweight='bold', color='#FDE047')
        ax.text(lx + 5.5, ly + 2, lrail, ha='center', va='center', fontsize=5, color='#94A3B8')

    # Right Section: Ultra-Low Noise RF LDOs (LT3045 U4 & U5)
    # LDO 1: U4 (1.3V RF)
    p_u4 = FancyBboxPatch((98, 55), 16, 12, boxstyle="round,pad=0.2,rounding_size=0.6",
                          fc='#1E293B', ec='#F59E0B', lw=1.8)
    ax.add_patch(p_u4)
    ax.text(106, 63, "U4: LT3045", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FCD34D')
    ax.text(106, 59, "1.3V RF LDO\n(0.8 uVrms noise)", ha='center', va='center', fontsize=6, color='#CBD5E1')

    # LDO 2: U5 (1.3V Synth) + FB3
    p_u5 = FancyBboxPatch((98, 35), 16, 12, boxstyle="round,pad=0.2,rounding_size=0.6",
                          fc='#1E293B', ec='#F59E0B', lw=1.8)
    ax.add_patch(p_u5)
    ax.text(106, 43, "U5: LT3045", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FCD34D')
    ax.text(106, 39, "1.3V Synth LDO\n+ FB3 Filter", ha='center', va='center', fontsize=6, color='#CBD5E1')

    # Bottom Annular Test Points (TP1-TP6 Duplicate Rings)
    test_rings = [
        ("TP1_BOT", 60, 20, '#10B981', "1.0V DIG"),
        ("TP2_BOT", 118, 61, '#F59E0B', "1.3V RF"),
        ("TP3_BOT", 118, 41, '#F59E0B', "1.3V SYN"),
        ("TP4_BOT", 20, 20, '#8B5CF6', "1.8V CLK"),
        ("TP5_BOT", 20, 75, '#3B82F6', "3.3V SYS"),
        ("TP6_BOT", 10, 85, '#EF4444', "12V MAIN")
    ]
    for tname, tx, ty, tcol, tsub in test_rings:
        c_ring = Circle((tx, ty), 3.2, fc=tcol, ec='#FFFFFF', lw=1.5)
        ax.add_patch(c_ring)
        ax.text(tx, ty, tname.split('_')[0], ha='center', va='center', fontsize=6, fontweight='bold', color='#FFFFFF')
        ax.text(tx, ty - 5, tsub, ha='center', va='center', fontsize=5.5, color='#F8FAFC')

    # Thermal Stitching Via Array Callout
    p_therm = FancyBboxPatch((55, 70), 30, 16, boxstyle="round,pad=0.3,rounding_size=0.8",
                             fc='#042F2E', ec='#14B8A6', lw=1.5)
    ax.add_patch(p_therm)
    ax.text(70, 80, "THERMAL STITCHING ARRAY", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#5EEAD4')
    ax.text(70, 74, "48 Solid Copper Vias direct to Top Ground\nMounting Zone for Solid Aluminum Backplate Heatsink",
            ha='center', va='center', fontsize=6, color='#CCFBF1')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "adrv9009_pcb_bottom_layer.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 4. ADRV9009 BGA Rework Thermal Profile Chart
# ==============================================================================
def generate_bga_rework_profile():
    fig, ax = plt.subplots(figsize=(12, 7), dpi=300)
    ax.set_facecolor('#F8FAFC')
    fig.patch.set_facecolor('#FFFFFF')

    # Time array (0 to 300 seconds)
    time = np.array([0, 30, 80, 110, 160, 190, 210, 225, 240, 255, 275, 300])
    temp = np.array([25, 80, 150, 170, 195, 217, 240, 243, 235, 217, 130, 45])

    # Plot profile curve
    ax.plot(time, temp, color='#2563EB', lw=3, label='Thermocouple Solder Joint Temp (°C)')
    ax.scatter(time, temp, color='#1D4ED8', s=45, zorder=5)

    # Zones background shading
    # Zone 1: Preheat (0-100s)
    ax.axvspan(0, 100, color='#EFF6FF', alpha=0.8, label='1. Preheat Zone (+1.5°C/s)')
    # Zone 2: Soak / Flux Activation (100-185s)
    ax.axvspan(100, 185, color='#FEF3C7', alpha=0.8, label='2. Soak / Flux Activation (170-200°C)')
    # Zone 3: Reflow / Peak (185-255s)
    ax.axvspan(185, 255, color='#FEE2E2', alpha=0.8, label='3. Reflow Peak (235-245°C, TAL 45s)')
    # Zone 4: Cooling (255-300s)
    ax.axvspan(255, 300, color='#ECFDF5', alpha=0.8, label='4. Controlled Cooling (-2.5°C/s)')

    # Liquidus line 217°C SAC305
    ax.axhline(217, color='#DC2626', ls='--', lw=1.8, label='Liquidus Temp TL = 217°C (SAC305)')
    ax.text(10, 220, "Liquidus Line (217°C SAC305 Lead-Free)", fontsize=8, fontweight='bold', color='#DC2626')

    # Peak annotation
    ax.annotate('Peak Temp: 243°C\n(TAL: 45s, Max 245°C)', xy=(225, 243), xytext=(205, 265),
                arrowprops=dict(facecolor='#DC2626', shrink=0.08, width=1.5, headwidth=6),
                fontsize=8.5, fontweight='bold', color='#B91C1C', ha='center',
                bbox=dict(boxstyle='round,pad=0.3', fc='#FEF2F2', ec='#DC2626', lw=1.2))

    # Vacuum pickup point annotation
    ax.annotate('Vacuum Pickup Activation\n(T >= 235°C)', xy=(220, 238), xytext=(150, 250),
                arrowprops=dict(facecolor='#7C3AED', shrink=0.08, width=1.2, headwidth=5),
                fontsize=8, fontweight='bold', color='#6D28D9', ha='center',
                bbox=dict(boxstyle='round,pad=0.2', fc='#F5F3FF', ec='#7C3AED', lw=1))

    # Titles & Labels
    ax.set_title("ADRV9009 196-Ball CSP_BGA Rework & Soldering Thermal Profile (IPC-7711/7721)",
                 fontsize=12, fontweight='bold', color='#0F172A', pad=15)
    ax.set_xlabel("Time (Seconds)", fontsize=10, fontweight='bold', color='#334155')
    ax.set_ylabel("Temperature (°C)", fontsize=10, fontweight='bold', color='#334155')
    ax.set_xlim(0, 300)
    ax.set_ylim(0, 280)
    ax.grid(True, ls=':', color='#CBD5E1', alpha=0.7)
    ax.legend(loc='lower right', fontsize=8, framealpha=0.95)

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "adrv9009_bga_rework_thermal_profile.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 5. Hardware Troubleshooting & Diagnostic Flowchart
# ==============================================================================
def generate_troubleshooting_flowchart():
    fig, ax = plt.subplots(figsize=(14, 11), dpi=300)
    ax.set_facecolor('#F8FAFC')
    fig.patch.set_facecolor('#FFFFFF')
    ax.set_xlim(0, 140)
    ax.set_ylim(0, 110)
    ax.axis('off')

    # Title
    ax.text(70, 106, "ADRV9009 HARDWARE SYSTEM TROUBLESHOOTING & REPAIR FLOWCHART",
            ha='center', va='center', fontsize=14, fontweight='bold', color='#0F172A')
    ax.text(70, 102.5, "Step-by-Step Diagnostic Sequence from Power Entry to RF Transmission & Clock Lock",
            ha='center', va='center', fontsize=9, color='#475569')

    # Box Styles
    def draw_box(x, y, w, h, title, text, bg, border, tcolor='#0F172A'):
        p = FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.3,rounding_size=0.8",
                           fc=bg, ec=border, lw=1.8)
        ax.add_patch(p)
        ax.text(x + w/2, y + h*0.7, title, ha='center', va='center', fontsize=8.5, fontweight='bold', color=tcolor)
        ax.text(x + w/2, y + h*0.35, text, ha='center', va='center', fontsize=7.2, color='#334155')

    def draw_decision(x, y, w, h, title, text):
        poly = Polygon([[x + w/2, y + h], [x + w, y + h/2], [x + w/2, y], [x, y + h/2]],
                       fc='#FEF3C7', ec='#D97706', lw=1.8)
        ax.add_patch(poly)
        ax.text(x + w/2, y + h*0.6, title, ha='center', va='center', fontsize=8, fontweight='bold', color='#92400E')
        ax.text(x + w/2, y + h*0.35, text, ha='center', va='center', fontsize=6.8, color='#78350F')

    # Step 1: Start / Power In
    draw_box(55, 88, 30, 9, "START: Power Up", "Connect 12.0 VDC to J_PWR", '#EFF6FF', '#2563EB', '#1E40AF')
    ax.arrow(70, 88, 0, -4, head_width=2, head_length=1.5, fc='#1E293B', ec='#1E293B')

    # Decision 1: TP6 == 12V?
    draw_decision(52, 69, 36, 13, "TP6 == 12.0V ?", "Measure TP6 vs GND")

    # If NO -> Check Fuse F1 & D1
    ax.arrow(52, 75.5, -12, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(45, 77.5, "NO (0V)", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 71, 36, 11, "POWER ENTRY FAILURE", "Replace Fuse F1 (5A 1206)\nCheck TVS D1 for short circuit", '#FEE2E2', '#DC2626', '#991B1B')

    # If YES -> Check TP1 & TP5
    ax.arrow(70, 69, 0, -5, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(72, 66.5, "YES", fontsize=7.5, fontweight='bold', color='#059669')

    # Decision 2: TP1 (1.0V) & TP5 (3.3V) OK?
    draw_decision(48, 47, 44, 14, "TP1 (1.0V) & TP5 (3.3V) OK?", "Measure Buck 1 & Buck 2 Output")

    # If NO -> ADP5054 Failure
    ax.arrow(48, 54, -10, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(42, 56, "NO", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 49, 34, 11, "PMIC BUCK FAILURE", "Inspect ADP5054 (U3)\nCheck Inductors L1, L2 & short to GND", '#FEE2E2', '#DC2626', '#991B1B')

    # If YES -> Check TP2 & TP3 (1.3V RF/Synth)
    ax.arrow(70, 47, 0, -5, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(72, 44.5, "YES", fontsize=7.5, fontweight='bold', color='#059669')

    # Decision 3: TP2 & TP3 == 1.30V?
    draw_decision(48, 25, 44, 14, "TP2 & TP3 == 1.30V?", "Measure LDO U4/U5 (LT3045)")

    # If NO -> LT3045 Failure
    ax.arrow(48, 32, -10, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(42, 34, "NO", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 27, 34, 11, "RF LDO FAILURE", "Inspect LT3045 U4/U5\nVerify FB3 Ferrite Bead & 1.5V In", '#FEE2E2', '#DC2626', '#991B1B')

    # If YES -> Check Clock & JESD204B
    ax.arrow(70, 25, 0, -5, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(72, 22.5, "YES", fontsize=7.5, fontweight='bold', color='#059669')

    # Decision 4: HMC7044 Lock & JESD204B Link?
    draw_decision(46, 3, 48, 14, "HMC7044 Locked & JESD204B OK?", "Check TP4 (1.8V), SYSREF & SYNC~")

    # If NO -> Clock Failure
    ax.arrow(46, 10, -8, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(40, 12, "NO", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 5, 34, 11, "CLOCK / SYNC FAILURE", "Verify SiT5356 40MHz TCXO\nInspect U2 (HMC7044) & JESD lanes", '#FEE2E2', '#DC2626', '#991B1B')

    # If YES -> System Operational & RF Ready!
    ax.arrow(94, 10, 10, 0, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(97, 12, "YES", fontsize=7.5, fontweight='bold', color='#059669')
    draw_box(106, 5, 30, 11, "SYSTEM FULLY OPERATIONAL", "RF Transceiver U1 Operational\nExecute BIT & RF Loopback Test", '#ECFDF5', '#059669', '#065F46')

    # Top Right Reference Box
    draw_box(96, 68, 40, 30, "KEY DIAGNOSTIC LIMITS",
             "• TP6 (12V Input): 10.8V to 13.2V\n• TP1 (Digital 1.0V): 0.95V to 1.05V\n• TP5 (System 3.3V): 3.14V to 3.46V\n• TP4 (Clock 1.8V): 1.71V to 1.89V\n• TP2 (RF 1.3V): 1.24V to 1.36V (<3mV rip)\n• TP3 (Synth 1.3V): 1.24V to 1.36V (<2mV rip)",
             '#FFFFFF', '#94A3B8', '#0F172A')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "adrv9009_hardware_troubleshooting_flowchart.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

if __name__ == '__main__':
    print("Generating ADRV9009 Maintenance & Repair Engineering Diagrams...")
    generate_power_tree_schematic()
    generate_pcb_top_layer()
    generate_pcb_bottom_layer()
    generate_bga_rework_profile()
    generate_troubleshooting_flowchart()
    print("All 5 diagrams successfully generated in docs/images/!")
