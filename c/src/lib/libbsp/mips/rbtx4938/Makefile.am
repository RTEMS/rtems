##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S start/regs.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsppredriverhook.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsplibc.c ../../shared/bsppost.c startup/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c ../../shared/setvec.c \
    startup/inittlb.c \
    startup/idtmem.S startup/idttlb.S startup/exception.S

# FIXME: clockdrv.c and clockdrv_shell.c are structurally flawed
clock_CPPFLAGS = -I$(srcdir)/../../shared
EXTRA_DIST += ../../mips/shared/clockdrv_shell.c
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/clockdrv.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(clock_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

console_SOURCES = console/console-io.c
liblnk_SOURCES = liblnk/lnklib.S liblnk/pmon.S

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(console_SOURCES) $(liblnk_SOURCES)
libbsp_a_LIBADD = \
    clock.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel \
    ../../../libcpu/@RTEMS_CPU@/tx49/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/tx49/vectorisrs.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
