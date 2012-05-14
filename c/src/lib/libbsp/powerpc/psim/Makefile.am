ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_HEADERS += include/psim.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppretaskinghook.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c \
    ../../powerpc/shared/startup/bspgetworkarea.c ../../shared/bootcard.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/gnatinstallhandler.c \
    startup/psim_registers.c

# pclock
libbsp_a_SOURCES += ../shared/clock/p_clock.c
# console
libbsp_a_SOURCES += ../../shared/console-polled.c
# debugio
libbsp_a_SOURCES += console/console-io.c console/consupp.S

# irq
include_bsp_HEADERS = irq/irq.h ../shared/openpic/openpic.h \
    ../../shared/include/irq-generic.h
libbsp_a_SOURCES += irq/irq_init.c ../shared/irq/openpic_i8259_irq.c \
    ../shared/openpic/openpic.c ../../shared/src/irq-server.c  \
    ../../shared/src/irq-generic.c ../../shared/src/irq-default-handler.c

EXTRA_DIST = vectors/README
# vectors
libbsp_a_SOURCES += vectors/align_h.S

include_bsp_HEADERS += ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h

EXTRA_DIST += shmsupp/README
if HAS_MP
# shmdr
libbsp_a_SOURCES += \
    shmsupp/addrconv.c shmsupp/getcfg.c shmsupp/lock.c shmsupp/mpisr.c
endif

if HAS_NETWORKING
libbsp_a_SOURCES += network/if_sim.c
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/altivec.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
