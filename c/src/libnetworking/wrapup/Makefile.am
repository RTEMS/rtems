##
## $Id$
##


if HAS_POSIX
POSIX_PIECES = rtems_webserver
endif

NET_O_PIECES = rtems_servers \
    rtems_telnetd pppd $(POSIX_PIECES)
OBJS = $(foreach piece, $(NET_O_PIECES), ../$(piece)/$(ARCH)/*.o)
LIB = $(ARCH)/libnetworking.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/libnetapps$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/libnetapps$(LIB_VARIANT).a

all-local: $(ARCH) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

include $(top_srcdir)/../../../automake/local.am
