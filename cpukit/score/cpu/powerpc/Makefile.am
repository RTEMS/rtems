##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

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

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/asm.h: asm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/asm.h

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/score
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score

$(PROJECT_INCLUDE)/rtems/score/ppc.h: rtems/score/ppc.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ppc.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/old-exceptions:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/old-exceptions
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/old-exceptions

$(PROJECT_INCLUDE)/rtems/old-exceptions/cpu.h: rtems/old-exceptions/cpu.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/old-exceptions/cpu.h

$(PROJECT_INCLUDE)/rtems/new-exceptions:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/new-exceptions
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/new-exceptions

$(PROJECT_INCLUDE)/rtems/new-exceptions/cpu.h: rtems/new-exceptions/cpu.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/new-exceptions/cpu.h

$(PROJECT_INCLUDE)/rtems/powerpc:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/powerpc
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc

$(PROJECT_INCLUDE)/rtems/powerpc/registers.h: rtems/powerpc/registers.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/registers.h

include $(top_srcdir)/../../../automake/local.am
