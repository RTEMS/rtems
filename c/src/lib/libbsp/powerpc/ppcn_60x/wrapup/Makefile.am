##
## $Id$
##

# We only build the network device driver if HAS_NETWORK was defined
if HAS_NETWORKING
NETWORK = network
endif

BSP_PIECES = startup clock console timer tod $(NETWORK) nvram universe pci \
    vectors
# pieces to pick up out of libcpu/$(RTEMS_CPU)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel
LIB = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(LIB)

include $(top_srcdir)/../../../../automake/local.am
