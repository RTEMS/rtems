##
## $Id$
##

SUBDIRS = . console-generic clock timer cpm mmu exceptions

include_mpc8xxdir = $(includedir)/mpc8xx

include_HEADERS = include/mpc8xx.h
include_mpc8xx_HEADERS = include/console.h include/cpm.h include/mmu.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/mpc8xx \
    $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%) \
    $(include_mpc8xx_HEADERS:include/%=$(PROJECT_INCLUDE)/mpc8xx/%)

$(PROJECT_INCLUDE)/mpc8xx:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc8xx/%.h: include/%.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
