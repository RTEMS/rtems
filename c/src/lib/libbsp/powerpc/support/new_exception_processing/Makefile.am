##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

include $(top_srcdir)/../../../../../automake/compile.am

noinst_PROGRAMS = rtems-cpu.rel
rtems_cpu_rel_SOURCES = cpu.c cpu_asm.S
rtems_cpu_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
rtems_cpu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(top_srcdir)/../../../../../automake/local.am
