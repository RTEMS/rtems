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
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard \
    ../$(piece)/$(ARCH)/*.$(OBJEXT))) $(wildcard \
    ../../../../libcpu/$(RTEMS_CPU)/shared/*/$(ARCH)/*.$(OBJEXT)) \
    ../@exceptions@/$(ARCH)/rtems-cpu.rel $(wildcard \
    ../../../../libcpu/$(RTEMS_CPU)/$(RTEMS_CPU_MODEL)/*/$(ARCH)/*.$(OBJEXT))
LIB = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(LIB)

include $(top_srcdir)/../../../../automake/local.am
