#!/usr/bin/env bash
# ==============================================================================
# eLynxSDR / iiS - Automated Dependency Installer for Ubuntu / Debian
# ==============================================================================
# Installs all required compilers, Qt5 libraries, DSP math libraries (FFTW3,
# MatIO), FreeType font engines, OpenGL/GLChart packages, and IIO tools (libiio,
# iiod, iio-oscilloscope) needed to build and run this software from source.
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
echo " eLynxSDR / iiS - Installing Dependencies for Fresh Ubuntu Install"
echo "===================================================================="

# Check OS distribution
if [[ -r /etc/os-release ]]; then
    # shellcheck disable=SC1091
    source /etc/os-release
    echo "Detected OS: ${PRETTY_NAME:-$ID $VERSION_ID} ($(dpkg --print-architecture 2>/dev/null || uname -m))"
fi

echo ""
echo "[1/5] Updating APT package repositories..."
$SUDO apt-get update -y

echo ""
echo "[2/5] Installing core build tools & compilers..."
CORE_PACKAGES=(
    build-essential
    g++
    gcc
    make
    cmake
    pkg-config
    git
    curl
    ca-certificates
)
$SUDO apt-get install --no-install-recommends --yes "${CORE_PACKAGES[@]}"

echo ""
echo "[3/5] Installing Qt5 frameworks & development packages..."
QT_PACKAGES=(
    qt5-qmake
    qtbase5-dev
    qtbase5-dev-tools
    libqt5opengl5-dev
    libqt5network5
    libqt5concurrent5
    libqt5printsupport5
    libqt5gui5
    libqt5widgets5
    libqt5core5a
)

# Attempt installation of Qt5 packages (falling back gracefully for distro-specific t64 trans-packages)
for pkg in "${QT_PACKAGES[@]}"; do
    if apt-cache show "$pkg" >/dev/null 2>&1; then
        $SUDO apt-get install --no-install-recommends --yes "$pkg" || true
    fi
done
# Ensure base Qt development package is present
$SUDO apt-get install --no-install-recommends --yes qtbase5-dev qt5-qmake libqt5opengl5-dev || true

echo ""
echo "[4/5] Installing Graphics, DSP, FreeType & IIO Libraries..."
DEPENDENCY_PACKAGES=(
    # FreeType font engine & fonts
    libfreetype6-dev
    libfreetype6
    fonts-freefont-ttf
    fonts-dejavu-core

    # OpenGL & 3D GLChart rendering
    libgl1-mesa-dev
    libglu1-mesa-dev
    mesa-common-dev
    libglew-dev

    # DSP & Mathematics
    libfftw3-dev
    libfftw3-double3
    libfftw3-single3
    libmatio-dev

    # GLib
    libglib2.0-dev
    libglib2.0-0

    # Industrial I/O (IIO) & ADI components
    libiio-dev
    libiio-utils
    iiod

    # Python utilities
    python3
    python3-pip
    python3-numpy
)

for pkg in "${DEPENDENCY_PACKAGES[@]}"; do
    if apt-cache show "$pkg" >/dev/null 2>&1; then
        $SUDO apt-get install --no-install-recommends --yes "$pkg" || true
    fi
done

# Try installing iio-oscilloscope if available from repository
if apt-cache show iio-oscilloscope >/dev/null 2>&1; then
    echo "Installing iio-oscilloscope package..."
    $SUDO apt-get install --no-install-recommends --yes iio-oscilloscope || true
fi

echo ""
echo "[5/5] Verifying installed dependencies & headers..."

MISSING=0
check_header() {
    local header="$1"
    local desc="$2"
    if [[ -f "$header" ]] || find /usr/include -name "$(basename "$header")" 2>/dev/null | grep -q .; then
        printf "  [OK] %-30s (%s)\n" "$desc" "$header"
    else
        printf "  [WARN] %-30s not found at %s\n" "$desc" "$header"
        MISSING=$((MISSING + 1))
    fi
}

check_binary() {
    local bin="$1"
    local desc="$2"
    if command -v "$bin" >/dev/null 2>&1; then
        printf "  [OK] %-30s (%s)\n" "$desc" "$(command -v "$bin")"
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
