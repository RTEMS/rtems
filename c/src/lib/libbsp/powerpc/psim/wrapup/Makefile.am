##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

OBJS = ../startup$(LIB_VARIANT).rel ../clock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../timer$(LIB_VARIANT).rel \
    ../vectors$(LIB_VARIANT).rel
if HAS_MP
OBJS += ../shmsupp$(LIB_VARIANT).rel
endif
OBJS += ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
