##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING_DRIVER = network
endif

BSP_PIECES = startup clock console timer tod $(NETWORKING_DRIVER)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../consolex/$(ARCH)/consolex.rel) \
    ../../../../libcpu/$(RTEMS_CPU)/shared/cache$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/shared/misc$(LIB_VARIANT).rel

if HAS_FPSP
OBJS += ../../../../libcpu/m68k/@RTEMS_CPU_MODEL@/fpsp$(LIB_VARIANT).rel
endif

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
