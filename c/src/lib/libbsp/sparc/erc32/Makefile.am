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

include_HEADERS += include/erc32.h
include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../sparc/shared/start.S
start.$(OBJEXT): ../../sparc/shared/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../sparc/shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bsppost.c ../../sparc/shared/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c startup/setvec.c \
    startup/spurious.c startup/erc32mec.c startup/boardinit.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += gnatsupp.rel
gnatsupp_rel_SOURCES = gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
gnatsupp_rel_CPPFLAGS = $(AM_CPPFLAGS)
gnatsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += debugio.rel
debugio_rel_SOURCES = console/debugputs.c
debugio_rel_CPPFLAGS = $(AM_CPPFLAGS)
debugio_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
erc32sonic_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += erc32sonic.rel
erc32sonic_rel_SOURCES = erc32sonic/erc32sonic.c
erc32sonic_rel_CPPFLAGS = $(AM_CPPFLAGS) $(erc32sonic_CPPFLAGS)
erc32sonic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = console.rel clock.rel timer.rel gnatsupp.rel \
  debugio.rel
if HAS_NETWORKING
libbsp_a_LIBADD += erc32sonic.rel
endif

libbsp_a_LIBADD +=  \
startup_rel-boardinit.o \
startup_rel-bootcard.o \
startup_rel-bspclean.o \
startup_rel-bsplibc.o \
startup_rel-bsppost.o \
startup_rel-bsppredriverhook.o \
startup_rel-bspstart.o \
startup_rel-erc32mec.o \
startup_rel-sbrk.o \
startup_rel-setvec.o \
startup_rel-spurious.o

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/reg_win.rel \
    ../../../libcpu/@RTEMS_CPU@/syscall.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
