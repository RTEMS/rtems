##
## $Id$
##

SUBDIRS = . console-generic clock timer cpm mmu exceptions

include_mpc8xxdir = $(includedir)/mpc8xx

include_HEADERS = include/mpc8xx.h
include_mpc8xx_HEADERS = include/console.h include/cpm.h include/mmu.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8xx/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/mpc8xx
	@: > $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)

$(PROJECT_INCLUDE)/mpc8xx.h: include/mpc8xx.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx.h

$(PROJECT_INCLUDE)/mpc8xx/console.h: include/console.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/console.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/console.h

$(PROJECT_INCLUDE)/mpc8xx/cpm.h: include/cpm.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/cpm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/cpm.h

$(PROJECT_INCLUDE)/mpc8xx/mmu.h: include/mmu.h $(PROJECT_INCLUDE)/mpc8xx/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mpc8xx/mmu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mpc8xx/mmu.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
