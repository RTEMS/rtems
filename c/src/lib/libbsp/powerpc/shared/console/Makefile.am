##
## $Id$
##


C_FILES = console.c inch.c polled_io.c uart.c reboot.c

H_FILES = consoleIo.h keyboard.h uart.h

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/bsp/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = console.c inch.c polled_io.c uart.c \
    reboot.c console.inl

include $(top_srcdir)/../../../automake/local.am
