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

include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S start/start.ram start/start.rom
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.ram \
    startup/linkcmds.rom

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/hw_init.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c startup/bspclean.c \
    ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@
BUILT_SOURCES = scitab.c
CLEANFILES = scitab.c

noinst_PROGRAMS += scitab.rel
scitab_rel_SOURCES = scitab.c
scitab_rel_CPPFLAGS = $(AM_CPPFLAGS)
scitab_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += debugio.rel
debugio_rel_SOURCES = ../../shared/dummy_printk_support.c
debugio_rel_CPPFLAGS = $(AM_CPPFLAGS)
debugio_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = ../../sh/shared/console.c console/config.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel scitab.rel console.rel debugio.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/sci.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
