##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

include $(top_srcdir)/../../../../../automake/compile.am

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/ppc_offs.h

AM_CPPFLAGS += -I$(srcdir)

EXTRA_DIST = TODO irq_stub.S ppccache.c

noinst_PROGRAMS = rtems-cpu.rel
rtems_cpu_rel_SOURCES = cpu.c cpu_asm.S rtems/score/ppc_offs.h
rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS)
rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/ppc_offs.h: rtems/score/ppc_offs.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ppc_offs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ppc_offs.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../../../automake/local.am
