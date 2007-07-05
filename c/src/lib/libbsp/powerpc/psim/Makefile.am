##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = ../shared/clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## Bulk of Console driver with dependencies that entails
noinst_PROGRAMS += console.rel
console_rel_SOURCES = ../../shared/console-polled.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## used for debug IO and bottom of console polled
noinst_PROGRAMS += consoleio.rel
consoleio_rel_SOURCES = console/console-io.c console/consupp.S
consoleio_rel_CPPFLAGS = $(AM_CPPFLAGS)
consoleio_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS = irq/irq.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq.c irq/irq_init.c irq/irq_asm.S
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_DIST += vectors/README

noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = vectors/align_h.S ../../powerpc/shared/vectors/vectors.S \
    ../../powerpc/shared/vectors/vectors_init.c
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h

if HAS_MP
EXTRA_DIST += shmsupp/README

noinst_PROGRAMS += shmsupp.rel
shmsupp_rel_SOURCES = shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c \
    shmsupp/mpisr.c
shmsupp_rel_CPPFLAGS = $(AM_CPPFLAGS)
shmsupp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = pclock.rel consoleio.rel console.rel irq.rel vectors.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

libbsp_a_LIBADD += \
startup_rel-bspclean.o \
startup_rel-bsplibc.o \
startup_rel-bsppost.o \
startup_rel-bspstart.o \
startup_rel-bootcard.o \
startup_rel-sbrk.o \
startup_rel-gnatinstallhandler.o

if HAS_MP
libbsp_a_LIBADD += shmsupp.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
