##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../pclock.rel ../irq.rel ../startup.rel ../console.rel \
    ../vectors.rel
if HAS_NETWORKING
___libbsp_a_LIBADD += ../network.rel
endif
___libbsp_a_LIBADD += ../../../../libcpu/powerpc/shared/cpuIdent.rel \
    ../../../../libcpu/powerpc/shared/cache.rel \
    ../@exceptions@/rtems-cpu.rel \
    ../../../../libcpu/powerpc/mpc8260/clock.rel \
    ../../../../libcpu/powerpc/mpc8260/console-generic.rel \
    ../../../../libcpu/powerpc/mpc8260/cpm.rel \
    ../../../../libcpu/powerpc/mpc8260/mmu.rel \
    ../../../../libcpu/powerpc/mpc8260/timer.rel \
    ../../../../libcpu/powerpc/mpc8260/exceptions.rel

include $(top_srcdir)/../../../../automake/local.am
