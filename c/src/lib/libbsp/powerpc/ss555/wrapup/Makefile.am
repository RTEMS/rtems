##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =

___libbsp_a_LIBADD = ../pclock.rel ../console.rel ../startup.rel \
    ../tm27supp.rel

___libbsp_a_LIBADD += ../../../../libcpu/powerpc/shared/cpuIdent.rel \
    ../@exceptions@/rtems-cpu.rel \
    ../../../../libcpu/powerpc/mpc5xx/clock.rel \
    ../../../../libcpu/powerpc/mpc5xx/console-generic.rel \
    ../../../../libcpu/powerpc/mpc5xx/exceptions.rel \
    ../../../../libcpu/powerpc/mpc5xx/irq.rel \
    ../../../../libcpu/powerpc/mpc5xx/timer.rel \
    ../../../../libcpu/powerpc/mpc5xx/vectors.rel

include $(top_srcdir)/../../../../automake/local.am
