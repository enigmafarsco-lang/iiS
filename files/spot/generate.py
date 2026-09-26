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
Each file is complex (I/Q) Gaussian noise whose power is confined to a
flat band of N MHz centred at DC (baseband).  The band edge is a
brick-wall DFT cutoff: every DFT bin inside +/-N/2 MHz is kept and every
bin outside is zeroed, so the transition is at most one DFT bin wide
(P/262144 MHz = 0.38 / 0.76 / 1.53 kHz for P = 100/200/400).  There is
no filter ramp; out-of-band energy sits at the 6-decimal text
quantisation floor (~ -120 dB), not at -60 dB.  (The legacy
spot5/10/500mhz.txt files in the files repo have a soft ramp edge, so
prefer these generated profile files.)

    -P/2 MHz ... -N/2 MHz ... 0 ... N/2 MHz ... +P/2 MHz
                |_____________|
                 band = N MHz

The file's assumed sample rate is Fs = P MHz (P = the ADRV9009 profile
the file belongs to), so every N <= P fits inside the Nyquist band
[-P/2, +P/2] MHz.  The largest file of a set (spot{P}mhz_{P}.txt) is
therefore full-band white noise for that profile.

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

Speed: with numpy installed (pip install numpy) all 700 files take a
few minutes.  Without numpy the pure-stdlib FFT path needs roughly
1-10 s per file.  Total on-disk size is about 4.5 GB, so make sure the
target folder has enough room.

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


def _bandlimit_pure(re, im, bins_each_side):
    """Brick-wall band limit around DC via FFT, zeroing out-of-band bins."""
    n = len(re)
    x_re = re[:]
    x_im = im[:]
    _fft_pure(x_re, x_im)
    c = bins_each_side
    if c:
        lo = c
        hi = n - c
        if hi > lo:
            for k in range(lo, hi):
                x_re[k] = 0.0
                x_im[k] = 0.0
    _fft_pure(x_re, x_im, inverse=True)
    re[:] = x_re
    im[:] = x_im


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
    # File sample rate = profile bandwidth (MHz).  Keep the baseband band
    # [-bw/2, +bw/2] MHz -> bw*n/(2*P) DFT bins on each side of DC.
    bins_each_side = int(bw_mhz * n // (2 * profile_bw))

    if use_numpy:
        import numpy as np
        rng = np.random.default_rng(seed)
        x = rng.standard_normal(n) + 1j * rng.standard_normal(n)
        spectrum = np.fft.fft(x)
        mask = np.zeros(n, dtype=bool)
        c = bins_each_side
        if c:
            mask[:c] = True
            mask[n - c:] = True
        x = np.fft.ifft(spectrum * mask)
        re = np.ascontiguousarray(x.real, dtype=np.float64)
        im = np.ascontiguousarray(x.imag, dtype=np.float64)
        peak = max(float(np.max(np.abs(re))), float(np.max(np.abs(im))))
        if peak > 0.0:
            re /= peak
            im /= peak
        n_bytes = _write_wavefile(path, re.tolist(), im.tolist())
    else:
        rnd = random.Random(seed)
        re = [rnd.gauss(0.0, 1.0) for _ in range(n)]
        im = [rnd.gauss(0.0, 1.0) for _ in range(n)]
        _bandlimit_pure(re, im, bins_each_side)
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
        n_bytes = _write_wavefile(path, re, im)

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
