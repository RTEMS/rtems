##
## $Id$
##

BSP_PIECES = clock console irq openpic pci residual startup vectors motorola \
    vme

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.$(OBJEXT)) \
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
