##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

EXTRA_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../pclock$(LIB_VARIANT).rel ../irq$(LIB_VARIANT).rel \
    ../startup$(LIB_VARIANT).rel ../console$(LIB_VARIANT).rel \
    ../vectors$(LIB_VARIANT).rel
if HAS_NETWORKING
___libbsp_a_LIBADD += ../network$(LIB_VARIANT).rel
endif
___libbsp_a_LIBADD += \
    ../../../../libcpu/powerpc/shared/cpuIdent$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/shared/cache$(LIB_VARIANT).rel \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/console-generic$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/cpm$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/mmu$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/exceptions$(LIB_VARIANT).rel

EXTRA_LIBRARIES += ../libbsp_g.a
CLEANFILES += ../libbsp_g.a
___libbsp_g_a_SOURCES = $(___libbsp_a_SOURCES)
___libbsp_g_a_LIBADD = $(___libbsp_a_LIBADD)

noinst_DATA = ../libbsp$(LIB_VARIANT).a

include $(top_srcdir)/../../../../automake/local.am
