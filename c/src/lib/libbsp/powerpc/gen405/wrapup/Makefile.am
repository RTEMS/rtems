##
## $Id$
##

OBJS = ../startup$(LIB_VARIANT).rel ../dlentry$(LIB_VARIANT).rel

OBJS += ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/console$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/tty_drv$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/vectors$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/ictrl$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
