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

SUBDIRS = . tools

# FIXME: This directory is missing
# ramdisk

include_HEADERS += include/syscall.h
include_HEADERS += include/ramdisk.h
include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds-le \
    startup/linkcmds-le.coff

startup_SOURCES = startup/hw_init.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../shared/bsppretaskinghook.c ../shared/bspgetworkarea.c \
    ../shared/bspstart.c ../../shared/bspclean.c ../../shared/sbrk.c \
    ../../shared/bootcard.c ../../shared/gnatinstallhandler.c \
    ../shared/bsphwinit.c
clock_SOURCES = clock/ckinit.c
console_SOURCES = console/console.c ../../shared/dummy_printk_support.c
timer_SOURCES = timer/timer.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
