##
## $Id$
##

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

BSP_PIECES = startup clock console timer

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/$(ARCH)/*.$(OBJEXT))
LIB = $(ARCH)/libbsp.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
