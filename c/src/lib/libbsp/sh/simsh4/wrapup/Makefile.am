##
## $Id$
##
## build and install libbsp
##

BSP_PIECES = startup console clock timer

# FIXME: This is missing.
# BSP_PIECES = ramdisk

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/$(piece).rel) \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
