##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

if HAS_MP
BSP_MP_O_PIECES = shmsupp
endif
BSP_PIECES = startup clock console timer $(BSP_MP_O_PIECES)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../../../../libcpu/$(RTEMS_CPU)/shared/cache$(LIB_VARIANT).rel \
    ../../../../libcpu/$(RTEMS_CPU)/shared/misc$(LIB_VARIANT).rel

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
