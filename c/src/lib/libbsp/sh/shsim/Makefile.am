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

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspstart.c ../../shared/bspclean.c ../../shared/sbrk.c \
    ../../shared/bootcard.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clockdrv.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@
BUILT_SOURCES = scitab.c
CLEANFILES = scitab.c

noinst_PROGRAMS += scitab.rel
scitab_rel_SOURCES = scitab.c
scitab_rel_CPPFLAGS = $(AM_CPPFLAGS)
scitab_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += gdbsci.rel
gdbsci_rel_SOURCES = gdbsci/gdbsci.c
gdbsci_rel_CPPFLAGS = $(AM_CPPFLAGS)
gdbsci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

#
# FIXME: trap34 is unused
#
noinst_PROGRAMS += trap34.rel
trap34_rel_SOURCES = trap34/console-io.c trap34/console-support.S \
    ../../shared/dummy_printk_support.c
trap34_rel_CPPFLAGS = $(AM_CPPFLAGS)
trap34_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = ../../sh/shared/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel clock.rel console.rel gdbsci.rel scitab.rel \
    trap34.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
