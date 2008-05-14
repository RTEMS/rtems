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

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
# XXX ignore tools since it doesn't build
SUBDIRS = .

include_HEADERS += ../../shared/tod.h
include_HEADERS += include/coverhd.h
include_HEADERS += include/page_table.h

nodist_include_HEADERS += $(top_srcdir)/../shared/mvme/mvme16x_hw.h

EXTRA_DIST = ../../m68k/shared/start.S
start.$(OBJEXT): ../../m68k/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppost.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../m68k/shared/m68kpretaskinghook.c \
    startup/page_table.c ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/ckinit.c
console_SOURCES = console/console.c ../../shared/dummy_printk_support.c
timer_SOURCES = timer/timer.c timer/timerisr.S
tod_SOURCES = tod/tod.c

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = ../mvme167/network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif


noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES) $(tod_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif
if HAS_FPSP
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/fpsp.rel
endif

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
