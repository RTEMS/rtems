##
## $Id$
##


PGM = $(ARCH)/network.rel

C_FILES = network.c if_hdlcsubr.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_NETWORKING
all-local: $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = network.c

include $(top_srcdir)/../../../../automake/local.am
