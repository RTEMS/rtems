## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/console.rel

C_FILES = console.c serial.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = concntl.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_INCLUDE)/concntl.h: concntl.h
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/concntl.h

all: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = concntl.h console.c serial.c serial.h

include $(top_srcdir)/../../../../../../automake/local.am
