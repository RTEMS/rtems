## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/start.o

## Assembly source names
S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: ${ARCH} $(PGM)
	$(INSTALL_VARIANT) -m 755 ${PGM} $(PROJECT_RELEASE)/lib

include $(top_srcdir)/../../../../../../automake/local.am
