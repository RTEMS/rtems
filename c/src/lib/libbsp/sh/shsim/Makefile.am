##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/gdbsci.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.sim

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@
BUILT_SOURCES = scitab.c
CLEANFILES = scitab.c

startup_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c ../shared/bspstart.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bspclean.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/gnatinstallhandler.c ../shared/bsphwinit.c
clock_SOURCES = ../../shared/clock_driver_simidle.c

trap34_SOURCES = trap34/console-io.c trap34/console-support.S \
    ../../shared/dummy_printk_support.c
console_SOURCES = ../../shared/console-polled.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(scitab_SOURCES) $(trap34_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
