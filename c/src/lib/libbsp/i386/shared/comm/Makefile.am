##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = uart.h tty_drv.h i386_io.h

C_FILES = i386-stub-glue.c i386-stub.c uart.c tty_drv.c gdb_glue.c

noinst_DATA = GDB.HOWTO

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = GDB.HOWTO i386-stub-glue.c i386-stub.c uart.c uart.h tty_drv.h \
    i386_io.h tty_drv.c gdb_glue.c

include $(top_srcdir)/../../../../../automake/local.am
