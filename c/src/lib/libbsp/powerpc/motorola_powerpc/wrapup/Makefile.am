##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../startup.rel ../pclock.rel ../console.rel ../irq.rel \
    ../openpic.rel ../pci.rel ../residual.rel ../vectors.rel ../motorola.rel \
    ../tod.rel ../vme.rel
___libbsp_a_LIBADD += ../../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../@exceptions@/rtems-cpu.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/clock.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/exceptions.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/mmu.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/timer.rel

include $(top_srcdir)/../../../../automake/local.am
