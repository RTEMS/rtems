##
## $Id$
##

# We only build the networking device driver if HAS_NETWORKING was defined
if HAS_NETWORKING
NETWORKING = network
endif

BSP_PIECES = clock irq startup start canbus vectors console $(NETWORKING)
# pieces to pick up out of libcpu/ppc
# CPU_PIECES = mpc860/clock mpc860/timer mpc860/console-generic \
#    mpc860/vectors

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../../../../libcpu/@RTEMS_CPU@/shared/cpuIdent$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel \
    ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/cpm$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/console-generic$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/exceptions$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/mmu$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
