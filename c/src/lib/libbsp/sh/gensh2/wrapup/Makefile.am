##
## $Id$
##
## build and install libbsp
##

BSP_PIECES = startup scitab console

# pieces to pick up out of libcpu/sh/sh7045
CPU_PIECES = clock timer sci score

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/$(piece).rel) \
    $(foreach piece, $(CPU_PIECES), \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/$(piece)$(LIB_VARIANT).rel)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
