##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am

include_HEADERS = asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/ppc.h rtems/score/types.h
include_rtems_score_HEADERS += rtems/score/cpu.h

include_rtems_old_exceptionsdir = $(includedir)/rtems/old-exceptions
include_rtems_old_exceptions_HEADERS = rtems/old-exceptions/cpu.h

include_rtems_new_exceptionsdir = $(includedir)/rtems/new-exceptions
include_rtems_new_exceptions_HEADERS = rtems/new-exceptions/cpu.h

include_rtems_powerpcdir = $(includedir)/rtems/powerpc
include_rtems_powerpc_HEADERS = rtems/powerpc/registers.h

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/asm.h: asm.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/asm.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/ppc.h: rtems/score/ppc.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ppc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ppc.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/old-exceptions/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/old-exceptions
	@: > $(PROJECT_INCLUDE)/rtems/old-exceptions/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/old-exceptions/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/old-exceptions/cpu.h: rtems/old-exceptions/cpu.h $(PROJECT_INCLUDE)/rtems/old-exceptions/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/old-exceptions/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/old-exceptions/cpu.h

$(PROJECT_INCLUDE)/rtems/new-exceptions/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/new-exceptions
	@: > $(PROJECT_INCLUDE)/rtems/new-exceptions/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/new-exceptions/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/new-exceptions/cpu.h: rtems/new-exceptions/cpu.h $(PROJECT_INCLUDE)/rtems/new-exceptions/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/new-exceptions/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/new-exceptions/cpu.h

$(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/powerpc
	@: > $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/powerpc/registers.h: rtems/powerpc/registers.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/registers.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/registers.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
