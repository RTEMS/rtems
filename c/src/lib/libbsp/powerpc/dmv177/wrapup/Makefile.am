##
## $Id$
##

OBJS = ../startup$(LIB_VARIANT).rel ../clock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../scv64$(LIB_VARIANT).rel \
    ../timer$(LIB_VARIANT).rel ../tod$(LIB_VARIANT).rel
if HAS_NETWORKING
OBJS += ../sonic$(LIB_VARIANT).rel
endif

OBJS += ../@exceptions@/rtems-cpu$(LIB_VARIANT).rel \
    ../../../../libcpu/@RTEMS_CPU@/shared/cache$(LIB_VARIANT).rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
