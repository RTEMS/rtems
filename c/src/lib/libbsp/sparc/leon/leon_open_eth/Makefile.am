##
## Makefile.am,v 1.5 2002/12/14 08:17:45 ralf Exp
##


PGM = $(ARCH)/leon_open_eth.rel

C_FILES = leon_open_eth.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

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

EXTRA_DIST = leon_open_eth.c

include $(top_srcdir)/../../../../../../automake/local.am
