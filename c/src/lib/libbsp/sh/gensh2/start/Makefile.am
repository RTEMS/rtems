## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGMS = ${ARCH}/start.o

## Assembly source names
S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

OBJS = $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: ${ARCH} $(OBJS)
	@$(INSTALL_VARIANT) -m 755 ${PGMS} $(PROJECT_RELEASE)/lib

EXTRA_DIST = $(S_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
