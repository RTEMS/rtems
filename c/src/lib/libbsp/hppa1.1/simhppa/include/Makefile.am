##
## $Id$
##


include_rtemsdir = $(includedir)/rtems
include_HEADERS = bsp.h coverhd.h bspopts.h
include_rtems_HEADERS = ttydrv.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@
CLEANFILES = coverhd.h

$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=%(PROJECT_INCLUDE)/%) \
    $(PROJECT_INCLUDE)/rtems \
    $(include_HEADERS:%=%(PROJECT_INCLUDE)/rtems/%) 

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
