##
## $Id$
##

OBJS = ../pclock$(LIB_VARIANT).rel ../irq$(LIB_VARIANT).rel \
    ../startup$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../vectors$(LIB_VARIANT).rel

if HAS_NETWORKING
OBJS += ../network$(LIB_VARIANT).rel
endif

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS += ../../../../libcpu/powerpc/shared/cpuIdent$(LIB_VARIANT).rel \
    ../../../../libcpu/powerpc/shared/cache$(LIB_VARIANT).rel \
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
