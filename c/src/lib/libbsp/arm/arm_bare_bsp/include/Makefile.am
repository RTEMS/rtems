##
## $Id$
##


include_HEADERS = bsp.h registers.h coverhd.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	cp $< $@
CLEANFILES = coverhd.h

TMPINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all: $(TMPINSTALL_FILES)

EXTRA_DIST = bspio.h bsp.h uart.h registers.h

include $(top_srcdir)/../../../../automake/local.am
