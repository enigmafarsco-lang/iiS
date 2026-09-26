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

* `pip install numpy` makes generation a few minutes total; without
  numpy the stdlib path takes roughly 1-10 s per file (~20-60 min for
  all 700).
* Total size is about **4.5 GB** — check free space first.
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

## What must NOT be committed

The generated `spot*.txt` files are blocked by this folder's
`.gitignore` — **never commit them** (they are ~4.5 GB of noise). Only
`generate.py` and this README belong in version control.
