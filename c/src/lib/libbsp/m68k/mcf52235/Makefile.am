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

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = startup/bspclean.c ../../shared/bsppredriverhook.c \
    ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../m68k/shared/m68kpretaskinghook.c \
    ../../m68k/shared/m68kbspgetworkarea.c \
    startup/init52235.c startup/bspstart.c startup/cfinit.c\
    ../../shared/bootcard.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/clock.c
console_SOURCES = console/console.c
debugio_SOURCES = console/debugio.c
timer_SOURCES = timer/timer.c

#if HAS_NETWORKING
#network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
#noinst_PROGRAMS += network.rel
#network_rel_SOURCES = network/network.c
#network_rel_CPPFLAGS = $(AM_CPPFLAGS) \
#    $(network_CPPFLAGS)
#network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
#endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(debugio_SOURCES) $(timer_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel
#if HAS_NETWORKING
#libbsp_a_LIBADD += network.rel
#endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
