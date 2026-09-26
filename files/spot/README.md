# Spot noise files (ADR-V9009 spot exciter)

Band-limited noise waveform files for the **Spot** tab of the Exciter.
When the Spot bandwidth is set to `N` (MHz) with ADRV9009 profile `P`
active (100/200/400, see the *Profile* tab in the receiver settings),
the software loads:

```
spot/spot{N}mhz_{P}.txt
```

e.g. typing `40` in the Exciter Spot bandwidth box with the 100 MHz
profile selected loads `spot40mhz_100.txt`. If that file is missing the
software falls back to the legacy `spot{N}mhz.txt` (e.g. `spot5mhz.txt`).

## Generating the files

Run the generator in this folder (or any folder you keep the spot files
in — the app looks in `./spot/` relative to the startup directory):

```bash
python3 generate.py            # all 700 files: 100 + 200 + 400
python3 generate.py --profiles 100        # only the 100 MHz set
python3 generate.py --profiles 200        # only the 200 MHz set
python3 generate.py --profiles 400 --bw 1 2 3 40 400
python3 generate.py --out /path/to/spot   # write elsewhere
```

A plain `python3 generate.py` always produces **all three profile
sets** — `spot1mhz_100.txt .. spot100mhz_100.txt` (100 files),
`spot1mhz_200.txt .. spot200mhz_200.txt` (200 files) and
`spot1mhz_400.txt .. spot400mhz_400.txt` (400 files) — and prints a
per-profile count at the end so a missing set cannot go unnoticed.

* `pip install numpy` makes generation roughly 3-5 minutes total;
  without numpy the stdlib path takes roughly 4-5 s per file
  (~40-60 min for all 700).
* Total size is about **3.5 GB** — check free space first.
* Re-running is safe: default output is deterministic per file.

## File format

```
TEXT
<I> <Q>
<I> <Q>
...
```

* header line `TEXT` (the waveform header the iio-oscilloscope DAC
  loader understands),
* two columns per line: in-phase / quadrature floats, 6 decimals,
* 262144 samples per file.

Each file is complex Gaussian noise confined to a flat band of
`N` MHz centred at DC.

**Assumed sample rate: `Fs = 0.5 x P MHz`** (P100: 50, P200: 100,
P400: 200 MS/s) — half the profile bandwidth.  This is the I/Q
playback rate of the board, calibrated with the scope (files
generated at the old `P` MHz assumption displayed at exactly half
width, e.g. a 100 MHz spot reading 50 MHz on the spectrum).  With
this rate **a spot of `N` MHz occupies exactly `N` MHz**: type 100,
see 100.

Consequences:

* the widest spot a profile can produce is `P/2 MHz` (Nyquist):
  **P100 -> 50 MHz, P200 -> 100 MHz, P400 -> 200 MHz**;
* `N > P/2` produces the profile's maximum (full-band) waveform
  (`spot{P}mhz_{P}.txt` and wider are all full-band white noise);
* to re-anchor the rate (different board/clock), change
  `SAMPLE_RATE_SCALE` in `generate.py` and regenerate.

(The Talise profile files list the TX baseband input rate as
`1.2288 x P` MHz — 122.88/245.76/491.52 MS/s — with 16/8/4x
interpolation to the 1966.08 MS/s DAC; the board's actual file
playback clock is the `0.5 x P` value above.)

The DAC loader auto-scales each file's peak to full scale.

### Spectral edge (high-order filter shape, not a soft ramp)

* `N >= 12 MHz`: flat passband (ripple well under 1 dB), **exactly
  -3 dB at the nominal edge** (+/-N/2 MHz), then a steep 2nd-order
  (zero-slope) raised-cosine rolloff reaching the noise floor by
  **+/(N/2 + 10) MHz** — more than 60 dB rejection there (measured:
  > 100 dB; the floor is the ~-120 dB 6-decimal text quantisation
  limit).  Example, 100 MHz spot on profile 400: flat to ~46 MHz,
  -3.7 dB at 50 MHz, -21 dB at 55 MHz, > 120 dB down by 60 MHz.
  When the edge is within 10 MHz of Nyquist the rolloff is
  compressed to end exactly at Nyquist (-3 dB still at the edge).
* `N <= 11 MHz`: brick-wall cutoff at +/-N/2 MHz (the 10 MHz rolloff
  cannot fit inside such a narrow band without distorting the
  passband centre); out-of-band energy at the same ~-120 dB floor.
* The in-band level is flat: the spectrum is synthesised with a
  fixed, averaged amplitude (random phases), so the PSD trace has no
  per-bin statistical ripples.

All MHz figures are in the file's sample-rate units
(Fs = 0.5 x P MHz, see above).  If the DAC plays the file at a
different clock, scale the displayed band by clock/Fs.

## Impulse (pulse) tab

`Impulse.txt` is generated live by the app from the PRI / pulse-width
spins.  Samples are created at the same profile playback rate
(`0.5 x P` MS/s, `profileBw` from the Profile tab): `samples =
microseconds x rate`, so a 10 us pulse at profile 200 is 1000
samples (at profile 100 it would be 500).  Older app versions wrote
1 sample/us (1 MS/s) regardless of profile — pulses came out 50-200x
too wide.

## What must NOT be committed

The generated `spot*.txt` files are blocked by this folder's
`.gitignore` — **never commit them** (they are ~3.5 GB of noise). Only
`generate.py` and this README belong in version control.
