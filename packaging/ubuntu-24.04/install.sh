#!/usr/bin/env bash
# This script is copied into the offline bundle by Makefile.download.
set -euo pipefail

bundle_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$bundle_dir"

if [[ ! -r /etc/os-release ]]; then
    echo "Error: cannot identify this Linux system." >&2
    exit 1
fi
# shellcheck disable=SC1091
source /etc/os-release
if [[ "${ID:-}" != ubuntu || "${VERSION_ID:-}" != "24.04" ]]; then
    echo "Error: this offline installer is for Ubuntu 24.04 (Noble) only." >&2
    exit 1
fi
if [[ "$(dpkg --print-architecture)" != amd64 ]]; then
    echo "Error: this package is for amd64/x86_64 only." >&2
    exit 1
fi

shopt -s nullglob
app_debs=(./eLynxSDR_*.deb)
runtime_debs=(./dependencies/*.deb)
if (( ${#app_debs[@]} != 1 )); then
    echo "Error: expected one eLynxSDR_*.deb in $bundle_dir." >&2
    exit 1
fi
if (( ${#runtime_debs[@]} == 0 )); then
    echo "Error: no runtime packages found in dependencies/." >&2
    exit 1
fi

optional_iiod_debs=()
if [[ "${1:-}" == "--with-iiod" ]]; then
    optional_iiod_debs=(./optional-iiod/*.deb)
    if (( ${#optional_iiod_debs[@]} == 0 )); then
        echo "Error: --with-iiod requested, but optional-iiod/ is empty." >&2
        exit 1
    fi
    echo "Including optional iiod server packages. This PC will act as an IIO network server."
fi

printf 'Installing eLynxSDR and %d offline dependency packages...\n' "${#runtime_debs[@]}"
sudo apt-get --no-download --yes install "${runtime_debs[@]}" "${app_debs[0]}" "${optional_iiod_debs[@]}"

mcr_root="${ELYNXSDR_MCR_ROOT:-/usr/local/MATLAB/MATLAB_Runtime/v99}"
if [[ ! -e "$mcr_root/runtime/glnxa64/libmwlaunchermain.so" ]]; then
    echo "NOTE: MATLAB Runtime 9.9 was not detected at $mcr_root."
    echo "The Sattar MATLAB-generated IQ tool needs that separate runtime; the rest of the app can still be installed."
fi

echo "Installation finished. Launch eLynxSDR from the Ubuntu Applications menu."
