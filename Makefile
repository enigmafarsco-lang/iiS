# Convenience top-level Makefile for this qmake project (eLynxSDR.pro), so
# a plain `make` at the repository root works. It runs qmake in a shadow
# build directory (the same one Qt Creator uses) and delegates to the
# generated Makefile there; qmake output never overwrites this file.

BUILD_DIR ?= build/Desktop-Debug
PRO       := $(CURDIR)/eLynxSDR.pro

QMAKE ?= $(shell command -v qmake6 2>/dev/null || command -v qmake 2>/dev/null || \
	ls /usr/lib/qt6/bin/qmake /usr/lib/qt5/bin/qmake \
	   /usr/lib/x86_64-linux-gnu/qt5/bin/qmake 2>/dev/null | head -n1)

.PHONY: all clean distclean

all: $(BUILD_DIR)/Makefile
	$(MAKE) -C $(BUILD_DIR)

$(BUILD_DIR)/Makefile: $(PRO)
	@if [ -z "$(QMAKE)" ]; then \
		echo "error: qmake not found - install Qt (qt5-qmake / qt6-base-dev)" >&2; \
		echo "       or build the project with Qt Creator" >&2; \
		exit 1; \
	fi
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(QMAKE) $(PRO)

clean:
	@if [ -f $(BUILD_DIR)/Makefile ]; then $(MAKE) -C $(BUILD_DIR) clean; fi

distclean:
	rm -rf $(BUILD_DIR)
