#!/usr/bin/env python3
"""
generate.py -- band-limited "spot" noise generator for iiS (Phase 5).

Generates the ADRV9009 spot-noise waveform files that the Exciter's Spot
tab loads and plays into the DAC buffer:

    spot{N}mhz_{P}.txt      P in {100, 200, 400},   N = 1..P

i.e. 100 + 200 + 400 = 700 files:

    spot1mhz_100.txt ... spot100mhz_100.txt     (100 MHz ADRV9009 profile)
    spot1mhz_200.txt ... spot200mhz_200.txt     (200 MHz ADRV9009 profile)
    spot1mhz_400.txt ... spot400mhz_400.txt     (400 MHz ADRV9009 profile)

File format (same family as the legacy spot5mhz.txt waveform file that
the iio-oscilloscope DAC loader already accepts):

    TEXT
    <I> <Q>
    <I> <Q>
    ...

    * header line : "TEXT"
    * 2 columns   : in-phase and quadrature floats, 6 decimal places
    * sample count: 262144 (2^18) per file

Spectral convention
-------------------
Each file is complex (I/Q) Gaussian noise confined to a flat band of
N MHz centred at DC (baseband):

    -P/2 MHz ... -N/2 MHz ... 0 ... N/2 MHz ... +P/2 MHz
                |_____________|
                 band = N MHz

The file's assumed sample rate is Fs = P MHz (P = the ADRV9009 profile
the file belongs to), so every N <= P fits inside the Nyquist band
[-P/2, +P/2] MHz.  The largest file of a set (spot{P}mhz_{P}.txt) is
full-band white noise for that profile.

The band edge is shaped like a high-order low-pass filter, not a soft
ramp:

  * N >= 12 MHz: flat passband (ripple well under 1 dB) up to the
    nominal edge, exactly -3 dB at +/-N/2 MHz, then a smooth rolloff
    that reaches the noise floor by +/-(N/2 + 10) MHz (more than
    60 dB rejection there).  E.g. a 100 MHz spot: flat to ~44 MHz,
    -3 dB at 50 MHz, > 60 dB down by 60 MHz.
  * N <= 11 MHz: brick-wall DFT cutoff at +/-N/2 MHz (the 10 MHz
    rolloff plus its ~5.7 MHz pre-edge knee cannot fit inside such a
    narrow band without distorting the passband centre).

Out-of-band energy sits at the 6-decimal text quantisation floor
(~ -120 dB), not at -60 dB.  The in-band level is flat because the
spectrum is synthesised with a fixed, averaged amplitude (random
phases) - the PSD trace shows no per-bin statistical ripples while
the time-domain samples remain Gaussian noise.

(The legacy spot5/10/500mhz.txt files in the files repo have a soft
ramp edge - only ~ -60 dB rejection ~10 MHz outside the band - so
prefer these generated profile files.)

All MHz values above are in the file's sample-rate units (Fs = P MHz).
If the DAC plays the file at a different clock, scale the displayed
band by clock/P.

The DAC loader auto-scales each file's peak to full scale, so the
absolute amplitude written here (normalized to a peak of 1.0) only
keeps the file compact.

Usage
-----
    python3 generate.py                  # all 700 files (100+200+400)
    python3 generate.py --profiles 100   # only the 100 MHz profile set
    python3 generate.py --profiles 200 --bw 12   # a few 200 MHz files
    python3 generate.py --profiles 400 --bw 1 2 3 40 400
    python3 generate.py --out /path/to/spot
    python3 generate.py --no-numpy       # force the stdlib (slower) path

    --bw values must be <= the profile bandwidth; invalid ones are
    skipped with a warning.  Re-running is idempotent (default seed is
    deterministic per file).

Speed: with numpy installed (pip install numpy) all 700 files take
roughly 5-10 minutes.  Without numpy the pure-stdlib FFT path needs
roughly 4-5 s per file (~40-60 min for all 700).  Total on-disk size
is about 3.5 GB, so make sure the target folder has enough room.

NEVER commit the generated .txt files (files/spot/.gitignore blocks them).
"""

import argparse
import math
import os
import random
import sys
import time

DEFAULT_SAMPLES = 262144          # 2^18
PROFILES = (100, 200, 400)        # ADRV9009 profile TX bandwidths, MHz


# ---------------------------------------------------------------------------
# Pure-stdlib FFT (iterative radix-2, parallel re/im float lists)
# ---------------------------------------------------------------------------

def _fft_pure(re, im, inverse=False):
    """In-place iterative radix-2 FFT over parallel float lists.

    `re`/`im` must have length 2^k.  Applies the 1/n normalization for
    the inverse transform.
    """
    n = len(re)
    if n < 2 or (n & (n - 1)):
        raise ValueError("FFT length must be a power of two")

    # bit-reversal permutation
    j = 0
    for i in range(1, n):
        bit = n >> 1
        while j & bit:
            j ^= bit
            bit >>= 1
        j |= bit
        if i < j:
            re[i], re[j] = re[j], re[i]
            im[i], im[j] = im[j], im[i]

    angle = (6.283185307179586 / n) * (1.0 if inverse else -1.0)
    length = 2
    while length <= n:
        half = length >> 1
        ang = angle * (n / length)
        W = complex(math.cos(ang), math.sin(ang))
        Wr, Wi = W.real, W.imag
        for start in range(0, n, length):
            wr, wi = 1.0, 0.0
            stop = start + half
            for k in range(start, stop):
                ar = re[k]
                ai = im[k]
                br = re[k + half]
                bi = im[k + half]
                tr = br * wr - bi * wi
                ti = br * wi + bi * wr
                re[k] = ar + tr
                im[k] = ai + ti
                re[k + half] = ar - tr
                im[k + half] = ai - ti
                wr, wi = wr * Wr - wi * Wi, wr * Wi + wi * Wr
        length <<= 1

    if inverse:
        inv = 1.0 / n
        for i in range(n):
            re[i] *= inv
            im[i] *= inv


# ---------------------------------------------------------------------------
# Band-edge filter design
# ---------------------------------------------------------------------------
#
# Requirement: a high-order-filter look, not a soft ramp.  For a
# 100 MHz spot the spectrum must be flat up to f0+/-50 MHz (passband,
# -3 dB at the nominal edge) and at least 40-60 dB down by f0+/-60
# MHz.  Implemented as a raised-cosine rolloff: flat top, exactly
# -3 dB at +/-N/2 MHz, zero (noise floor) at +/-(N/2 + 10) MHz.  For
# N <= 11 MHz the 10 MHz rolloff plus its ~5.7 MHz pre-edge knee
# cannot fit without distorting the passband centre, so those files
# use a brick-wall DFT cutoff at +/-N/2 MHz.
#
# The PSD is synthesised: the (smoothed, multi-sequence averaged)
# amplitude spectrum is fixed and the phases are random, so the
# in-band trace is flat (no per-bin statistical ripples) while the
# time-domain samples remain Gaussian noise.

N_SEQUENCES = 4          # independent noise sequences averaged into the PSD
BIN_SMOOTH = 16          # circular moving-average width for the PSD (bins)
ROLLOFF_MHZ = 10.0       # stopband starts at N/2 + ROLLOFF_MHZ
KNEE_MHZ = 5.706         # pre-edge knee (raised cosine -> -3 dB at N/2)
BRICKWALL_MAX_BW = 11.0  # N <= this uses the brick-wall edge


def _band_edge_response(bw_mhz, profile_bw, n):
    """Per-DFT-bin amplitude response H[k] (list of n floats).

    Bin k (0..n-1) is the signed frequency
    (k if k <= n/2 else k-n) * P/n MHz, file sample rate P MHz.
    """
    p = float(profile_bw)
    e = bw_mhz / 2.0                       # nominal edge, MHz
    half = n // 2
    if bw_mhz >= profile_bw:               # full band: white
        return [1.0] * n
    if e >= KNEE_MHZ:                      # high-order rolloff template
        b = min(e + ROLLOFF_MHZ, p / 2.0)  # stopband start, MHz
        t = KNEE_MHZ * (b - e) / ROLLOFF_MHZ
        a = e - t                          # rolloff start, MHz
        span = 1.0 / (b - a)
        pi = math.pi
        H = []
        append = H.append
        for k in range(n):
            af = (k if k <= half else k - n) * (p / n)
            if af < 0.0:
                af = -af
            if af <= a:
                append(1.0)
            elif af < b:
                append(0.5 * (1.0 + math.cos(pi * (af - a) * span)))
            else:
                append(0.0)
        return H
    c = int(bw_mhz * n // (2 * profile_bw))  # brick-wall bin count
    return [1.0 if (k if k <= half else n - k) < c else 0.0
            for k in range(n)]


def _synthesize_numpy(n, seed, H):
    """Fixed-amplitude / random-phase re-synthesis (numpy path)."""
    import numpy as np
    rng = np.random.default_rng(seed)
    S = None
    for _ in range(N_SEQUENCES):
        x = rng.standard_normal(n) + 1j * rng.standard_normal(n)
        X = np.fft.fft(x)
        pw = X.real * X.real + X.imag * X.imag
        S = pw if S is None else S + pw
    S /= N_SEQUENCES
    acc = np.zeros(n)
    for r in range(BIN_SMOOTH):
        acc += np.roll(S, r)
    Ss = acc / BIN_SMOOTH
    amp = np.asarray(H, dtype=np.float64) * np.sqrt(Ss)
    amp[0] = 0.0                      # no DC component
    ph = rng.uniform(0.0, 2.0 * math.pi, n)
    y = np.fft.ifft(amp * np.cos(ph) + 1j * (amp * np.sin(ph)))
    return y.real, y.imag


def _synthesize_pure(n, seed, H):
    """Fixed-amplitude / random-phase re-synthesis (stdlib path)."""
    rnd = random.Random(seed)
    S = [0.0] * n
    for _ in range(N_SEQUENCES):
        re = [rnd.gauss(0.0, 1.0) for _ in range(n)]
        im = [rnd.gauss(0.0, 1.0) for _ in range(n)]
        _fft_pure(re, im)
        for k in range(n):
            S[k] += re[k] * re[k] + im[k] * im[k]
    w = BIN_SMOOTH
    tail = w - 1
    T = S[:tail] + S                  # circular windowing
    pref = [0.0] * (len(T) + 1)
    ssum = 0.0
    for i, v in enumerate(T):
        ssum += v
        pref[i + 1] = ssum
    Ss = [0.0] * n
    for k in range(n):
        Ss[k] = (pref[k + w] - pref[k]) / w
    y_re = [0.0] * n
    y_im = [0.0] * n
    for k in range(n):
        a = H[k] * math.sqrt(Ss[k])
        if k:
            p = rnd.uniform(0.0, 2.0 * math.pi)
            y_re[k] = a * math.cos(p)
            y_im[k] = a * math.sin(p)
    _fft_pure(y_re, y_im, inverse=True)
    return y_re, y_im


# ---------------------------------------------------------------------------
# File generation
# ---------------------------------------------------------------------------

def _write_wavefile(path, re, im):
    """Write one TEXT-format I/Q file (6 decimals per float)."""
    n = len(re)
    parts = ["TEXT\n"]
    append = parts.append
    fmt = "{:.6f} {:.6f}\n".format
    for i in range(n):
        append(fmt(re[i], im[i]))
    with open(path, "w") as fh:
        fh.write("".join(parts))


def generate_file(path, bw_mhz, profile_bw, samples, seed, use_numpy):
    """Generate one band-limited noise file.  Returns (seconds, bytes)."""
    t0 = time.time()
    n = samples
    H = _band_edge_response(bw_mhz, profile_bw, n)

    if use_numpy:
        re, im = _synthesize_numpy(n, seed, H)
        import numpy as np
        re = np.ascontiguousarray(re, dtype=np.float64)
        im = np.ascontiguousarray(im, dtype=np.float64)
        peak = max(float(np.max(np.abs(re))), float(np.max(np.abs(im))))
        if peak > 0.0:
            re /= peak
            im /= peak
        _write_wavefile(path, re.tolist(), im.tolist())
    else:
        re, im = _synthesize_pure(n, seed, H)
        peak = 0.0
        for i in range(n):
            a = abs(re[i])
            b = abs(im[i])
            if a > peak:
                peak = a
            if b > peak:
                peak = b
        if peak > 0.0:
            s = 1.0 / peak
            for i in range(n):
                re[i] *= s
                im[i] *= s
        _write_wavefile(path, re, im)

    return time.time() - t0, os.path.getsize(path)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _is_pow2(x):
    return x >= 2 and (x & (x - 1)) == 0


def main(argv=None):
    ap = argparse.ArgumentParser(
        description="Generate spot{N}mhz_{P}.txt band-limited noise files "
                    "for the iiS ADRV9009 spot exciter (see module docstring).")
    ap.add_argument("--profiles", type=int, nargs="*", default=list(PROFILES),
                    choices=PROFILES,
                    help="profile bandwidths P to generate (default: all)")
    ap.add_argument("--bw", type=int, nargs="*", default=None, metavar="N",
                    help="spot bandwidths N to generate (default: 1..P per profile)")
    ap.add_argument("--out", default=".",
                    help="output directory (default: current directory)")
    ap.add_argument("--samples", type=int, default=DEFAULT_SAMPLES,
                    help="samples per file, power of two (default: 262144)")
    ap.add_argument("--seed", default=None,
                    help="global RNG seed (int) or 'random'; default: "
                         "deterministic per file")
    ap.add_argument("--no-numpy", action="store_true",
                    help="force the pure-stdlib (slower) path")
    ap.add_argument("--quiet", action="store_true", help="less progress output")
    args = ap.parse_args(argv)

    if not _is_pow2(args.samples):
        ap.error("--samples must be a power of two (>= 2)")
    if args.seed is not None and args.seed != "random":
        try:
            args.seed = int(args.seed)
        except ValueError:
            ap.error("--seed must be an integer or 'random'")

    try:
        import numpy  # noqa: F401
        have_numpy = True
    except ImportError:
        have_numpy = False
    use_numpy = have_numpy and not args.no_numpy

    profiles = sorted(set(args.profiles))
    if not profiles:
        ap.error("no profiles selected")

    # build the (P, N) work list
    work = []
    for p in profiles:
        if args.bw:
            for n_bw in sorted(set(args.bw)):
                if n_bw < 1:
                    print(f"warning: bw {n_bw} is < 1 MHz, skipping", file=sys.stderr)
                    continue
                if n_bw > p:
                    print(f"warning: bw {n_bw} MHz > profile {p} MHz, skipping "
                          f"(max spot bandwidth for this profile is {p} MHz)",
                          file=sys.stderr)
                    continue
                work.append((p, n_bw))
        else:
            work.extend((p, n_bw) for n_bw in range(1, p + 1))

    if not work:
        print("nothing to do", file=sys.stderr)
        return 2

    os.makedirs(args.out, exist_ok=True)
    total = len(work)
    if use_numpy:
        path_desc = "numpy"
    else:
        path_desc = "pure stdlib" + ("" if not have_numpy else " (no numpy detected or --no-numpy)")
    print(f"Generating {total} file(s) into {os.path.abspath(args.out)}")
    print(f"  samples/file: {args.samples}   path: {path_desc}")
    est_total_gb = total * args.samples * 17 / 1e9
    print(f"  expected total size: ~{est_total_gb:.1f} GB")

    t_start = time.time()
    made = 0
    total_bytes = 0
    per_profile = {}  # profile bw -> [made, total]
    for idx, (p, n_bw) in enumerate(work, 1):
        name = f"spot{n_bw}mhz_{p}.txt"
        path = os.path.join(args.out, name)
        per_profile.setdefault(p, [0, 0])[1] += 1
        if args.seed == "random":
            seed = random.randrange(1 << 31)
        elif args.seed is not None:
            seed = args.seed
        else:
            seed = n_bw * 1000003 + p  # deterministic per (N, P)
        try:
            secs, nbytes = generate_file(path, n_bw, p, args.samples,
                                         seed, use_numpy)
        except Exception as exc:  # keep going, report at the end
            print(f"  [{idx:3d}/{total}] {name}  FAILED: {exc}", file=sys.stderr)
            continue
        made += 1
        total_bytes += nbytes
        per_profile[p][0] += 1
        if not args.quiet:
            elapsed = time.time() - t_start
            eta = elapsed / idx * (total - idx)
            print(f"  [{idx:3d}/{total}] {name}  {secs:6.2f}s  "
                  f"eta {eta/60.0:5.1f} min")

    elapsed = time.time() - t_start
    print(f"Done: {made}/{total} file(s), {total_bytes/1e9:.2f} GB in "
          f"{elapsed/60.0:.1f} min")
    for p in sorted(per_profile):
        m, t = per_profile[p]
        print(f"  profile {p:>3d} MHz: {m}/{t} file(s)")
    return 0 if made == total else 1


if __name__ == "__main__":
    sys.exit(main())
