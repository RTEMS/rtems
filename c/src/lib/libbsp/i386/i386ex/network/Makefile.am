## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/network.rel

## C source names
C_FILES = network.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES = netexterns.h uti596.h
noinst_HEADERS = $(H_FILES)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): ${OBJS}
	$(make-rel)

if HAS_NETWORKING
all-local: ${ARCH} $(PGM)
else
all-local:
endif

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
