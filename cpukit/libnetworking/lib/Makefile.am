##
## $Id$
##


LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = getprotoby.c rtems_bsdnet_ntp.c syslog.c tftpDriver.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DNOPOLL -DNOSELECT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README getprotoby.c rtems_bsdnet_ntp.c syslog.c tftpDriver.c

include $(top_srcdir)/../automake/local.am
