##
## $Id$
##

BSP_PIECES = startup dlentry
# pieces to pick up out of libcpu/ppc
CPU_PIECES = clock timer console tty_drv vectors ictrl

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.$(OBJEXT))) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel \
    $(foreach piece, $(CPU_PIECES), ../../../../libcpu/$(RTEMS_CPU)/ppc403/$(piece)/$(ARCH)/$(piece).rel)

LIB = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
