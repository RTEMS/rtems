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

SUBDIRS = mpc5xx mpc505 mpc6xx mpc8260 mpc8xx ppc403

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

# shared/include
if shared
include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = shared/include/io.h shared/include/mmu.h shared/include/page.h \
    shared/include/byteorder.h shared/include/pgtable.h

noinst_PROGRAMS += shared/cpuIdent.rel
shared_cpuIdent_rel_SOURCES = shared/include/cpuIdent.c shared/include/cpuIdent.h
shared_cpuIdent_rel_CPPFLAGS = $(AM_CPPFLAGS)
shared_cpuIdent_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += shared/include/cpuIdent.h

## shared/cache
noinst_PROGRAMS += shared/cache.rel
shared_cache_rel_SOURCES = shared/src/cache.c shared/src/cache_.h \
    ../shared/src/cache_aligned_malloc.c ../shared/src/cache_manager.c \
    ../shared/include/cache.h
shared_cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/shared/src
shared_cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += ../shared/include/cache.h

## shared/stack
noinst_PROGRAMS += shared/stack.rel
shared_stack_rel_SOURCES = shared/src/stack.c shared/include/spr.h shared/src/stackTrace.h
shared_stack_rel_CPPFLAGS = $(AM_CPPFLAGS)
shared_stack_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_libcpu_HEADERS += shared/include/spr.h
include_libcpu_HEADERS += shared/src/stackTrace.h
endif


include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
