##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_mpc8xxdir = $(includedir)/mpc8xx

include_HEADERS = mpc8xx.h
include_mpc8xx_HEADERS = console.h cpm.h mmu.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%) \
    $(include_mpc8xx_HEADERS:%=$(PROJECT_INCLUDE)/mpc8xx/%)

$(PROJECT_INCLUDE)/mpc8xx:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc8xx/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
