##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_HEADERS += include/psim.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

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

noinst_LIBRARIES = libbsp.a

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    ../../powerpc/shared/startup/bspgetworkarea.c ../../shared/bootcard.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/gnatinstallhandler.c \
    startup/psim_registers.c

pclock_SOURCES = ../shared/clock/p_clock.c
console_SOURCES = ../../shared/console-polled.c
debugio_SOURCES = console/console-io.c console/consupp.S

include_bsp_HEADERS = irq/irq.h
irq_SOURCES = irq/irq_init.c irq/no_pic.c

EXTRA_DIST += vectors/README
vectors_SOURCES = vectors/align_h.S

include_bsp_HEADERS += \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h	\
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h	\
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

if HAS_MP
EXTRA_DIST += shmsupp/README

shmdr_SOURCES = \
    shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c shmsupp/mpisr.c
endif

libbsp_a_SOURCES = $(startup_SOURCES) $(pclock_SOURCES) $(console_SOURCES) \
    $(debugio_SOURCES) $(irq_SOURCES) $(vectors_SOURCES) $(shmdr_SOURCES)

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
