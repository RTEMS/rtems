##
## $Id$
##

BSP_PIECES = startup dlentry flashentry
# pieces to pick up out of libcpu/ppc

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/console$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/vectors$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/ppc403/ictrl$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
