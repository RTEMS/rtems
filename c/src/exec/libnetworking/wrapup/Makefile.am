##
## $Id$
##


NET_O_PIECES = kern lib libc net netinet nfs rtems
OBJS = $(foreach piece, $(NET_O_PIECES), ../$(piece)/$(ARCH)/*.o)
LIB = $(ARCH)/libnetworking.a

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/libnetworking$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

all-local: $(ARCH) $(OBJS) $(LIB)

include $(top_srcdir)/../automake/local.am
