##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

include $(top_srcdir)/../../../../../automake/compile.am

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

all-local: $(TMPINSTALL_FILES)

EXTRA_PROGRAMS += rtems-cpu.rel
CLEANFILES += rtems-cpu.rel
rtems_cpu_rel_SOURCES = cpu.c cpu_asm.S
rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += rtems-cpu_g.rel
CLEANFILES += rtems-cpu_g.rel
rtems_cpu_g_rel_SOURCES = $(rtems_cpu_rel_SOURCES)
rtems_cpu_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
rtems_cpu_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += rtems-cpu$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../../automake/local.am
