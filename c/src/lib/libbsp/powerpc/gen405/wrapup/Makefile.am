##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

EXTRA_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =

___libbsp_a_LIBADD = ../startup$(LIB_VARIANT).rel \
    ../dlentry$(LIB_VARIANT).rel

___libbsp_a_LIBADD += ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/console$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/tty_drv$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/vectors$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/ictrl$(LIB_VARIANT).rel

EXTRA_LIBRARIES += ../libbsp_g.a
CLEANFILES += ../libbsp_g.a
___libbsp_g_a_SOURCES = $(___libbsp_a_SOURCES)
___libbsp_g_a_LIBADD = $(___libbsp_a_LIBADD)

noinst_DATA = ../libbsp$(LIB_VARIANT).a

include $(top_srcdir)/../../../../automake/local.am
