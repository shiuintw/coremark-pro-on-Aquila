# ======================================================================
#  Makefile for CoreMarkPro
# ======================================================================
#  CY-HSU 2026/05/31
# ======================================================================
SUB_MAKEFILES := $(wildcard Makefile-*)

.PHONY: all clean $(SUB_MAKEFILES)

all: $(SUB_MAKEFILES)

$(SUB_MAKEFILES):
	$(MAKE) -f $@ all

clean:
	rm -rf build/