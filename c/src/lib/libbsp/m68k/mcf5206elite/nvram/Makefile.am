## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/nvram.rel

C_FILES = nvram.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = nvram.c

include $(top_srcdir)/../../../../../../automake/local.am
