##
## $Id$
##

# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
if HAS_NETWORKING
NETWORK = ne2000 wd8003 3c509
endif

BSP_FILES = startup clock console timer $(NETWORK)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    $(wildcard ../../../../libcpu/i386/$(ARCH)/*.o)
LIB = $(ARCH)/libbsp.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
