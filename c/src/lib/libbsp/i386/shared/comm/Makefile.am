## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = uart.h

C_FILES = i386-stub-glue.c i386-stub.c uart.c

noinst_DATA = GDB.HOWTO

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all: $(PREINSTALL_FILES)

EXTRA_DIST = GDB.HOWTO i386-stub-glue.c i386-stub.c uart.c uart.h

include $(top_srcdir)/../../../../../automake/local.am
