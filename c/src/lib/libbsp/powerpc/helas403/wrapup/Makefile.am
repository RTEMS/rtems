##
## $Id$
##

BSP_PIECES = startup dlentry flashentry
# pieces to pick up out of libcpu/ppc
CPU_PIECES = clock timer console vectors ictrl

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel $(foreach piece, $(CPU_PIECES), \
    ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/$(piece)/$(ARCH)/$(piece).rel)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
