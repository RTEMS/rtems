##
## $Id$
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_tic4xdir = $(includedir)/rtems/tic4x
include_rtems_tic4x_HEADERS = rtems/tic4x/c4xio.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/c4x.h \
    rtems/score/types.h rtems/score/cpu_asm.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c irq.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/asm.h: rtems/asm.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/asm.h

$(PROJECT_INCLUDE)/rtems/tic4x/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/tic4x
	@: > $(PROJECT_INCLUDE)/rtems/tic4x/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/tic4x/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/tic4x/c4xio.h: rtems/tic4x/c4xio.h $(PROJECT_INCLUDE)/rtems/tic4x/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/tic4x/c4xio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/tic4x/c4xio.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/c4x.h: rtems/score/c4x.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/c4x.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/c4x.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/cpu_asm.h: rtems/score/cpu_asm.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu_asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu_asm.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
