##
## $Id$
##


include_mpc8260dir = $(includedir)/mpc8260

include_HEADERS = mpc8260.h
include_mpc8260_HEADERS = console.h mmu.h cpm.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/mpc8260 \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%) \
    $(include_mpc8260_HEADERS:%=$(PROJECT_INCLUDE)/mpc8260/%)

$(PROJECT_INCLUDE)/mpc8260:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc8260/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
