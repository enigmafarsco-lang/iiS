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

**Assumed sample rate: `Fs = 0.6144 x P MHz`** — the board consumes
the DAC buffer at half the profile's sample rate (the Talise TX
input / ORx output rate of Tx_BW200_IR245p76 etc. is 1.2288 x P):

| profile | file rate | widest spot |
|---|---|---|
| 100 | 61.44 MS/s | 61.44 MHz |
| 200 | 122.88 MS/s | 122.88 MHz |
| 400 | 245.76 MS/s | 245.76 MHz |

The earlier `P^2/800` and `0.5 x P` assumptions made the on-air band
wrong and different per profile.  With the rate above **a spot of `N` MHz
occupies exactly `N` MHz**: type 100, see 100.

Consequences:

* the widest spot a profile can produce is its playback rate
  (Nyquist): **12.5 / 50 / 200 MHz** for the 100/200/400 profiles;
* `N` above that produces the profile's maximum (full-band)
  waveform;
* to re-anchor a rate (different board/clock), edit
  `profile_rate_mhz()` in `generate.py` and regenerate.

(The Talise profile files run the TX datapath at `1.2288 x P` MHz —
122.88/245.76/491.52 MS/s — with 16/8/4x interpolation to the
1966.08 MS/s DAC; the DAC buffer is consumed at half of that.)

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
(Fs = 0.6144 x P MHz, see above).  If the DAC plays the file at a
different clock, scale the displayed band by clock/Fs.

## Impulse (pulse) tab

`Impulse.txt` is generated live by the app from the PRI / pulse-width
spins.  Samples are created at the same profile playback rate
(`P^2/800` MS/s = 12.5/50/200, `profileBw` from the Profile tab):
`samples = microseconds x rate`, so a 10 us pulse at profile 200 is
500 samples (profile 100: 125, profile 400: 2000).  Older app
versions wrote `us x 487.8` samples (1 MS/s base plus a bogus
x2000/4.1 factor) regardless of profile — pulses came out 5-1000x
too wide (2.4x on the 400 profile up to ~39x on the 100 profile).

## Sweep tab

`Sweep.txt` is generated live by the app like the LFM tab: a stepped-sine
I/Q built in-app at the profile playback rate (`0.6144 x P` MS/s) with
**baseband** start / stop / step frequencies (the LO does not move).  The
tones sit at `start, start+step, ...` toward `stop` - start 0, stop 10,
step 2 gives exactly the tones 0/2/4/6/8/10 MHz - and they share the
looping 262144-sample file equally (the per-tone dwell is not critical),
with continuous phase across tone changes.

## Multi Target tab

* LFM/NLFM rows take just **start frequency / BW / T** (the exact
  LFM/NLFM tab formula, `phase = 2*pi*(f0*t + B*t^2/2T)`) and have **no
  frequency shift** - the chirp placement comes from the start frequency.
* **Bridge** rows load `bridge/bridge{N}mhz_{P}.txt` (run
  `files/bridge/generate_bridge.py` first) - band-limited noise like a
  Spot row but an independent realization.
* Spot / Bridge rows default to a 10 MHz bandwidth.

## What must NOT be committed

The generated `spot*.txt` files are blocked by this folder's
`.gitignore` — **never commit them** (they are ~3.5 GB of noise). Only
`generate.py` and this README belong in version control.
