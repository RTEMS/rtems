##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

EXTRA_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =

___libbsp_a_LIBADD = ../pclock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../startup$(LIB_VARIANT).rel 

___libbsp_a_LIBADD += \
    ../../../../libcpu/powerpc/shared/cpuIdent$(LIB_VARIANT).rel \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc5xx/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc5xx/console-generic$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc5xx/exceptions$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc5xx/irq$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc5xx/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc5xx/vectors$(LIB_VARIANT).rel

EXTRA_LIBRARIES += ../libbsp_g.a
CLEANFILES += ../libbsp_g.a
___libbsp_g_a_SOURCES = $(___libbsp_a_SOURCES)
___libbsp_g_a_LIBADD = $(___libbsp_a_LIBADD)

noinst_DATA = ../libbsp$(LIB_VARIANT).a

include $(top_srcdir)/../../../../automake/local.am
