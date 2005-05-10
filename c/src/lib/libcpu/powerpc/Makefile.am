##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
DISTCLEANFILES =

include_rtems_powerpcdir = $(includedir)/rtems/powerpc
include_rtems_powerpc_HEADERS = rtems/powerpc/cache.h \
    rtems/powerpc/debugmod.h rtems/powerpc/powerpc.h

SUBDIRS = shared mpc5xx mpc505 mpc6xx mpc8260 mpc8xx ppc403

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include_rtems_scoredir = $(includedir)/rtems/score

if OLD_EXCEPTIONS
include_rtems_score_HEADERS = old-exceptions/rtems/score/ppc_offs.h

EXTRA_DIST = old-exceptions/TODO old-exceptions/irq_stub.S \
    old-exceptions/ppccache.c

noinst_PROGRAMS = old-exceptions/rtems-cpu.rel
old_exceptions_rtems_cpu_rel_SOURCES = old-exceptions/cpu.c \
    old-exceptions/cpu_asm.S old-exceptions/rtems/score/ppc_offs.h
old_exceptions_rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS) \
    -I$(srcdir)/old-exceptions
old_exceptions_rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
else
noinst_PROGRAMS = new-exceptions/rtems-cpu.rel
new_exceptions_rtems_cpu_rel_SOURCES = new-exceptions/cpu.c \
    new-exceptions/cpu_asm.S
new_exceptions_rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS)
new_exceptions_rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
