## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/reg_win.rel

S_FILES = window.S
reg_win_rel_OBJECTS = $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

$(PGM): $(reg_win_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

# the .rel file built here will be put into libbsp.a by
#    libbsp/sparc/BSP/wrapup/Makefile

EXTRA_DIST = $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
