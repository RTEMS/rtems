##
## $Id$
##

OBJS = ../startup$(LIB_VARIANT).rel ../clock$(LIB_VARIANT).rel \
    ../console$(LIB_VARIANT).rel ../timer$(LIB_VARIANT).rel \
    ../tod$(LIB_VARIANT).rel ../vectors$(LIB_VARIANT).rel \
    ../PCI_bus$(LIB_VARIANT).rel

OBJS += ../@exceptions@/$(ARCH)/*.rel

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

$(ARCH)/libbsp.a: $(OBJS)
	$(make-library)

noinst_DATA = $(ARCH)/libbsp.a

include $(top_srcdir)/../../../../automake/local.am
