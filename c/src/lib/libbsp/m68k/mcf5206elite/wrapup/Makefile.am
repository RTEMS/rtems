##
## $Id$
##

BSP_PIECES = startup console i2c tod nvram

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../../../../libcpu/$(RTEMS_CPU)/shared/cache$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/shared/misc$(LIB_VARIANT).rel \
    ../../../../libcpu/m68k/mcf5206/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/m68k/mcf5206/console$(LIB_VARIANT).rel \
    ../../../../libcpu/m68k/mcf5206/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/m68k/mcf5206/mbus$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
