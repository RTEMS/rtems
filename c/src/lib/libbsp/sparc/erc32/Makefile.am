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

SUBDIRS = . tools

include_HEADERS += include/erc32.h
include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../sparc/shared/start.S
start.$(OBJEXT): ../../sparc/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../sparc/shared/bspgetworkarea.c \
    ../../shared/bsppost.c ../../sparc/shared/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c startup/setvec.c \
    startup/spurious.c startup/erc32mec.c startup/boardinit.S
gnatsupp_SOURCES = gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
console_SOURCES = console/console.c
debugio_SOURCES = console/debugputs.c
clock_SOURCES = clock/ckinit.c
timer_SOURCES = timer/timer.c

if HAS_NETWORKING
erc32sonic_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += erc32sonic.rel
erc32sonic_rel_SOURCES = erc32sonic/erc32sonic.c
erc32sonic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(erc32sonic_CPPFLAGS)
erc32sonic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_SOURCES = $(startup_SOURCES) $(gnatsupp_SOURCES) $(console_SOURCES) \
    $(debugio_SOURCES) $(clock_SOURCES) $(timer_SOURCES)

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel
if HAS_NETWORKING
libbsp_a_LIBADD += erc32sonic.rel
endif

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
