##
## $Id$
##

C_FILES = console.c inch.c polled_io.c uart.c reboot.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = consoleIo.h keyboard.h uart.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = console.c inch.c polled_io.c uart.c reboot.c console.inl

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/consoleIo.h: consoleIo.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/consoleIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/keyboard.h: keyboard.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/keyboard.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/keyboard.h

$(PROJECT_INCLUDE)/bsp/uart.h: uart.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
