##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING = network
endif

BSP_PIECES = clock irq start startup console vectors $(NETWORKING)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    $(wildcard ../../../../libcpu/powerpc/shared/*/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/console-generic$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/cpm$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/mmu$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/mpc8260/exceptions$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
