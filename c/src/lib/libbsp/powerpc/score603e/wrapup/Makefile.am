##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am

EXTRA_LIBRARIES = ../libbsp.a
CLEANFILES = ../libbsp.a
___libbsp_a_SOURCES =
___libbsp_a_LIBADD = ../startup$(LIB_VARIANT).rel ../clock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../timer$(LIB_VARIANT).rel \
    ../tod$(LIB_VARIANT).rel ../vectors$(LIB_VARIANT).rel \
    ../PCI_bus$(LIB_VARIANT).rel
___libbsp_a_LIBADD += ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel

EXTRA_LIBRARIES += ../libbsp_g.a
CLEANFILES += ../libbsp_g.a
___libbsp_g_a_SOURCES = $(___libbsp_a_SOURCES)
___libbsp_g_a_LIBADD = $(___libbsp_a_LIBADD)

noinst_DATA = ../libbsp$(LIB_VARIANT).a

include $(top_srcdir)/../../../../automake/local.am
