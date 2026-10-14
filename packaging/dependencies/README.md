# eLynxSDR / iiS - Offline Dependencies & Source Trees

This directory contains the complete source trees and automated installer scripts for all external libraries and tools required by **eLynxSDR**. When you clone or download this Git repository, all necessary dependency sources are included locally so that you can compile and install them **offline** on a fresh Ubuntu or Debian machine without needing an internet connection.

---

## One-Command Installation

From the root of the repository, run:

```bash
make dependencies
```

Or run the installer directly inside this directory:

```bash
cd packaging/dependencies
sudo ./install.sh
```

---

## Bundled Source Trees (`packaging/dependencies/sources/`)

The following source packages are included directly in the repository under `sources/`:

| Component | Source Path | Description |
|---|---|---|
| **FreeType** | `sources/freetype/` | FreeType font rendering engine (`libfreetype6` / headers in `/usr/include/freetype2`) |
| **libiio & iiod** | `sources/libiio/` | Analog Devices Industrial I/O client library, utilities, and `iiod` network server daemon |
| **IIO-Oscilloscope** | `sources/iio-oscilloscope/` | Analog Devices IIO Oscilloscope (`adi-osc` application, plugins, and XML definitions) |
| **FFTW3** | `sources/fftw3/` | High-performance Fast Fourier Transform (FFT) library (`libfftw3`) |
| **MatIO** | `sources/matio/` | MATLAB MAT file I/O library for waveform and dataset parsing |

---

## Building Dependencies Offline

If you are on an offline system without access to APT packages, you can build and install all bundled dependencies directly from source:

```bash
cd packaging/dependencies
sudo ./build_and_install.sh
```

This compiles each library and installs its headers, shared libraries, and binaries into `/usr/local` (or your configured `INSTALL_PREFIX`).

---

## Verifying Dependencies

Check if all compilers, Qt tools, headers, and libraries are available on the system:

```bash
make -C packaging/dependencies check
```

---

## Building the Main Application

Once dependencies are installed, you can build eLynxSDR from the repository root:

```bash
make
```

Or open `eLynxSDR.pro` in **Qt Creator** and click **Build**.
