##
## $Id$
##

SUBDIRS = . console-generic clock timer exceptions mmu cpm

include_mpc8260dir = $(includedir)/mpc8260

include_HEADERS = include/mpc8260.h
include_mpc8260_HEADERS = include/console.h include/mmu.h include/cpm.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8260/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/mpc8260
	@: > $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8260.h: include/mpc8260.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260.h

$(PROJECT_INCLUDE)/mpc8260/console.h: include/console.h $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/console.h

$(PROJECT_INCLUDE)/mpc8260/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/mmu.h

$(PROJECT_INCLUDE)/mpc8260/cpm.h: include/cpm.h $(PROJECT_INCLUDE)/mpc8260/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8260/cpm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8260/cpm.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
