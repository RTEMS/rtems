##
## $Id$
##

BSP_FILES = startup console liblnk

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_FILES), ../$(piece)/$(ARCH)/*.$(OBJEXT)) \
    ../../../../libcpu/mips/shared/cache$(LIB_VARIANT).rel \
    ../../../../libcpu/mips/shared/interrupts$(LIB_VARIANT).rel \
    ../../../../libcpu/mips/r46xx/clock$(LIB_VARIANT).rel \
    ../../../../libcpu/mips/r46xx/timer$(LIB_VARIANT).rel \
    ../../../../libcpu/mips/r46xx/vectorisrs$(LIB_VARIANT).rel

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
