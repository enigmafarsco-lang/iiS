# ==============================================================================
# eLynxSDR / iiS - Dependencies Makefile
# ==============================================================================

SHELL := /bin/bash
.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
.DEFAULT_GOAL := install

SCRIPT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

.PHONY: all install build-sources check help

all: install

install:
	@chmod +x "$(SCRIPT_DIR)/install.sh" "$(SCRIPT_DIR)/build_and_install.sh"
	@"$(SCRIPT_DIR)/install.sh"

build-sources:
	@chmod +x "$(SCRIPT_DIR)/build_and_install.sh"
	@"$(SCRIPT_DIR)/build_and_install.sh"

check:
	@echo "Checking system build and library dependencies..."
	@which g++ >/dev/null 2>&1 && echo "  [OK] g++ ($$(g++ --version | head -n1))" || echo "  [MISSING] g++"
	@which make >/dev/null 2>&1 && echo "  [OK] make ($$(make --version | head -n1))" || echo "  [MISSING] make"
	@which qmake >/dev/null 2>&1 && echo "  [OK] qmake ($$(qmake -v 2>&1 | tail -n1))" || echo "  [MISSING] qmake"
	@which cmake >/dev/null 2>&1 && echo "  [OK] cmake ($$(cmake --version | head -n1))" || echo "  [MISSING] cmake"
	@which iiod >/dev/null 2>&1 && echo "  [OK] iiod" || echo "  [MISSING] iiod"
	@which iio_info >/dev/null 2>&1 && echo "  [OK] iio_info" || echo "  [MISSING] iio_info"
	@which pkg-config >/dev/null 2>&1 || { echo "  [MISSING] pkg-config"; exit 0; }
	@pkg-config --exists freetype2 >/dev/null 2>&1 && echo "  [OK] freetype2" || echo "  [MISSING] freetype2"
	@pkg-config --exists libiio >/dev/null 2>&1 && echo "  [OK] libiio" || echo "  [MISSING] libiio"
	@pkg-config --exists fftw3 >/dev/null 2>&1 && echo "  [OK] fftw3" || echo "  [MISSING] fftw3"
	@pkg-config --exists matio >/dev/null 2>&1 && echo "  [OK] matio" || echo "  [MISSING] matio"
	@pkg-config --exists glib-2.0 >/dev/null 2>&1 && echo "  [OK] glib-2.0" || echo "  [MISSING] glib-2.0"

help:
	@echo "eLynxSDR Dependency Management"
	@echo "Usage:"
	@echo "  make install         Install all packages and compile offline sources"
	@echo "  make build-sources   Build and install bundled source packages from sources/"
	@echo "  make check           Check status of compilers, Qt tools and linked libraries"
