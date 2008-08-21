##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = ../gp32/include/bsp.h
include_HEADERS += ../gp32/smc/smc.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = ../gp32/start/start.S
start.$(OBJEXT): ../gp32/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../gp32/startup/bspstart.c ../gp32/startup/bspclean.c startup/memmap.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/bsppredriverhook.c ../../shared/gnatinstallhandler.c
console_SOURCES = ../gp32/console/uart.c ../../shared/console.c
abort_SOURCES = ../shared/abort/abort.c
smc_SOURCES = ../gp32/smc/smc.c ../gp32/smc/smc.h

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(console_SOURCES) $(abort_SOURCES) \
    $(smc_SOURCES)

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
