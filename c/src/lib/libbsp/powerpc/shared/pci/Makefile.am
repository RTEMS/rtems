##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = pci.c

H_FILES = pci.h

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/bsp/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = pci.c pci.h

include $(top_srcdir)/../../../../../automake/local.am
