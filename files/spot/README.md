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
`N` MHz centred at DC, assumed sample rate `P` MHz (so `N <= P` always
fits the Nyquist band; `spot{P}mhz_{P}.txt` is full-band white noise).
The DAC loader auto-scales each file's peak to full scale.

### Spectral edge (high-order filter shape, not a soft ramp)

* `N >= 12 MHz`: flat passband (ripple well under 1 dB), **exactly
  -3 dB at the nominal edge** (+/-N/2 MHz), then a smooth rolloff
  reaching the noise floor by **+/(N/2 + 10) MHz** — more than 60 dB
  rejection there (measured: > 100 dB; the floor is the ~-120 dB
  6-decimal text quantisation limit).  Example, 100 MHz spot:
  flat to ~44 MHz, -3 dB at 50 MHz, -12.7 dB at 55 MHz, > 100 dB
  down by 60 MHz.
* `N <= 11 MHz`: brick-wall cutoff at +/-N/2 MHz (the 10 MHz rolloff
  cannot fit inside such a narrow band without distorting the
  passband centre); out-of-band energy at the same ~-120 dB floor.
* The in-band level is flat: the spectrum is synthesised with a
  fixed, averaged amplitude (random phases), so the PSD trace has no
  per-bin statistical ripples.

All MHz figures are in the file's sample-rate units (Fs = P MHz, the
profile bandwidth).  If the DAC plays the file at a different clock,
scale the displayed band by clock/P.

## What must NOT be committed

The generated `spot*.txt` files are blocked by this folder's
`.gitignore` — **never commit them** (they are ~3.5 GB of noise). Only
`generate.py` and this README belong in version control.
