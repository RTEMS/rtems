##
## $Id$
##

# We only build the network device driver if HAS_NETWORK was defined
if HAS_NETWORKING
NETWORK = network
endif

BSP_PIECES = startup clock console timer tod $(NETWORK) nvram universe pci \
    vectors

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
