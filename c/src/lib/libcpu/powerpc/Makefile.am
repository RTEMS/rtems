##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

include_rtems_powerpcdir = $(includedir)/rtems/powerpc
include_rtems_powerpc_HEADERS = rtems/powerpc/cache.h \
    rtems/powerpc/debugmod.h rtems/powerpc/powerpc.h

SUBDIRS = shared mpc5xx mpc505 mpc6xx mpc8260 mpc8xx ppc403

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include_rtems_scoredir = $(includedir)/rtems/score

if OLD_EXCEPTIONS
include_rtems_score_HEADERS = \
    ../../libbsp/powerpc/support/old_exception_processing/rtems/score/ppc_offs.h

EXTRA_DIST = ../../libbsp/powerpc/support/old_exception_processing/TODO \
    ../../libbsp/powerpc/support/old_exception_processing/irq_stub.S \
    ../../libbsp/powerpc/support/old_exception_processing/ppccache.c

noinst_PROGRAMS = old-exceptions/rtems-cpu.rel
old_exceptions_rtems_cpu_rel_SOURCES = \
    ../../libbsp/powerpc/support/old_exception_processing/cpu.c \
    ../../libbsp/powerpc/support/old_exception_processing/cpu_asm.S \
    ../../libbsp/powerpc/support/old_exception_processing/rtems/score/ppc_offs.h
old_exceptions_rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS) \
    -I$(srcdir)/../../libbsp/powerpc/support/old_exception_processing
old_exceptions_rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
else
noinst_PROGRAMS = new-exceptions/rtems-cpu.rel
new_exceptions_rtems_cpu_rel_SOURCES = \
    ../../libbsp/powerpc/support/new_exception_processing/cpu.c \
    ../../libbsp/powerpc/support/new_exception_processing/cpu_asm.S
new_exceptions_rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS)
new_exceptions_rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/powerpc
	@: > $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/powerpc/cache.h: rtems/powerpc/cache.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/cache.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/cache.h

$(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h: rtems/powerpc/debugmod.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/debugmod.h

$(PROJECT_INCLUDE)/rtems/powerpc/powerpc.h: rtems/powerpc/powerpc.h $(PROJECT_INCLUDE)/rtems/powerpc/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/powerpc/powerpc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/powerpc/powerpc.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

if OLD_EXCEPTIONS
$(PROJECT_INCLUDE)/rtems/score/ppc_offs.h: ../../libbsp/powerpc/support/old_exception_processing/rtems/score/ppc_offs.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ppc_offs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ppc_offs.h

endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
