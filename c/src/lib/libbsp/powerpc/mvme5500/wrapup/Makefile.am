##
## Makefile.am,v 1.12.2.1 2003/02/20 21:57:21 joel Exp
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../pclock.rel ../console.rel ../irq.rel ../pci.rel \
    ../startup.rel ../tod.rel ../vectors.rel ../GT64260.rel
if HAS_NETWORKING
___libbsp_a_LIBADD += ../network.rel
endif
___libbsp_a_LIBADD += ../vme.rel
___libbsp_a_LIBADD += ../../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../@exceptions@/rtems-cpu.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/clock.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/exceptions.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/mmu.rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/timer.rel

include $(top_srcdir)/../../../../automake/local.am
