##
## $Id$
##

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = ../startup$(LIB_VARIANT).rel ../pclock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../irq$(LIB_VARIANT).rel \
    ../openpic$(LIB_VARIANT).rel ../pci$(LIB_VARIANT).rel \
    ../residual$(LIB_VARIANT).rel ../vectors$(LIB_VARIANT).rel \
    ../motorola$(LIB_VARIANT).rel ../vme$(LIB_VARIANT).rel
OBJS += ../../../../libcpu/@RTEMS_CPU@/shared/cpuIdent$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/stack$(LIB_VARIANT).rel \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/exceptions$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/mmu$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/timer$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
