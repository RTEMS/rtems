##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =

___libbsp_a_LIBADD = ../startup.rel ../clock.rel ../console.rel ../scv64.rel \
    ../timer.rel ../tod.rel
if HAS_NETWORKING
___libbsp_a_LIBADD += ../sonic.rel
endif
___libbsp_a_LIBADD += ../@exceptions@/rtems-cpu.rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache.rel

include $(top_srcdir)/../../../../automake/local.am
