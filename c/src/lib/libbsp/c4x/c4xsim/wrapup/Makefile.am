##
## $Id$
##

BSP_PIECES = startup console clock timer
# pieces to pick up out of libcpu/sparc
CPU_PIECES =

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    $(foreach piece, $(CPU_PIECES), ../../../../libcpu/$(RTEMS_CPU)/$(piece)/$(ARCH)/$(piece).rel)
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
