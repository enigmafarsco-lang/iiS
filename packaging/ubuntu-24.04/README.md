# Ubuntu 24.04 offline distribution (amd64)

The release is an offline bundle containing an application `.deb`, the complete APT runtime dependency closure, an installer and an optional `iiod` package set. The bundle contains the compiled application and runtime assets, **not the C++ source tree**.

## Licensing prerequisite — resolve before distributing

This application links to FFTW3 (`-lfftw3`). FFTW's published default license is GPL-2.0-or-later; its authors say non-free use requires a separate license from MIT. Therefore, do **not** distribute this as a closed-source application until the project owner has obtained the appropriate FFTW license or replaced FFTW with a suitably licensed implementation. Otherwise the source/redistribution obligations of the applicable FFTW license must be followed. See the [FFTW license terms](https://www.fftw.org/doc/License-and-Copyright.html). This packaging workflow does not grant or establish any third-party licensing rights.

The build also dynamically links Qt and libiio. Review and satisfy their LGPL notice, source-access and relinking/replacement obligations; see [Qt's official LGPL guidance](https://www.qt.io/faq/qt-open-source-licensing) and [libiio's license information](https://github.com/analogdevicesinc/libiio). This note is not legal advice.

## Build and prepare the offline bundle

Run these steps on an Internet-connected **Ubuntu 24.04 amd64** machine. The package builder rejects other Ubuntu releases and architectures.

1. Install the build-only packages (one time):

   ```bash
   cd packaging/ubuntu-24.04
   make -f Makefile.download install-build-deps
   ```

2. Build the release `.deb` and collect its full runtime dependencies from Ubuntu's repositories:

   ```bash
   make -f Makefile.download bundle
   ```

   The generated files are placed in `packaging/ubuntu-24.04/offline-bundle/`:

   ```text
   eLynxSDR_0.0.2_amd64.deb
   dependencies/*.deb
   install.sh
   Makefile.install
   README.md
   ```

   APT is run with an empty package-status view so it downloads the app's dependency closure even when the build machine already has Qt or the libraries installed. The generated `.deb` packages come from Ubuntu 24.04 repositories. The app's direct runtime dependencies include Qt 5, libiio, FFTW3, MATIO, FreeType, GLib and Python 3; APT downloads their required dependencies too. The large, generated spot/bridge waveform text files are not included; the shipped generator scripts can create the needed files on the user's machine.

3. Copy the entire `offline-bundle/` directory to a USB drive. The bundle is local output and is intentionally ignored by Git; do not commit the `.deb` files.

### Optional `iiod` bundle

The application connects to IIO hardware over an IP address, so the normal setup needs `libiio0` on the Ubuntu PC. `iiod` is the network server, and normally runs on the radio/embedded device. Only if this Ubuntu PC itself must serve a local IIO device, additionally run:

```bash
make -f Makefile.download download-iiod
```

This places its packages under `offline-bundle/optional-iiod/`. On that PC, install them separately with:

```bash
cd offline-bundle
make -f Makefile.install install-iiod
```

If that machine has no `make` installed, `./install.sh --with-iiod` installs the app and optional daemon together. Do not use that option for an ordinary client PC.

## Install on the offline Ubuntu PC

The recipient machine must be Ubuntu 24.04 amd64. Copy the complete bundle to it, open a terminal in `offline-bundle`, then run either:

```bash
./install.sh
```

or:

```bash
make -f Makefile.install
```

The installer uses `apt-get --no-download`, so it will not try to access the Internet. It installs the local app and dependency `.deb`s in one operation. The user does not compile the project or receive the source tree.

## Important separate MATLAB Runtime requirement

The bundled `files/signal/Executable_Generator_DoubleChannel` is a MATLAB-compiled executable and links to `libmwlaunchermain.so`. The Sattar IQ-generation feature therefore also needs **MATLAB Runtime 9.9 (R2020b)**; it is not an Ubuntu APT package and is not downloaded by the Makefile. Get the matching Linux x86_64 runtime installer from MathWorks on an Internet-connected machine, transfer it separately, and install it offline. By default the app looks for it at `/usr/local/MATLAB/MATLAB_Runtime/v99`; if installed elsewhere, set `ELYNXSDR_MCR_ROOT` to its root directory. You may place the archive in `packaging/ubuntu-24.04/vendor/` and add it to the bundle with `make -f Makefile.download include-mcr MCR_ARCHIVE=/path/to/runtime.zip`. The archive is not committed to Git. Without the runtime, the rest of eLynxSDR can install, but this MATLAB-generated IQ feature will not run.

The packaged build does not embed the builder's checkout path. Shipped read-only assets are installed with the app, and files the user changes or generates are copied to the user's writable application-data directory.
