##
## $Id$
##

# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
if HAS_NETWORKING
NETWORK = ne2000 wd8003 3c509
endif

BSP_FILES = startup clock console timer $(NETWORK) ide

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../../../../libcpu/i386/cache$(LIB_VARIANT).rel \
    ../../../../libcpu/i386/page$(LIB_VARIANT).rel \
    ../../../../libcpu/i386/score$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
