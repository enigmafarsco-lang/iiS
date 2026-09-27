# Bridge noise files (ADR-V9009 bridge noise tab)

Band-limited noise waveform files for the **Bridge Noise** tab of the
Exciter (Phase 6).  When the Bridge Noise bandwidth is set to `N` (MHz)
with ADRV9009 profile `P` active (100/200/400, see the *Profile* tab in
the receiver settings), the software loads:

```
bridge/bridge{N}mhz_{P}.txt
```

e.g. typing `40` in the Bridge Noise bandwidth box with the 200 MHz
profile selected loads `bridge40mhz_200.txt`.  If the file is missing
the app shows an error (there is no legacy bridge fallback).

## Generating the files

Run the generator in this folder (or any folder you keep the bridge
files in — the app looks in `./bridge/` relative to the startup
directory):

```bash
python3 generate_bridge.py            # all 700 files: 100 + 200 + 400
python3 generate_bridge.py --profiles 200 --bw 10 40 200
python3 generate_bridge.py --out /path/to/bridge
```

* `pip install numpy` makes generation roughly 3-5 minutes total;
  without numpy the stdlib path takes roughly 4-5 s per file.
* Total size is about **3.5 GB** — check free space first.
* Re-running is safe: output is deterministic per file.

## Engine

The bridge files use the **same generator engine as the spot files**
(`files/spot/generate.py`): identical spectral convention (flat
passband, exactly -3 dB at +/-N/2, 2nd-order rolloff to the noise
floor by +/-N/2+10 MHz, brick wall for N <= 11), identical file format
(`TEXT` header, two I/Q columns, 6 decimals, 262144 samples) and the
same board-calibrated sample rate `P^2/800 MS/s` (12.5/50/200 for the
100/200/400 profiles).  The seed stream is offset (+777001) so each
bridge file is a different noise realization than the corresponding
spot file.

## What must NOT be committed

The generated `bridge*.txt` files are blocked by this folder's
`.gitignore` — **never commit them** (they are ~3.5 GB of noise). Only
`generate_bridge.py` and this README belong in version control.
