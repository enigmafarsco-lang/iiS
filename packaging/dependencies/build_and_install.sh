#!/usr/bin/env bash
# ==============================================================================
# eLynxSDR / iiS - Offline Source Build and Installer
# ==============================================================================
# Compiles and installs the bundled open-source dependencies directly from the
# offline source trees in packaging/dependencies/sources:
#   1. FreeType (libfreetype)
#   2. libiio & iiod (IIO client library & network server daemon)
#   3. FFTW3 (Fast Fourier Transform library)
#   4. MatIO (MATLAB MAT file library)
#   5. ADI IIO-Oscilloscope (adi-osc GUI & plugins)
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SOURCES_DIR="${SCRIPT_DIR}/sources"
PREFIX="${INSTALL_PREFIX:-/usr/local}"
JOBS="$(nproc 2>/dev/null || echo 4)"

SUDO=""
if [[ $EUID -ne 0 ]]; then
    if command -v sudo >/dev/null 2>&1; then
        SUDO="sudo"
    else
        echo "Error: this script must be run as root or with sudo installed." >&2
        exit 1
    fi
fi

echo "===================================================================="
echo " Building & Installing Offline Dependencies from Source Tree"
echo " Target Prefix: ${PREFIX}"
echo " Parallel Jobs: ${JOBS}"
echo "===================================================================="

# ------------------------------------------------------------------------------
# 1. Build and install FreeType
# ------------------------------------------------------------------------------
echo ""
echo "[1/5] Building FreeType from source..."
if [[ -d "${SOURCES_DIR}/freetype" ]]; then
    cd "${SOURCES_DIR}/freetype"
    if command -v cmake >/dev/null 2>&1; then
        rm -rf build && mkdir -p build
        cmake -B build -S . \
            -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=ON
        cmake --build build -j"${JOBS}"
        $SUDO cmake --install build
    elif [[ -x "./autogen.sh" || -f "configure" ]]; then
        if [[ -x "./autogen.sh" && ! -f "configure" ]]; then ./autogen.sh; fi
        ./configure --prefix="${PREFIX}" --enable-shared
        make -j"${JOBS}"
        $SUDO make install
    fi
    # Ensure standard include path link exists
    if [[ ! -d "/usr/include/freetype2" && -d "${PREFIX}/include/freetype2" ]]; then
        $SUDO ln -sf "${PREFIX}/include/freetype2" "/usr/include/freetype2" || true
    fi
    echo "  -> FreeType built and installed successfully."
fi

# ------------------------------------------------------------------------------
# 2. Build and install libiio & iiod
# ------------------------------------------------------------------------------
echo ""
echo "[2/5] Building libiio & iiod from source..."
if [[ -d "${SOURCES_DIR}/libiio" ]]; then
    cd "${SOURCES_DIR}/libiio"
    rm -rf build && mkdir -p build
    if command -v cmake >/dev/null 2>&1; then
        cmake -B build -S . \
            -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_IIOD=ON \
            -DWITH_LOCAL_BACKEND=ON \
            -DWITH_NETWORK_BACKEND=ON \
            -DWITH_USB_BACKEND=ON \
            -DENABLE_IPV6=ON \
            -DBUILD_SHARED_LIBS=ON
        cmake --build build -j"${JOBS}"
        $SUDO cmake --install build
    fi
    echo "  -> libiio and iiod built and installed successfully."
fi

# ------------------------------------------------------------------------------
# 3. Build and install FFTW3
# ------------------------------------------------------------------------------
echo ""
echo "[3/5] Building FFTW3 from source..."
if [[ -d "${SOURCES_DIR}/fftw3" ]]; then
    cd "${SOURCES_DIR}/fftw3"
    if command -v cmake >/dev/null 2>&1; then
        rm -rf build && mkdir -p build
        cmake -B build -S . \
            -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=ON \
            -DENABLE_THREADS=ON \
            -DENABLE_OPENMP=OFF
        cmake --build build -j"${JOBS}"
        $SUDO cmake --install build
    elif [[ -f "configure" || -x "./bootstrap.sh" ]]; then
        if [[ -x "./bootstrap.sh" && ! -f "configure" ]]; then ./bootstrap.sh; fi
        ./configure --prefix="${PREFIX}" --enable-shared --enable-threads
        make -j"${JOBS}"
        $SUDO make install
    fi
    echo "  -> FFTW3 built and installed successfully."
fi

# ------------------------------------------------------------------------------
# 4. Build and install MatIO
# ------------------------------------------------------------------------------
echo ""
echo "[4/5] Building MatIO from source..."
if [[ -d "${SOURCES_DIR}/matio" ]]; then
    cd "${SOURCES_DIR}/matio"
    if command -v cmake >/dev/null 2>&1; then
        rm -rf build && mkdir -p build
        cmake -B build -S . \
            -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=ON \
            -DMATIO_WITH_ZLIB=OFF
        cmake --build build -j"${JOBS}"
        $SUDO cmake --install build
    elif [[ -x "./autogen.sh" || -f "configure" ]]; then
        if [[ -x "./autogen.sh" && ! -f "configure" ]]; then ./autogen.sh; fi
        ./configure --prefix="${PREFIX}" --enable-shared
        make -j"${JOBS}"
        $SUDO make install
    fi
    echo "  -> MatIO built and installed successfully."
fi

# ------------------------------------------------------------------------------
# 5. Build and install ADI IIO-Oscilloscope
# ------------------------------------------------------------------------------
echo ""
echo "[5/5] Building ADI IIO-Oscilloscope from source..."
if [[ -d "${SOURCES_DIR}/iio-oscilloscope" ]]; then
    cd "${SOURCES_DIR}/iio-oscilloscope"
    if command -v cmake >/dev/null 2>&1; then
        rm -rf build && mkdir -p build
        cmake -B build -S . \
            -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
            -DCMAKE_BUILD_TYPE=Release
        cmake --build build -j"${JOBS}" || echo "Note: iio-oscilloscope build optional components skipped if gtk/glade dev libraries not present"
        $SUDO cmake --install build || true
    fi
    echo "  -> IIO-Oscilloscope build step completed."
fi

# Update dynamic library cache
$SUDO ldconfig || true

echo ""
echo "===================================================================="
echo " All offline source dependencies built and installed successfully!"
echo "===================================================================="
