## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@/../../shared/comm:@srcdir@/../../shared/io

PGM = $(ARCH)/console.rel

C_FILES = uart.c console.c printk.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = uart.h bspio.h

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

all: $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

include $(top_srcdir)/../../../../../../automake/local.am
