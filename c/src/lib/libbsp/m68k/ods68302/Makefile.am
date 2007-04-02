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

EXTRA_DIST =

include_HEADERS += include/bare.h
include_HEADERS += include/crc.h
include_HEADERS += include/debugport.h
include_HEADERS += include/m68302scc.h
include_HEADERS += include/coverhd.h

# FIXME: Better merge both files into one and use #ifdef GDB_MONITOR_ACTIVE
if ODS68302_DEBUG
EXTRA_DIST += start/debugreset.S
start.$(OBJEXT): start/debugreset.S
	$(CPPASCOMPILE) -o $@ -c $<
else
EXTRA_DIST += start/reset.S
start.$(OBJEXT): start/reset.S
	$(CPPASCOMPILE) -o $@ -c $<
endif

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/rom startup/debugger

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/cpuboot.c startup/crc.c startup/debugport.c \
    startup/gdb-hooks.c ../../shared/bootcard.c ../../shared/main.c \
    startup/m68302scc.c startup/m68k-stub.c \
    ../../m68k/shared/m68kpretaskinghook.c startup/memcheck.c \
    startup/trace.c ../../shared/gnatinstallhandler.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c startup/bspclean.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c ../../shared/dummy_printk_support.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c timer/timerisr.S
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = startup.rel clock.rel console.rel timer.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
