##
## $Id$
##

LIB = $(ARCH)/lib.a

C_FILES = getprotoby.c rtems_bsdnet_ntp.c ftpfs.c syslog.c tftpDriver.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -DNOPOLL -DNOSELECT

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README getprotoby.c rtems_bsdnet_ntp.c syslog.c tftpDriver.c

include $(top_srcdir)/../automake/local.am
