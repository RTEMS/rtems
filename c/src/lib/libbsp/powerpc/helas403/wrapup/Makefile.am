##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =

___libbsp_a_LIBADD = ../startup.rel ../dlentry.rel ../flashentry.rel

___libbsp_a_LIBADD += ../@exceptions@/rtems-cpu.rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/console.rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/vectors.rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/ictrl.rel

include $(top_srcdir)/../../../../automake/local.am
