##
## Makefile.am,v 1.12.2.1 2003/02/20 21:57:21 joel Exp
##

BSP_PIECES = clock console irq pci startup tod vectors GT64260 network vme

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/mpc6xx/*/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/*/$(ARCH)/*.$(OBJEXT))
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
