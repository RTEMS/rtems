## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/network.rel

C_FILES = network.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = netexterns.h uti596.h
noinst_HEADERS = $(H_FILES)

network_rel_OBJECTS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PGM): ${network_rel_OBJECTS}
	$(make-rel)

if HAS_NETWORKING
all-local: ${ARCH} $(PGM)
endif

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

EXTRA_DIST = network.c

include $(top_srcdir)/../../../../../../automake/local.am
