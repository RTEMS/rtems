##
## Makefile.am,v 1.12.2.1 2003/02/20 21:57:21 joel Exp
##

include $(top_srcdir)/../../../../automake/compile.am

EXTRA_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../pclock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../irq$(LIB_VARIANT).rel \
    ../pci$(LIB_VARIANT).rel ../startup$(LIB_VARIANT).rel \
    ../tod$(LIB_VARIANT).rel ../vectors$(LIB_VARIANT).rel \
    ../GT64260$(LIB_VARIANT).rel
if HAS_NETWORKING
___libbsp_a_LIBADD += ../network$(LIB_VARIANT).rel
endif
___libbsp_a_LIBADD += ../vme$(LIB_VARIANT).rel
___libbsp_a_LIBADD += \
    ../../../../libcpu/@RTEMS_CPU@/shared/cpuIdent$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/stack$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/exceptions$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/mmu$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/timer$(LIB_VARIANT).rel

EXTRA_LIBRARIES += ../libbsp_g.a
CLEANFILES += ../libbsp_g.a
___libbsp_g_a_SOURCES = $(___libbsp_a_SOURCES)
___libbsp_g_a_LIBADD = $(___libbsp_a_LIBADD)

noinst_DATA = ../libbsp$(LIB_VARIANT).a

include $(top_srcdir)/../../../../automake/local.am
