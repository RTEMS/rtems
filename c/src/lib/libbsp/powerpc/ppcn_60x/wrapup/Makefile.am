##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

noinst_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../startup.rel ../clock.rel ../console.rel ../timer.rel \
    ../tod.rel ../nvram.rel ../universe.rel ../pci.rel ../vectors.rel
if HAS_NETWORKING
___libbsp_a_LIBADD += ../network.rel
endif
___libbsp_a_LIBADD += ../@exceptions@/rtems-cpu.rel

include $(top_srcdir)/../../../../automake/local.am
