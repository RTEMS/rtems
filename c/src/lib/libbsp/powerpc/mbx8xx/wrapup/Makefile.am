##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING = network
endif

BSP_PIECES = clock irq ide startup console vectors $(NETWORKING)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel \
    ../../../../libcpu/powerpc/mpc8xx/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8xx/console-generic$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8xx/cpm$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8xx/mmu$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8xx/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8xx/exceptions$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
