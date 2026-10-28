#!/usr/bin/env python3
"""
generate_repair_diagrams.py
Generates high-resolution engineering schematics, Altium Job1 & Job2 output drawings,
PCB layer callouts (Top & Bottom), thermal rework profile, RF test bench setup diagram,
and troubleshooting flowchart for the ADRV9009 Hardware Maintenance and Repair Manual.
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

plt.rcParams['font.sans-serif'] = 'DejaVu Sans'
plt.rcParams['font.family'] = 'sans-serif'

# ==============================================================================
# 1. Altium Output Job 1: Schematic Sheet with Test Check Points & Net Traces
# ==============================================================================
def generate_altium_job1_schematic():
    fig, ax = plt.subplots(figsize=(16, 11), dpi=300)
    ax.set_facecolor('#FFFFFC')  # Altium Schematic Off-White / Cream background
    fig.patch.set_facecolor('#F1F5F9')
    ax.set_xlim(0, 160)
    ax.set_ylim(0, 110)
    ax.axis('off')

    # Draw Altium Outer Border & Title Block
    border = Rectangle((2, 2), 156, 106, fill=False, edgecolor='#8B0000', lw=2)
    inner_border = Rectangle((4, 4), 152, 102, fill=False, edgecolor='#8B0000', lw=1)
    ax.add_patch(border)
    ax.add_patch(inner_border)

    # Grid reference labels (A, B, C, D / 1, 2, 3, 4)
    for idx, (x, lbl) in enumerate([(20, '1'), (60, '2'), (100, '3'), (140, '4')]):
        ax.text(x, 107, lbl, fontsize=8, color='#8B0000', fontweight='bold', ha='center')
        ax.text(x, 3, lbl, fontsize=8, color='#8B0000', fontweight='bold', ha='center')
    for idx, (y, lbl) in enumerate([(20, 'D'), (45, 'C'), (70, 'B'), (95, 'A')]):
        ax.text(3, y, lbl, fontsize=8, color='#8B0000', fontweight='bold', va='center')
        ax.text(157, y, lbl, fontsize=8, color='#8B0000', fontweight='bold', va='center')

    # Altium Title Block (Bottom Right)
    tb = Rectangle((100, 4), 56, 18, fill=True, facecolor='#FFFFF0', edgecolor='#8B0000', lw=1.2)
    ax.add_patch(tb)
    ax.plot([100, 156], [16, 16], color='#8B0000', lw=1)
    ax.plot([100, 156], [10, 10], color='#8B0000', lw=1)
    ax.plot([128, 128], [4, 16], color='#8B0000', lw=1)

    ax.text(102, 18.5, "ALTIUM DESIGNER OUTJOB 1: SCHEMATIC CHECK POINTS", fontsize=8, fontweight='bold', color='#8B0000')
    ax.text(102, 13, "PROJECT: ADRV9009_RF_TRX_REV_C", fontsize=7.5, fontweight='bold', color='#0F172A')
    ax.text(102, 7, "DOC: SCH_POWER_RF_DIAGNOSTICS", fontsize=7, color='#334155')
    ax.text(130, 13, "SHEET: 01 OF 04", fontsize=7.5, fontweight='bold', color='#0F172A')
    ax.text(130, 7, "REV: 3.2 (OCT 2026)", fontsize=7, color='#334155')

    # Altium Grid Dots (Subtle)
    gx, gy = np.meshgrid(np.arange(8, 154, 4), np.arange(8, 104, 4))
    ax.scatter(gx, gy, s=0.2, color='#CBD5E1', alpha=0.5)

    # --- BLOCK 1: Power Input & Protection (Top Left) ---
    p_b1 = Rectangle((8, 68), 44, 34, fill=True, facecolor='#FFFFFF', edgecolor='#00008B', lw=1.5)
    ax.add_patch(p_b1)
    ax.text(10, 99, "BLOCK 1: 12V MAIN POWER INPUT & PROTECTION", fontsize=7.5, fontweight='bold', color='#00008B')

    # J1 Connector
    ax.text(12, 88, "[J_PWR]\n12V IN", fontsize=7, fontweight='bold', color='#0F172A', bbox=dict(boxstyle='square,pad=0.2', fc='#E2E8F0', ec='#475569'))
    ax.plot([19, 23], [88, 88], color='#00008B', lw=1.5)

    # Fuse F1
    p_f = Rectangle((23, 85), 8, 6, fill=True, facecolor='#FEF08A', edgecolor='#CA8A04', lw=1.2)
    ax.add_patch(p_f)
    ax.text(27, 88, "F1\n5A 125V", fontsize=6, fontweight='bold', color='#854D0E', ha='center', va='center')
    ax.plot([31, 37], [88, 88], color='#00008B', lw=1.5)

    # TVS Diode D1
    ax.plot([37, 37], [88, 76], color='#00008B', lw=1.5)
    p_d = Rectangle((34, 76), 6, 5, fill=True, facecolor='#FEE2E2', edgecolor='#DC2626', lw=1)
    ax.add_patch(p_d)
    ax.text(37, 78.5, "D1\nSMBJ12", fontsize=5.5, fontweight='bold', color='#991B1B', ha='center', va='center')
    ax.plot([37, 37], [76, 72], color='#00008B', lw=1.5)
    # GND bar
    ax.plot([35, 39], [72, 72], color='#00008B', lw=1.5)

    # TP6 Test Point Callout
    c_tp6 = Circle((46, 88), 2.5, fc='#EF4444', ec='#991B1B', lw=1.5)
    ax.add_patch(c_tp6)
    ax.text(46, 88, "TP6", fontsize=6.5, fontweight='bold', color='#FFFFFF', ha='center', va='center')
    ax.text(46, 93, "NET: VIN_12V\n(10.8V - 13.2V)", fontsize=6, fontweight='bold', color='#B91C1C', ha='center')

    # Net label output to PMIC
    ax.plot([37, 50], [88, 88], color='#00008B', lw=1.5)
    ax.text(50, 88, ">> VIN_12V0", fontsize=6.5, fontweight='bold', color='#00008B', va='center')

    # --- BLOCK 2: ADP5054 PMIC Quad Buck Regulator (Top Center) ---
    p_b2 = Rectangle((58, 54), 42, 48, fill=True, facecolor='#FFFFFF', edgecolor='#00008B', lw=1.5)
    ax.add_patch(p_b2)
    ax.text(60, 99, "BLOCK 2: ADP5054 QUAD BUCK PMIC (U3)", fontsize=7.5, fontweight='bold', color='#00008B')

    # ADP5054 Chip Symbol
    p_u3 = Rectangle((64, 58), 20, 36, fill=True, facecolor='#EFF6FF', edgecolor='#1D4ED8', lw=1.2)
    ax.add_patch(p_u3)
    ax.text(74, 90, "U3: ADP5054", fontsize=7.5, fontweight='bold', color='#1E3A8A', ha='center')
    ax.text(74, 86, "48-LFCSP", fontsize=6, color='#3B82F6', ha='center')

    # Buck outputs & Inductors L1-L4
    bucks = [
        ("CH1: SW1", "L1 (1.0uH)", 82, "TP1", "1.00V", '#10B981', '#047857', "VDD_1P0_DIG"),
        ("CH2: SW2", "L2 (2.2uH)", 74, "TP5", "3.30V", '#3B82F6', '#1D4ED8', "VCC_3P3_SYS"),
        ("CH3: SW3", "L3 (2.2uH)", 66, "TP4", "1.80V", '#8B5CF6', '#6D28D9', "VDDA_1P8_CLK"),
        ("CH4: SW4", "L4 (1.5uH)", 58, "PRE", "1.50V", '#F59E0B', '#B45309', "VREG_1P5_LDO"),
    ]
    for ch_lbl, ind_lbl, y_p, tp_name, v_val, col_fill, col_edge, net_name in bucks:
        ax.plot([84, 88], [y_p + 2, y_p + 2], color='#00008B', lw=1.2)
        # Inductor box
        p_ind = Rectangle((88, y_p), 7, 4, fill=True, facecolor='#FEF3C7', edgecolor='#B45309', lw=1)
        ax.add_patch(p_ind)
        ax.text(91.5, y_p + 2, ind_lbl[:2], fontsize=5.5, fontweight='bold', color='#78350F', ha='center', va='center')
        ax.plot([95, 98], [y_p + 2, y_p + 2], color='#00008B', lw=1.2)

        # TP marker
        if tp_name != "PRE":
            c_tp = Circle((98, y_p + 2), 1.8, fc=col_fill, ec=col_edge, lw=1)
            ax.add_patch(c_tp)
            ax.text(98, y_p + 2, tp_name, fontsize=5, fontweight='bold', color='#FFFFFF', ha='center', va='center')
            ax.text(98, y_p + 4.5, f"{v_val}", fontsize=5, fontweight='bold', color=col_edge, ha='center')

    # Net Labels on Right of Block 2
    ax.text(100, 84, ">> VDD_1P0_DIG (To U1 Core)", fontsize=6, fontweight='bold', color='#047857')
    ax.text(100, 76, ">> VCC_3P3_SYS (To System Bus)", fontsize=6, fontweight='bold', color='#1D4ED8')
    ax.text(100, 68, ">> VDDA_1P8_CLK (To U2 Clock)", fontsize=6, fontweight='bold', color='#6D28D9')
    ax.text(100, 60, ">> VREG_1P5 (To LT3045 LDOs)", fontsize=6, fontweight='bold', color='#B45309')

    # --- BLOCK 3: Ultra-Low Noise RF LDOs (Top Right) ---
    p_b3 = Rectangle((108, 54), 46, 48, fill=True, facecolor='#FFFFFF', edgecolor='#00008B', lw=1.5)
    ax.add_patch(p_b3)
    ax.text(110, 99, "BLOCK 3: LT3045 ULTRA-LOW NOISE LDOs", fontsize=7.5, fontweight='bold', color='#00008B')

    # LDO U4 (RF 1.3V)
    p_u4 = Rectangle((114, 80), 16, 14, fill=True, facecolor='#FEF3C7', edgecolor='#D97706', lw=1.2)
    ax.add_patch(p_u4)
    ax.text(122, 90, "U4: LT3045", fontsize=7, fontweight='bold', color='#92400E', ha='center')
    ax.text(122, 85, "1.3V RF LDO\nNoise: 0.8uV", fontsize=5.5, color='#B45309', ha='center')

    ax.plot([130, 136], [87, 87], color='#00008B', lw=1.2)
    c_tp2 = Circle((136, 87), 2, fc='#F59E0B', ec='#B45309', lw=1.2)
    ax.add_patch(c_tp2)
    ax.text(136, 87, "TP2", fontsize=5.5, fontweight='bold', color='#FFFFFF', ha='center', va='center')
    ax.text(136, 91.5, "1.30V RF\n(<3mV rip)", fontsize=5, fontweight='bold', color='#B45309', ha='center')
    ax.text(139, 87, ">> VDDA_1P3_RF", fontsize=5.5, fontweight='bold', color='#B45309', va='center')

    # LDO U5 (Synth 1.3V) + FB3
    p_u5 = Rectangle((114, 58), 16, 14, fill=True, facecolor='#FEF3C7', edgecolor='#D97706', lw=1.2)
    ax.add_patch(p_u5)
    ax.text(122, 68, "U5: LT3045", fontsize=7, fontweight='bold', color='#92400E', ha='center')
    ax.text(122, 63, "1.3V Synth LDO\n+ FB3 Filter", fontsize=5.5, color='#B45309', ha='center')

    ax.plot([130, 136], [65, 65], color='#00008B', lw=1.2)
    c_tp3 = Circle((136, 65), 2, fc='#F59E0B', ec='#B45309', lw=1.2)
    ax.add_patch(c_tp3)
    ax.text(136, 65, "TP3", fontsize=5.5, fontweight='bold', color='#FFFFFF', ha='center', va='center')
    ax.text(136, 69.5, "1.30V SYN\n(<2mV rip)", fontsize=5, fontweight='bold', color='#B45309', ha='center')
    ax.text(139, 65, ">> VDDA_1P3_SYNTH", fontsize=5.5, fontweight='bold', color='#B45309', va='center')

    # --- BLOCK 4: ADRV9009 Core Connections & Differential RF (Bottom Left/Center) ---
    p_b4 = Rectangle((8, 24), 88, 40, fill=True, facecolor='#FFFFFF', edgecolor='#00008B', lw=1.5)
    ax.add_patch(p_b4)
    ax.text(10, 61, "BLOCK 4: ADRV9009 (U1) POWER & DIFFERENTIAL RF CHANNELS", fontsize=7.5, fontweight='bold', color='#00008B')

    # ADRV9009 Chip Representation
    p_u1 = Rectangle((14, 28), 34, 28, fill=True, facecolor='#1E293B', edgecolor='#94A3B8', lw=1.5)
    ax.add_patch(p_u1)
    ax.text(31, 51, "U1: ADRV9009BBCZ", fontsize=8, fontweight='bold', color='#FFFFFF', ha='center')
    ax.text(31, 46, "196-Ball CSP_BGA (12x12mm)", fontsize=6, color='#94A3B8', ha='center')
    ax.text(31, 38, "Dual RF Transceiver\n75 MHz - 6.0 GHz", fontsize=6.5, color='#38BDF8', ha='center')

    # RF Baluns T1-T4 Representation
    rf_items = [
        ("T1 (Balun)", "TX1_OUT", 50, '#10B981'),
        ("T2 (Balun)", "TX2_OUT", 44, '#10B981'),
        ("T3 (Balun)", "RX1_IN", 38, '#3B82F6'),
        ("T4 (Balun)", "RX2_IN", 32, '#3B82F6'),
    ]
    for bname, port_name, yp, pcol in rf_items:
        ax.plot([48, 54], [yp, yp], color='#00008B', lw=1.2)
        p_t = Rectangle((54, yp - 2), 10, 4, fill=True, facecolor='#F1F5F9', edgecolor='#475569', lw=1)
        ax.add_patch(p_t)
        ax.text(59, yp, bname[:2], fontsize=5.5, fontweight='bold', color='#0F172A', ha='center', va='center')
        ax.plot([64, 70], [yp, yp], color='#00008B', lw=1.2)
        # Port tag
        ax.text(71, yp, f"[{port_name}]", fontsize=6, fontweight='bold', color=pcol, va='center')

    # Decoupling Array annotation under U1
    ax.text(52, 26, "Decoupling: 0402 0.1uF X7R on each VDD pin directly under BGA", fontsize=6, color='#64748B')

    # Diagnostic Callout Box on Left
    p_call = FancyBboxPatch((8, 6), 88, 16, boxstyle="round,pad=0.2,rounding_size=0.6",
                            fc='#EFF6FF', ec='#2563EB', lw=1.2)
    ax.add_patch(p_call)
    ax.text(10, 18.5, "ALTIUM SCHEMATIC REPAIR CHECK TRACK:", fontsize=7.5, fontweight='bold', color='#1E40AF')
    ax.text(10, 14.5, "1. Power Entry: If TP6 == 0V, verify Fuse F1 (5A) and check TVS Diode D1 for ground short.", fontsize=6.5, color='#1E293B')
    ax.text(10, 11.5, "2. Buck PMIC: Verify SW1-SW4 switching at ADP5054 pins. If TP1 < 0.95V, check L1 inductor solder joint.", fontsize=6.5, color='#1E293B')
    ax.text(10, 8.5, "3. RF LDOs: Probe TP2 & TP3. High ripple (>5mV) indicates ceramic output cap failure or LDO oscillation.", fontsize=6.5, color='#1E293B')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "altium_job1_schematic_checkpoints.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 2. Altium Output Job 2: PCB Top & Bottom Layer Check Points & Routing Traces
# ==============================================================================
def generate_altium_job2_pcb_layers():
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(18, 10), dpi=300)
    fig.patch.set_facecolor('#0F172A')

    for ax, layer_name, bg_col, is_top in [(ax1, "TOP LAYER ASSEMBLY (JOB 2A)", "#0B2014", True),
                                           (ax2, "BOTTOM LAYER ASSEMBLY (JOB 2B)", "#05130B", False)]:
        ax.set_facecolor(bg_col)
        ax.set_xlim(-5, 145)
        ax.set_ylim(-5, 105)
        ax.axis('off')

        # Board outline
        board = FancyBboxPatch((0, 0), 140, 100, boxstyle="round,pad=0.5,rounding_size=2.5",
                               fc='#133820', ec='#D4AF37', lw=3)
        ax.add_patch(board)

        # Mounting holes
        for mx, my in [(6, 6), (134, 6), (6, 94), (134, 94)]:
            c_outer = Circle((mx, my), 3.2, fc='#D4AF37', ec='#854D0E', lw=1.2)
            c_inner = Circle((mx, my), 2.0, fc='#0F172A', ec='#D4AF37', lw=0.8)
            ax.add_patch(c_outer)
            ax.add_patch(c_inner)

        # Title on PCB Silkscreen
        ax.text(70, 96, f"ALTIUM OUTJOB 2: {layer_name}", ha='center', va='center',
                fontsize=9.5, fontweight='bold', color='#FFFFFF')

        if is_top:
            ax.text(70, 92, "Component Silkscreen, RF Traces & Test Check Points", ha='center', va='center',
                    fontsize=7.5, color='#A7F3D0')

            # Top Layer: SMA Connectors & RF Microstrip lines
            rf_ports = [("TX1", 25), ("TX2", 48), ("RX1", 75), ("RX2", 98), ("ORX", 120)]
            for pname, px in rf_ports:
                p_sma = Rectangle((px - 5, 82), 10, 8, fc='#D4AF37', ec='#FDE047', lw=1.2)
                ax.add_patch(p_sma)
                ax.text(px, 86, pname, ha='center', va='center', fontsize=6, fontweight='bold', color='#000000')

                # Balun
                p_bal = Rectangle((px - 3, 72), 6, 5, fc='#334155', ec='#94A3B8', lw=1)
                ax.add_patch(p_bal)
                ax.text(px, 74.5, "Balun", ha='center', va='center', fontsize=4.5, color='#FFFFFF')
                # RF Coplanar Microstrip Trace (Gold)
                ax.plot([px, px, 70], [72, 64, 58], color='#FDE047', lw=2)

            # Central ADRV9009 (U1)
            p_u1 = FancyBboxPatch((55, 36), 30, 24, boxstyle="round,pad=0.2,rounding_size=0.6",
                                  fc='#1E293B', ec='#E2E8F0', lw=2)
            ax.add_patch(p_u1)
            c_p1 = Circle((58, 57), 1.0, fc='#FFFFFF')
            ax.add_patch(c_p1)
            ax.text(70, 50, "U1: ADRV9009", ha='center', va='center', fontsize=9, fontweight='bold', color='#FFFFFF')
            ax.text(70, 44, "196-CSP_BGA\nCheck Pin 1 Orientation", ha='center', va='center', fontsize=6.5, color='#94A3B8')

            # Top Power Entry: Fuse F1 & TVS D1 & TP6
            p_j = Rectangle((6, 62), 10, 12, fc='#334155', ec='#E2E8F0', lw=1.2)
            ax.add_patch(p_j)
            ax.text(11, 68, "12V IN", ha='center', va='center', fontsize=6, fontweight='bold', color='#FFFFFF')

            p_f = Rectangle((10, 50), 7, 4.5, fc='#EAB308', ec='#CA8A04', lw=1.2)
            ax.add_patch(p_f)
            ax.text(13.5, 52.2, "F1: 5A", ha='center', va='center', fontsize=5.5, fontweight='bold', color='#000000')

            p_d = Rectangle((10, 40), 7, 4.5, fc='#EF4444', ec='#991B1B', lw=1.2)
            ax.add_patch(p_d)
            ax.text(13.5, 42.2, "D1: TVS", ha='center', va='center', fontsize=5.5, fontweight='bold', color='#FFFFFF')

            # Top Test Points
            tps_top = [
                (20, 52.2, "TP6", '#DC2626', "12V IN"),
                (50, 48, "TP1", '#10B981', "1.0V DIG"),
                (90, 52, "TP2", '#F59E0B', "1.3V RF"),
                (90, 40, "TP3", '#F59E0B', "1.3V SYN"),
                (38, 20, "TP4", '#8B5CF6', "1.8V CLK"),
                (112, 22, "TP5", '#3B82F6', "3.3V SYS")
            ]
            for tx, ty, tname, tcol, tlbl in tps_top:
                c_t = Circle((tx, ty), 2.5, fc=tcol, ec='#FFFFFF', lw=1.5)
                ax.add_patch(c_t)
                ax.text(tx, ty, tname, ha='center', va='center', fontsize=5.5, fontweight='bold', color='#FFFFFF')
                ax.text(tx, ty - 4.5, tlbl, ha='center', va='center', fontsize=5, fontweight='bold', color='#F8FAFC')

            # FMC Connector (J1)
            p_fmc = Rectangle((55, 4), 65, 8, fc='#334155', ec='#D4AF37', lw=1.2)
            ax.add_patch(p_fmc)
            ax.text(87.5, 8, "J1: HIGH-SPEED FMC CONNECTOR (JESD204B & SPI)",
                    ha='center', va='center', fontsize=6, fontweight='bold', color='#FDE047')

        else:
            ax.text(70, 92, "ADP5054 PMIC, LT3045 LDOs & Decoupling Array", ha='center', va='center',
                    fontsize=7.5, color='#A7F3D0')

            # Bottom Layer: ADP5054 (U3) & Inductors L1-L4
            p_u3 = FancyBboxPatch((15, 45), 20, 20, boxstyle="round,pad=0.2,rounding_size=0.6",
                                  fc='#1E293B', ec='#3B82F6', lw=1.8)
            ax.add_patch(p_u3)
            ax.text(25, 57, "U3: ADP5054", ha='center', va='center', fontsize=8, fontweight='bold', color='#60A5FA')
            ax.text(25, 52, "Quad Buck PMIC", ha='center', va='center', fontsize=6, color='#94A3B8')

            # Inductors L1-L4
            for lidx, (lname, ly, lrail) in enumerate([("L1 (1.0uH)", 58, "1.0V"), ("L2 (2.2uH)", 48, "3.3V"),
                                                       ("L3 (2.2uH)", 38, "1.8V"), ("L4 (1.5uH)", 28, "1.5V")]):
                p_l = Rectangle((38, ly), 11, 6.5, fc='#0F172A', ec='#CA8A04', lw=1.2)
                ax.add_patch(p_l)
                ax.text(43.5, ly + 4, lname, ha='center', va='center', fontsize=5.5, fontweight='bold', color='#FDE047')
                ax.text(43.5, ly + 1.8, lrail, ha='center', va='center', fontsize=4.5, color='#94A3B8')

            # Bottom LDOs U4 & U5
            p_u4 = FancyBboxPatch((98, 55), 16, 12, boxstyle="round,pad=0.2,rounding_size=0.5",
                                  fc='#1E293B', ec='#F59E0B', lw=1.5)
            ax.add_patch(p_u4)
            ax.text(106, 62, "U4: LT3045\n1.3V RF LDO", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FCD34D')

            p_u5 = FancyBboxPatch((98, 35), 16, 12, boxstyle="round,pad=0.2,rounding_size=0.5",
                                  fc='#1E293B', ec='#F59E0B', lw=1.5)
            ax.add_patch(p_u5)
            ax.text(106, 42, "U5: LT3045\n1.3V SYNTH LDO", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FCD34D')

            # Underside Decoupling Matrix under U1 BGA
            p_bga_sub = FancyBboxPatch((55, 36), 30, 24, boxstyle="round,pad=0.2,rounding_size=0.6",
                                       fc='#164E2E', ec='#D4AF37', lw=1.2, ls='--')
            ax.add_patch(p_bga_sub)
            ax.text(70, 56, "UNDERSIDE OF U1 BGA", ha='center', va='center', fontsize=7, fontweight='bold', color='#FDE047')
            for cx in np.linspace(58, 82, 6):
                for cy in np.linspace(40, 50, 3):
                    c_cap = Rectangle((cx - 0.8, cy - 0.6), 1.6, 1.2, fc='#CBD5E1', ec='#475569', lw=0.4)
                    ax.add_patch(c_cap)
            ax.text(70, 34, "0402 Decoupling Matrix (0.1uF NPO)", ha='center', va='center', fontsize=5.5, color='#CBD5E1')

            # Bottom Test Rings
            tps_bot = [
                (60, 20, "TP1", '#10B981', "1.0V DIG"),
                (118, 61, "TP2", '#F59E0B', "1.3V RF"),
                (118, 41, "TP3", '#F59E0B', "1.3V SYN"),
                (20, 20, "TP4", '#8B5CF6', "1.8V CLK"),
                (20, 75, "TP5", '#3B82F6', "3.3V SYS"),
                (10, 85, "TP6", '#EF4444', "12V MAIN")
            ]
            for tx, ty, tname, tcol, tlbl in tps_bot:
                c_t = Circle((tx, ty), 2.8, fc=tcol, ec='#FFFFFF', lw=1.5)
                ax.add_patch(c_t)
                ax.text(tx, ty, tname, ha='center', va='center', fontsize=5.5, fontweight='bold', color='#FFFFFF')
                ax.text(tx, ty - 4.5, tlbl, ha='center', va='center', fontsize=5, fontweight='bold', color='#F8FAFC')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "altium_job2_pcb_assembly_top_bottom.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 3. RF Test Bench Setup & Measurement Instruments Diagram
# ==============================================================================
def generate_rf_test_bench_diagram():
    fig, ax = plt.subplots(figsize=(15, 9), dpi=300)
    ax.set_facecolor('#F8FAFC')
    fig.patch.set_facecolor('#FFFFFF')
    ax.set_xlim(0, 150)
    ax.set_ylim(0, 95)
    ax.axis('off')

    # Title & Subtitle
    ax.text(75, 91, "ADRV9009 RF TEST & MEASUREMENT BENCH SETUP",
            ha='center', va='center', fontsize=14, fontweight='bold', color='#0F172A')
    ax.text(75, 87, "Laboratory Instrumentation, Cable Connections, Calibrated Attenuators & Control Setup",
            ha='center', va='center', fontsize=9, color='#475569')

    # Central DUT Box (ADRV9009 Board)
    p_dut = FancyBboxPatch((55, 30), 40, 36, boxstyle="round,pad=0.3,rounding_size=1.2",
                           fc='#133820', ec='#D4AF37', lw=2.5)
    ax.add_patch(p_dut)
    ax.text(75, 60, "DEVICE UNDER TEST (DUT)", ha='center', va='center', fontsize=9, fontweight='bold', color='#FDE047')
    ax.text(75, 54, "ADRV9009 Transceiver Board", ha='center', va='center', fontsize=10, fontweight='bold', color='#FFFFFF')
    ax.text(75, 48, "Zynq UltraScale+ Baseband Carrier", ha='center', va='center', fontsize=8, color='#A7F3D0')
    ax.text(75, 38, "[TX1]   [TX2]   [RX1]   [RX2]   [ORX]", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FCD34D')

    # Instrument 1: DC Power Supply (Top Left)
    p_psu = FancyBboxPatch((6, 64), 36, 18, boxstyle="round,pad=0.2,rounding_size=0.8",
                           fc='#F1F5F9', ec='#475569', lw=1.5)
    ax.add_patch(p_psu)
    ax.text(24, 78, "PRECISION DC POWER SUPPLY", ha='center', va='center', fontsize=8, fontweight='bold', color='#0F172A')
    ax.text(24, 73, "Keysight E36313A / Rigol DP832", ha='center', va='center', fontsize=7, color='#475569')
    ax.text(24, 68, "Output: +12.00 VDC @ 3.0A (Limit: 5A)", ha='center', va='center', fontsize=7, fontweight='bold', color='#B91C1C')
    # Cable to DUT
    ax.plot([42, 55], [71, 56], color='#DC2626', lw=2.5)
    ax.text(48, 66, "12V DC\nPower", fontsize=6.5, fontweight='bold', color='#DC2626')

    # Instrument 2: Vector Signal Generator (Bottom Left - RX Testing)
    p_vsg = FancyBboxPatch((6, 10), 36, 22, boxstyle="round,pad=0.2,rounding_size=0.8",
                           fc='#EFF6FF', ec='#2563EB', lw=1.5)
    ax.add_patch(p_vsg)
    ax.text(24, 28, "VECTOR SIGNAL GENERATOR (VSG)", ha='center', va='center', fontsize=8, fontweight='bold', color='#1E40AF')
    ax.text(24, 23, "Rohde & Schwarz SMBV100B / Agilent N5182A", ha='center', va='center', fontsize=6.5, color='#3B82F6')
    ax.text(24, 17, "Freq: 75 MHz – 6.0 GHz | Pout: -80 to +10 dBm\nCW Tone / LTE 20MHz QAM64 Waveform", ha='center', va='center', fontsize=6.5, color='#1E3A8A')
    # Cable to DUT RX1
    ax.plot([42, 55], [21, 35], color='#2563EB', lw=2.5)
    ax.text(48, 26, "RF IN (RX1)\n50Ω Coax", fontsize=6.5, fontweight='bold', color='#2563EB')

    # Instrument 3: Spectrum & Signal Analyzer (Top Right - TX Testing)
    p_vsa = FancyBboxPatch((108, 62), 36, 22, boxstyle="round,pad=0.2,rounding_size=0.8",
                           fc='#FEF2F2', ec='#DC2626', lw=1.5)
    ax.add_patch(p_vsa)
    ax.text(126, 80, "SPECTRUM ANALYZER (VSA)", ha='center', va='center', fontsize=8, fontweight='bold', color='#991B1B')
    ax.text(126, 75, "Keysight N9020B (MXA) / FSW26", ha='center', va='center', fontsize=6.5, color='#B91C1C')
    ax.text(126, 69, "Span: 10 Hz – 13.6 GHz | RBW: 1 kHz - 3 MHz\nPhase Noise, ACPR, EVM, LOL, Harmonics", ha='center', va='center', fontsize=6.5, color='#7F1D1D')
    # Attenuator Pad & Cable to DUT TX1
    p_att = Rectangle((98, 52), 7, 5, fc='#FEF08A', ec='#CA8A04', lw=1.2)
    ax.add_patch(p_att)
    ax.text(101.5, 54.5, "20dB\nATT", ha='center', va='center', fontsize=5.5, fontweight='bold', color='#854D0E')
    ax.plot([95, 98], [54.5, 54.5], color='#DC2626', lw=2.5)
    ax.plot([105, 108], [54.5, 71], color='#DC2626', lw=2.5)
    ax.text(102, 62, "TX1_OUT\n(+7dBm)", fontsize=6.5, fontweight='bold', color='#DC2626')

    # Instrument 4: Host PC / eLynxSDR Workstation (Bottom Right)
    p_pc = FancyBboxPatch((108, 10), 36, 22, boxstyle="round,pad=0.2,rounding_size=0.8",
                          fc='#ECFDF5', ec='#059669', lw=1.5)
    ax.add_patch(p_pc)
    ax.text(126, 28, "HOST WORKSTATION (eLynxSDR)", ha='center', va='center', fontsize=8, fontweight='bold', color='#065F46')
    ax.text(126, 23, "Linux Ubuntu / eLynxSDR Software Suite", ha='center', va='center', fontsize=6.5, color='#047857')
    ax.text(126, 17, "Gigabit Ethernet (192.168.1.10) / IIO API\nUSB UART Console (/dev/ttyUSB0 115200)", ha='center', va='center', fontsize=6.5, color='#064E3B')
    # Cable to DUT Ethernet & UART
    ax.plot([95, 108], [35, 21], color='#059669', lw=2, ls='--')
    ax.text(100, 26, "GbE LAN &\nUART", fontsize=6.5, fontweight='bold', color='#059669')

    # 10 MHz Reference Clock Synchronization Distribution (Dashed Line)
    ax.plot([24, 24, 126, 126], [64, 4, 4, 62], color='#7C3AED', lw=1.5, ls=':')
    ax.text(75, 6, "10.000 MHz MASTER REFERENCE CLOCK (BNC 50Ω DAISY-CHAIN SYNCHRONIZATION)",
            ha='center', va='center', fontsize=7, fontweight='bold', color='#7C3AED')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "rf_test_bench_setup_diagram.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

# ==============================================================================
# 4. Power Tree & Test Points Schematic Diagram
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
# 5. PCB Top Layer Layout & Test Point Callout Diagram
# ==============================================================================
def generate_pcb_top_layer():
    fig, ax = plt.subplots(figsize=(14, 10), dpi=300)
    ax.set_facecolor('#0B2014')  # Dark PCB Green
    fig.patch.set_facecolor('#05130B')
    ax.set_xlim(-5, 145)
    ax.set_ylim(-5, 105)
    ax.axis('off')

    board = FancyBboxPatch((0, 0), 140, 100, boxstyle="round,pad=0.5,rounding_size=3",
                           fc='#133820', ec='#D4AF37', lw=3.5)
    ax.add_patch(board)

    for mx, my in [(6, 6), (134, 6), (6, 94), (134, 94)]:
        c_outer = Circle((mx, my), 3.5, fc='#D4AF37', ec='#854D0E', lw=1.5)
        c_inner = Circle((mx, my), 2.2, fc='#05130B', ec='#D4AF37', lw=1)
        ax.add_patch(c_outer)
        ax.add_patch(c_inner)

    ax.text(70, 96, "ADRV9009 EVALUATION BOARD - TOP LAYER COMPONENT & TEST POINT MAP",
            ha='center', va='center', fontsize=11, fontweight='bold', color='#FFFFFF')
    ax.text(70, 92.5, "IPC-7711/7721 Hardware Diagnostics & Voltage Check Points Reference",
            ha='center', va='center', fontsize=8, color='#A7F3D0')

    rf_connectors = [
        ("TX1_OUT", 25, "TX1 Out (Balun T1)"),
        ("TX2_OUT", 48, "TX2 Out (Balun T2)"),
        ("RX1_IN", 75, "RX1 In (Balun T3)"),
        ("RX2_IN", 98, "RX2 In (Balun T4)"),
        ("ORX_IN", 120, "ORX In (Balun T5)")
    ]
    for name, x_pos, desc in rf_connectors:
        p_sma = FancyBboxPatch((x_pos - 6, 82), 12, 8, boxstyle="round,pad=0.2,rounding_size=0.5",
                               fc='#D4AF37', ec='#FDE047', lw=1.5)
        ax.add_patch(p_sma)
        c_pin = Circle((x_pos, 86), 1.5, fc='#FFFFFF', ec='#854D0E', lw=1)
        ax.add_patch(c_pin)
        ax.text(x_pos, 86, name, ha='center', va='center', fontsize=6.5, fontweight='bold', color='#000000')

        p_balun = Rectangle((x_pos - 3.5, 71), 7, 6, fc='#334155', ec='#94A3B8', lw=1)
        ax.add_patch(p_balun)
        ax.text(x_pos, 74, "Balun\n" + name.split('_')[0], ha='center', va='center', fontsize=5.5, color='#F8FAFC')
        ax.plot([x_pos, x_pos, 70], [71, 62, 58], color='#D4AF37', lw=1.8)

    p_u1 = FancyBboxPatch((55, 36), 30, 24, boxstyle="round,pad=0.3,rounding_size=0.8",
                          fc='#1E293B', ec='#E2E8F0', lw=2)
    ax.add_patch(p_u1)
    c_pin1 = Circle((58, 57), 1.2, fc='#FFFFFF')
    ax.add_patch(c_pin1)
    ax.text(70, 50, "U1: ADRV9009BBCZ", ha='center', va='center', fontsize=9.5, fontweight='bold', color='#FFFFFF')
    ax.text(70, 44, "196-Ball CSP_BGA (12x12mm)\nDual RF Transceiver", ha='center', va='center', fontsize=7, color='#94A3B8')

    p_jack = FancyBboxPatch((6, 60), 12, 14, boxstyle="round,pad=0.2,rounding_size=0.8",
                            fc='#334155', ec='#E2E8F0', lw=1.5)
    ax.add_patch(p_jack)
    ax.text(12, 67, "12V IN\nJ_PWR", ha='center', va='center', fontsize=7, fontweight='bold', color='#FFFFFF')

    p_f1 = Rectangle((10, 46), 7, 5, fc='#EAB308', ec='#CA8A04', lw=1.5)
    ax.add_patch(p_f1)
    ax.text(13.5, 48.5, "F1: 5A", ha='center', va='center', fontsize=6, fontweight='bold', color='#000000')

    p_d1 = Rectangle((10, 35), 7, 5, fc='#EF4444', ec='#991B1B', lw=1.5)
    ax.add_patch(p_d1)
    ax.text(13.5, 37.5, "D1: TVS", ha='center', va='center', fontsize=6, fontweight='bold', color='#FFFFFF')

    c_tp6 = Circle((22, 48.5), 2.8, fc='#DC2626', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp6)
    ax.text(22, 48.5, "TP6", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    p_u2 = FancyBboxPatch((18, 14), 18, 14, boxstyle="round,pad=0.2,rounding_size=0.6",
                          fc='#1E293B', ec='#60A5FA', lw=1.5)
    ax.add_patch(p_u2)
    ax.text(27, 22, "U2: HMC7044", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#93C5FD')
    ax.text(27, 17, "Clock Gen / Jitter Cleaner", ha='center', va='center', fontsize=5.5, color='#CBD5E1')

    p_y1 = Rectangle((6, 16), 8, 6, fc='#64748B', ec='#E2E8F0', lw=1)
    ax.add_patch(p_y1)
    ax.text(10, 19, "Y1: 40M\nTCXO", ha='center', va='center', fontsize=5, fontweight='bold', color='#FFFFFF')

    c_tp4 = Circle((39, 21), 2.8, fc='#8B5CF6', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp4)
    ax.text(39, 21, "TP4", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    c_clk_p = Circle((39, 14), 1.8, fc='#D4AF37', ec='#FFFFFF', lw=1)
    c_clk_n = Circle((44, 14), 1.8, fc='#D4AF37', ec='#FFFFFF', lw=1)
    ax.add_patch(c_clk_p)
    ax.add_patch(c_clk_n)
    ax.text(41.5, 9.5, "CLK_DIFF (122.88M)", ha='center', va='center', fontsize=5.5, color='#E2E8F0')

    p_fmc = FancyBboxPatch((55, 4), 65, 8, boxstyle="round,pad=0.2,rounding_size=0.5",
                           fc='#334155', ec='#D4AF37', lw=1.5)
    ax.add_patch(p_fmc)
    ax.text(87.5, 8, "J1: HIGH-SPEED FMC CONNECTOR (JESD204B SERDES & CONTROL BUS)",
            ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FDE047')

    c_tp1 = Circle((50, 48), 2.8, fc='#10B981', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp1)
    ax.text(50, 48, "TP1", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    c_tp2 = Circle((90, 52), 2.8, fc='#F59E0B', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp2)
    ax.text(90, 52, "TP2", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    c_tp3 = Circle((90, 40), 2.8, fc='#F59E0B', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp3)
    ax.text(90, 40, "TP3", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    p_u6 = Rectangle((100, 20), 8, 6, fc='#1E293B', ec='#94A3B8', lw=1)
    ax.add_patch(p_u6)
    ax.text(104, 23, "U6\nEEPROM", ha='center', va='center', fontsize=5.5, color='#F8FAFC')
    c_tp5 = Circle((112, 23), 2.8, fc='#3B82F6', ec='#FFFFFF', lw=1.8)
    ax.add_patch(c_tp5)
    ax.text(112, 23, "TP5", ha='center', va='center', fontsize=6.5, fontweight='bold', color='#FFFFFF')

    for gx, gy, gname in [(30, 48.5, "GND1"), (112, 35, "GND2")]:
        c_gnd = Circle((gx, gy), 2.5, fc='#000000', ec='#D4AF37', lw=1.5)
        ax.add_patch(c_gnd)
        ax.text(gx, gy, "GND", ha='center', va='center', fontsize=5.5, fontweight='bold', color='#D4AF37')

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
# 6. PCB Bottom Layer Layout & Power IC Callout Diagram
# ==============================================================================
def generate_pcb_bottom_layer():
    fig, ax = plt.subplots(figsize=(14, 10), dpi=300)
    ax.set_facecolor('#0B2014')  # Dark PCB Green
    fig.patch.set_facecolor('#05130B')
    ax.set_xlim(-5, 145)
    ax.set_ylim(-5, 105)
    ax.axis('off')

    board = FancyBboxPatch((0, 0), 140, 100, boxstyle="round,pad=0.5,rounding_size=3",
                           fc='#133820', ec='#D4AF37', lw=3.5)
    ax.add_patch(board)

    for mx, my in [(6, 6), (134, 6), (6, 94), (134, 94)]:
        c_outer = Circle((mx, my), 3.5, fc='#D4AF37', ec='#854D0E', lw=1.5)
        c_inner = Circle((mx, my), 2.2, fc='#05130B', ec='#D4AF37', lw=1)
        ax.add_patch(c_outer)
        ax.add_patch(c_inner)

    ax.text(70, 96, "ADRV9009 EVALUATION BOARD - BOTTOM LAYER POWER IC & DECOUPLING MAP",
            ha='center', va='center', fontsize=11, fontweight='bold', color='#FFFFFF')
    ax.text(70, 92.5, "ADP5054 Buck PMIC, LT3045 LDO Regulators & High-Frequency Decoupling Array",
            ha='center', va='center', fontsize=8, color='#A7F3D0')

    p_bga_sub = FancyBboxPatch((55, 36), 30, 24, boxstyle="round,pad=0.3,rounding_size=0.8",
                               fc='#164E2E', ec='#D4AF37', lw=1.5, ls='--')
    ax.add_patch(p_bga_sub)
    ax.text(70, 56, "UNDERSIDE OF U1 (ADRV9009)", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FDE047')
    
    for cx in np.linspace(58, 82, 7):
        for cy in np.linspace(39, 51, 4):
            c_cap = Rectangle((cx - 1, cy - 0.7), 2, 1.4, fc='#CBD5E1', ec='#475569', lw=0.5)
            ax.add_patch(c_cap)
    ax.text(70, 34, "0402 / 0201 Bypass Caps (0.1uF / 10nF / 1nF NPO)", ha='center', va='center', fontsize=6, color='#CBD5E1')

    p_u3 = FancyBboxPatch((15, 45), 20, 20, boxstyle="round,pad=0.3,rounding_size=0.8",
                          fc='#1E293B', ec='#3B82F6', lw=2)
    ax.add_patch(p_u3)
    ax.text(25, 57, "U3: ADP5054", ha='center', va='center', fontsize=8.5, fontweight='bold', color='#60A5FA')
    ax.text(25, 52, "Quad Buck PMIC\n(48-LFCSP 7x7mm)", ha='center', va='center', fontsize=6.5, color='#94A3B8')

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

    p_u4 = FancyBboxPatch((98, 55), 16, 12, boxstyle="round,pad=0.2,rounding_size=0.6",
                          fc='#1E293B', ec='#F59E0B', lw=1.8)
    ax.add_patch(p_u4)
    ax.text(106, 63, "U4: LT3045", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FCD34D')
    ax.text(106, 59, "1.3V RF LDO\n(0.8 uVrms noise)", ha='center', va='center', fontsize=6, color='#CBD5E1')

    p_u5 = FancyBboxPatch((98, 35), 16, 12, boxstyle="round,pad=0.2,rounding_size=0.6",
                          fc='#1E293B', ec='#F59E0B', lw=1.8)
    ax.add_patch(p_u5)
    ax.text(106, 43, "U5: LT3045", ha='center', va='center', fontsize=7.5, fontweight='bold', color='#FCD34D')
    ax.text(106, 39, "1.3V Synth LDO\n+ FB3 Filter", ha='center', va='center', fontsize=6, color='#CBD5E1')

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
# 7. ADRV9009 BGA Rework Thermal Profile Chart
# ==============================================================================
def generate_bga_rework_profile():
    fig, ax = plt.subplots(figsize=(12, 7), dpi=300)
    ax.set_facecolor('#F8FAFC')
    fig.patch.set_facecolor('#FFFFFF')

    time = np.array([0, 30, 80, 110, 160, 190, 210, 225, 240, 255, 275, 300])
    temp = np.array([25, 80, 150, 170, 195, 217, 240, 243, 235, 217, 130, 45])

    ax.plot(time, temp, color='#2563EB', lw=3, label='Thermocouple Solder Joint Temp (°C)')
    ax.scatter(time, temp, color='#1D4ED8', s=45, zorder=5)

    ax.axvspan(0, 100, color='#EFF6FF', alpha=0.8, label='1. Preheat Zone (+1.5°C/s)')
    ax.axvspan(100, 185, color='#FEF3C7', alpha=0.8, label='2. Soak / Flux Activation (170-200°C)')
    ax.axvspan(185, 255, color='#FEE2E2', alpha=0.8, label='3. Reflow Peak (235-245°C, TAL 45s)')
    ax.axvspan(255, 300, color='#ECFDF5', alpha=0.8, label='4. Controlled Cooling (-2.5°C/s)')

    ax.axhline(217, color='#DC2626', ls='--', lw=1.8, label='Liquidus Temp TL = 217°C (SAC305)')
    ax.text(10, 220, "Liquidus Line (217°C SAC305 Lead-Free)", fontsize=8, fontweight='bold', color='#DC2626')

    ax.annotate('Peak Temp: 243°C\n(TAL: 45s, Max 245°C)', xy=(225, 243), xytext=(205, 265),
                arrowprops=dict(facecolor='#DC2626', shrink=0.08, width=1.5, headwidth=6),
                fontsize=8.5, fontweight='bold', color='#B91C1C', ha='center',
                bbox=dict(boxstyle='round,pad=0.3', fc='#FEF2F2', ec='#DC2626', lw=1.2))

    ax.annotate('Vacuum Pickup Activation\n(T >= 235°C)', xy=(220, 238), xytext=(150, 250),
                arrowprops=dict(facecolor='#7C3AED', shrink=0.08, width=1.2, headwidth=5),
                fontsize=8, fontweight='bold', color='#6D28D9', ha='center',
                bbox=dict(boxstyle='round,pad=0.2', fc='#F5F3FF', ec='#7C3AED', lw=1))

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
# 8. Hardware Troubleshooting & Diagnostic Flowchart
# ==============================================================================
def generate_troubleshooting_flowchart():
    fig, ax = plt.subplots(figsize=(14, 11), dpi=300)
    ax.set_facecolor('#F8FAFC')
    fig.patch.set_facecolor('#FFFFFF')
    ax.set_xlim(0, 140)
    ax.set_ylim(0, 110)
    ax.axis('off')

    ax.text(70, 106, "ADRV9009 HARDWARE SYSTEM TROUBLESHOOTING & REPAIR FLOWCHART",
            ha='center', va='center', fontsize=14, fontweight='bold', color='#0F172A')
    ax.text(70, 102.5, "Step-by-Step Diagnostic Sequence from Power Entry to RF Transmission & Clock Lock",
            ha='center', va='center', fontsize=9, color='#475569')

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

    draw_box(55, 88, 30, 9, "START: Power Up", "Connect 12.0 VDC to J_PWR", '#EFF6FF', '#2563EB', '#1E40AF')
    ax.arrow(70, 88, 0, -4, head_width=2, head_length=1.5, fc='#1E293B', ec='#1E293B')

    draw_decision(52, 69, 36, 13, "TP6 == 12.0V ?", "Measure TP6 vs GND")

    ax.arrow(52, 75.5, -12, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(45, 77.5, "NO (0V)", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 71, 36, 11, "POWER ENTRY FAILURE", "Replace Fuse F1 (5A 1206)\nCheck TVS D1 for short circuit", '#FEE2E2', '#DC2626', '#991B1B')

    ax.arrow(70, 69, 0, -5, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(72, 66.5, "YES", fontsize=7.5, fontweight='bold', color='#059669')

    draw_decision(48, 47, 44, 14, "TP1 (1.0V) & TP5 (3.3V) OK?", "Measure Buck 1 & Buck 2 Output")

    ax.arrow(48, 54, -10, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(42, 56, "NO", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 49, 34, 11, "PMIC BUCK FAILURE", "Inspect ADP5054 (U3)\nCheck Inductors L1, L2 & short to GND", '#FEE2E2', '#DC2626', '#991B1B')

    ax.arrow(70, 47, 0, -5, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(72, 44.5, "YES", fontsize=7.5, fontweight='bold', color='#059669')

    draw_decision(48, 25, 44, 14, "TP2 & TP3 == 1.30V?", "Measure LDO U4/U5 (LT3045)")

    ax.arrow(48, 32, -10, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(42, 34, "NO", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 27, 34, 11, "RF LDO FAILURE", "Inspect LT3045 U4/U5\nVerify FB3 Ferrite Bead & 1.5V In", '#FEE2E2', '#DC2626', '#991B1B')

    ax.arrow(70, 25, 0, -5, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(72, 22.5, "YES", fontsize=7.5, fontweight='bold', color='#059669')

    draw_decision(46, 3, 48, 14, "HMC7044 Locked & JESD204B OK?", "Check TP4 (1.8V), SYSREF & SYNC~")

    ax.arrow(46, 10, -8, 0, head_width=2, head_length=1.5, fc='#DC2626', ec='#DC2626')
    ax.text(40, 12, "NO", fontsize=7.5, fontweight='bold', color='#DC2626')
    draw_box(4, 5, 34, 11, "CLOCK / SYNC FAILURE", "Verify SiT5356 40MHz TCXO\nInspect U2 (HMC7044) & JESD lanes", '#FEE2E2', '#DC2626', '#991B1B')

    ax.arrow(94, 10, 10, 0, head_width=2, head_length=1.5, fc='#059669', ec='#059669')
    ax.text(97, 12, "YES", fontsize=7.5, fontweight='bold', color='#059669')
    draw_box(106, 5, 30, 11, "SYSTEM FULLY OPERATIONAL", "RF Transceiver U1 Operational\nExecute BIT & RF Loopback Test", '#ECFDF5', '#059669', '#065F46')

    draw_box(96, 68, 40, 30, "KEY DIAGNOSTIC LIMITS",
             "• TP6 (12V Input): 10.8V to 13.2V\n• TP1 (Digital 1.0V): 0.95V to 1.05V\n• TP5 (System 3.3V): 3.14V to 3.46V\n• TP4 (Clock 1.8V): 1.71V to 1.89V\n• TP2 (RF 1.3V): 1.24V to 1.36V (<3mV rip)\n• TP3 (Synth 1.3V): 1.24V to 1.36V (<2mV rip)",
             '#FFFFFF', '#94A3B8', '#0F172A')

    plt.tight_layout()
    output_path = os.path.join(OUTPUT_DIR, "adrv9009_hardware_troubleshooting_flowchart.png")
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"Generated: {output_path}")

if __name__ == '__main__':
    print("Generating Complete ADRV9009 Engineering Diagrams & Test Bench Figures...")
    generate_altium_job1_schematic()
    generate_altium_job2_pcb_layers()
    generate_rf_test_bench_diagram()
    generate_power_tree_schematic()
    generate_pcb_top_layer()
    generate_pcb_bottom_layer()
    generate_bga_rework_profile()
    generate_troubleshooting_flowchart()
    print("All 8 diagrams generated successfully!")
