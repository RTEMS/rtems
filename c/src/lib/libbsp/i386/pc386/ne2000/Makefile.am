## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/ne2000.rel

## C source names
C_FILES = ne2000.c
ne2000_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PGM): $(ne2000_rel_OBJECTS)
	$(make-rel)

if HAS_NETWORKING
all-local: ${ARCH} $(PGM)
endif

# the .rel file built here will be put into libbsp.a by 
# 	../wrapup/Makefile

EXTRA_DIST = ne2000.c

include $(top_srcdir)/../../../../../../automake/local.am
