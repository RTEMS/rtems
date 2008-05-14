##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += smc/smc.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c \
    startup/bspstart.c startup/exit.c startup/memmap.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
console_SOURCES = console/uart.c ../../shared/console.c
abort_SOURCES = ../shared/abort/abort.c
smc_SOURCES = smc/smc.c smc/smc.h

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(console_SOURCES) $(abort_SOURCES) \
    $(smc_SOURCES)

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
