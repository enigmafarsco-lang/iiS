# eLynxSDR / iiS - Automated Dependency Installer

This directory provides automated installation scripts for setting up all build-time and runtime dependencies required by **eLynxSDR** on a clean installation of **Ubuntu** (20.04, 22.04, 24.04) or Debian-based systems.

---

## Quick Start (One Command)

From the root of the repository, run:

```bash
make dependencies
```

Or run the installer directly:

```bash
cd packaging/dependencies
sudo ./install.sh
```

---

## Installed Packages and Components

The installer automatically installs all required packages from Ubuntu repositories:

### 1. Build Tools & Compilers
- `build-essential` (gcc, g++, make, libc-dev)
- `cmake`, `pkg-config`, `git`, `curl`

### 2. Qt5 Framework & Extensions
- `qt5-qmake`, `qtbase5-dev`, `qtbase5-dev-tools`
- `libqt5opengl5-dev` (OpenGL integration)
- `libqt5network5`, `libqt5concurrent5`, `libqt5printsupport5`, `libqt5widgets5`

### 3. Font Engine & FreeType
- `libfreetype6-dev`, `libfreetype6` (FreeType2 header & library `/usr/include/freetype2`)
- `fonts-freefont-ttf`, `fonts-dejavu-core`

### 4. 3D Graphics & OpenGL (GLChart)
- `libgl1-mesa-dev`, `libglu1-mesa-dev`, `mesa-common-dev`
- `libglew-dev`

### 5. DSP & Signal Processing Mathematics
- `libfftw3-dev`, `libfftw3-double3`, `libfftw3-single3` (Fast Fourier Transform library)
- `libmatio-dev` (MATLAB MAT file I/O library)

### 6. Industrial I/O (IIO) & ADI Drivers
- `libiio-dev`, `libiio-utils` (Analog Devices IIO client library & utilities)
- `iiod` (IIO network daemon for local/remote SDR connection)
- `iio-oscilloscope` (ADI IIO Oscilloscope)

### 7. Core Libraries & Python
- `libglib2.0-dev`, `libglib2.0-0`
- `python3`, `python3-pip`, `python3-numpy`

---

## Verifying Dependencies

You can check whether all required compilers, headers, and libraries are present on the system by running:

```bash
make -C packaging/dependencies check
```

---

## Building the Software

Once dependencies are installed, you can build eLynxSDR from the repository root:

```bash
make
```

Or open `eLynxSDR.pro` directly in **Qt Creator** and build.
