##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = openpic.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = openpic.h

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(include_bsp_HEADERS:%=$(PROJECT_INCLUDE)/bsp/%)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = openpic.c

include $(top_srcdir)/../../../../../automake/local.am
