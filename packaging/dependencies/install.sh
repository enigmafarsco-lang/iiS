#!/usr/bin/env bash
# ==============================================================================
# eLynxSDR / iiS - Complete Dependency & Source Installer
# ==============================================================================
# Installs all required compilers, Qt5 libraries, DSP math libraries (FFTW3,
# MatIO), FreeType font engines, OpenGL/GLChart packages, and IIO tools (libiio,
# iiod, iio-oscilloscope) needed to build and run this software from source.
#
# Supports both:
#   1. Automatic APT package installation (when APT/network is available)
#   2. Offline compilation & installation from bundled source trees in sources/
#
# Usage:
#   sudo ./install.sh
#   or from repository root: make dependencies
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

# Identify privileges
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
echo " eLynxSDR / iiS - Installing Dependencies & Offline Sources"
echo "===================================================================="

# Check OS distribution
if [[ -r /etc/os-release ]]; then
    # shellcheck disable=SC1091
    source /etc/os-release
    echo "Detected OS: ${PRETTY_NAME:-$ID $VERSION_ID} ($(dpkg --print-architecture 2>/dev/null || uname -m))"
fi

# Step 1: Install system packages via APT if available
if command -v apt-get >/dev/null 2>&1; then
    echo ""
    echo "[1/4] Installing system build tools & Qt5 frameworks..."
    $SUDO apt-get update -y || true

    SYSTEM_PACKAGES=(
        build-essential
        g++
        gcc
        make
        cmake
        pkg-config
        git
        curl
        ca-certificates
        qt5-qmake
        qtbase5-dev
        qtbase5-dev-tools
        libqt5opengl5-dev
        libqt5network5
        libqt5concurrent5
        libqt5printsupport5
        libqt5gui5
        libqt5widgets5
        libgl1-mesa-dev
        libglu1-mesa-dev
        mesa-common-dev
        libglew-dev
        libglib2.0-dev
        fonts-freefont-ttf
        fonts-dejavu-core
        python3
        python3-pip
        python3-numpy
    )

    for pkg in "${SYSTEM_PACKAGES[@]}"; do
        if apt-cache show "$pkg" >/dev/null 2>&1; then
            $SUDO apt-get install --no-install-recommends --yes "$pkg" || true
        fi
    done
fi

# Step 2: Build & Install all bundled offline sources
echo ""
echo "[2/4] Building & installing offline source dependencies (FreeType, libiio, iiod, FFTW3, MatIO, IIO-Oscilloscope)..."
if [[ -x "${SCRIPT_DIR}/build_and_install.sh" ]]; then
    "${SCRIPT_DIR}/build_and_install.sh"
fi

# Step 3: Ensure header search paths and ldconfig
echo ""
echo "[3/4] Configuring runtime linker and library paths..."
$SUDO ldconfig || true

# Symlink FreeType include directory if needed
if [[ -d "/usr/local/include/freetype2" && ! -d "/usr/include/freetype2" ]]; then
    $SUDO ln -sf "/usr/local/include/freetype2" "/usr/include/freetype2" || true
fi

# Step 4: Verification
echo ""
echo "[4/4] Verifying installed dependencies & headers..."

MISSING=0
check_header() {
    local header="$1"
    local desc="$2"
    if [[ -f "$header" ]] || find /usr/include /usr/local/include -name "$(basename "$header")" 2>/dev/null | grep -q .; then
        printf "  [OK] %-30s (%s)\n" "$desc" "$header"
    else
        printf "  [WARN] %-30s not found at %s\n" "$desc" "$header"
        MISSING=$((MISSING + 1))
    fi
}

check_binary() {
    local bin="$1"
    local desc="$2"
    if command -v "$bin" >/dev/null 2>&1 || [[ -x "/usr/local/bin/$bin" || -x "/usr/bin/$bin" ]]; then
        printf "  [OK] %-30s (%s)\n" "$desc" "$(command -v "$bin" 2>/dev/null || echo "/usr/local/bin/$bin")"
    else
        printf "  [WARN] %-30s not found in PATH\n" "$desc"
    fi
}

check_header "/usr/include/freetype2/ft2build.h" "FreeType2 Headers"
check_header "/usr/include/fftw3.h" "FFTW3 Headers"
check_header "/usr/include/matio.h" "MatIO Headers"
check_header "/usr/include/iio.h" "libiio Headers"
check_header "/usr/include/GL/gl.h" "OpenGL Headers"
check_binary "qmake" "Qt qmake"
check_binary "iiod" "IIO Server (iiod)"
check_binary "iio_info" "IIO Utilities (libiio-utils)"
check_binary "g++" "C++ Compiler (g++)"
check_binary "make" "Make Build Tool"
check_binary "python3" "Python 3 Runtime"

echo ""
echo "===================================================================="
echo " Dependencies installation complete!"
echo " You can now build eLynxSDR by running: make"
echo "===================================================================="
