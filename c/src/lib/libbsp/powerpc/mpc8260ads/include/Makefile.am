##
## $Id$
##


include_HEADERS = bsp.h bspopts.h coverhd.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp: 
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp \
	$(include_HEADERS:%=$(PROJECT_INCLUDE)/%) \
	$(include_bsp_HEADERS:%=$(PROJECT_INCLUDE)/bsp/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/force-preinstall.am
include $(top_srcdir)/../../../../automake/local.am
