##
## $Id$
##


include_HEADERS = uart.h

C_FILES = console.c uart.c

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all: $(PREINSTALL_FILES)

EXTRA_DIST = console.c uart.c

include $(top_srcdir)/../../../automake/local.am
