##
## $Id$
##

OBJS = ../startup$(LIB_VARIANT).rel ../canbus$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../irq$(LIB_VARIANT).rel \
    ../pclock$(LIB_VARIANT).rel ../vectors$(LIB_VARIANT).rel
if HAS_NETWORKING
OBJS += ../network$(LIB_VARIANT).rel
endif
OBJS += ../../../../libcpu/@RTEMS_CPU@/shared/cpuIdent$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/cpm$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/console-generic$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/exceptions$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/mmu$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
