##
## $Id$
##


PGM = $(ARCH)/ne2000.rel

C_FILES = ne2000.c
ne2000_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PGM): $(ne2000_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

if HAS_NETWORKING
all-local: $(ARCH) $(ne2000_rel_OBJECTS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = ne2000.c

include $(top_srcdir)/../../../../../../automake/local.am
