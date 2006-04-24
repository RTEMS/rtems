##
## Makefile.am,v 1.1.2.1 2005/10/05 19:26:00 joel Exp
##


PGM = $(ARCH)/leon_greth.rel

C_FILES = leon_greth.c
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
all-local: $(ARCH) $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = leon_greth.c

include $(top_srcdir)/../../../../../../automake/local.am
