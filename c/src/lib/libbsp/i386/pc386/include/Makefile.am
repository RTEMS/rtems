##
## $Id$
##


# This driver needs to be reworked for the BSD stack.
# We only install wd80x3.h if HAS_NETWORKING was defined
if HAS_NETWORKING
WD80X3 = wd80x3.h
endif

include_HEADERS = bsp.h crt.h coverhd.h $(WD80X3) bspopts.h

coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	cp $< $@
CLEANFILES = coverhd.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = wd80x3.h

include $(top_srcdir)/../../../../automake/local.am
