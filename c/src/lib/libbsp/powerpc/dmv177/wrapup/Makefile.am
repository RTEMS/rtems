##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING = sonic
endif

# pieces specific to this BSP
BSP_PIECES = startup clock console scv64 timer tod $(NETWORKING)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), \
    ../$(piece)/$(ARCH)/*.$(OBJEXT))
OBJS += ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
