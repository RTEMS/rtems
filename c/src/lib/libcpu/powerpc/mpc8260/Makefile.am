##
## $Id$
##

SUBDIRS = . console-generic clock timer exceptions mmu cpm

include_mpc8260dir = $(includedir)/mpc8260

include_HEADERS = include/mpc8260.h
include_mpc8260_HEADERS = include/console.h include/mmu.h include/cpm.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/mpc8260 \
    $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%) \
    $(include_mpc8260_HEADERS:include/%=$(PROJECT_INCLUDE)/mpc8260/%)

$(PROJECT_INCLUDE)/mpc8260:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mpc8260/%.h: include/%.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
