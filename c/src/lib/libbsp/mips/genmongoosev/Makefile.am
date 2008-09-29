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

nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S start/regs.h start/mg5.h
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c \
    ../../shared/setvec.c
clock_SOURCES = clock/clockdrv.c
console_SOURCES = console/conscfg.c ../../shared/console.c
timer_SOURCES = timer/timer.c

gdbstub_CPPFLAGS = -I$(srcdir)/../../mips/shared/gdbstub
noinst_PROGRAMS += gdbstub.rel
gdbstub_rel_SOURCES = ../../mips/shared/gdbstub/mips-stub.c \
    startup/gdb-support.c ../../shared/gdbstub/rtems-stub-glue.c
gdbstub_rel_CPPFLAGS = $(AM_CPPFLAGS) $(gdbstub_CPPFLAGS)
gdbstub_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES) $(gdbstub_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel \
    ../../../libcpu/@RTEMS_CPU@/mongoosev/duart.rel \
    ../../../libcpu/@RTEMS_CPU@/mongoosev/vectorisrs.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
