##
## $Id$
##

if HAS_POSIX
POSIX_PIECES = rtems_webserver
endif

NET_O_PIECES = rtems_servers rtems_telnetd pppd $(POSIX_PIECES)
OBJS = $(foreach piece, $(NET_O_PIECES), ../$(piece)/$(ARCH)/*.$(OBJEXT))
LIB = $(ARCH)/libnetapps.a

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

if HAS_NETWORKING
$(LIB): $(OBJS)
	$(make-library)

all-local: $(OBJS) $(LIB)
endif

PREINSTALL_FILES =

include $(top_srcdir)/automake/local.am
