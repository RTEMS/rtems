##
## $Id$
##


PGM = $(ARCH)/3c509.rel

C_FILES = 3c509.c elink.c
X3c509_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

noinst_HEADERS = 3c509.h elink.h

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PGM): $(X3c509_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

if HAS_NETWORKING
all-local: $(ARCH) $(X3c509_rel_OBJECTS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = 3c509.c 3c509.h elink.c elink.h

include $(top_srcdir)/../../../../../../automake/local.am
